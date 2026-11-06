"""Moniteur série Cypher Home : affiche le texte de la carte et enregistre les images.

Première fois (PowerShell, à la racine du dépôt) :
    py -m venv firmware\\scripts\\.venv
    firmware\\scripts\\.venv\\Scripts\\python -m pip install -r firmware\\scripts\\requirements.txt
Lancement :
    firmware\\scripts\\.venv\\Scripts\\python firmware\\scripts\\serial_monitor.py
Options : --port COM5 (si la détection échoue), --baud 115200.
Arrêter avec Ctrl+C, notamment avant chaque téléversement.
"""

import argparse
import sys
import time
from datetime import datetime
from pathlib import Path

import serial
from serial.tools import list_ports

DEFAULT_BAUD = 460800

ARDUINO_VID = 0x2341
READ_TIMEOUT_S = 2
MAX_IMAGE_BYTES = 8 * 1024 * 1024
OUTPUT_DIR = Path(__file__).resolve().parent.parent / "output"


def find_port():
    """Renvoie le premier port d'identifiant USB Arduino, ou None."""
    for port in list_ports.comports():
        if port.vid == ARDUINO_VID:
            return port.device
    return None


def is_printable(text):
    """Vrai si la ligne ne contient ni caractère de contrôle ni octet illisible."""
    return "�" not in text and all(c >= " " or c == "\t" for c in text)


def parse_header(line):
    """Lit « IMG_BEGIN <taille> <L>x<H> » ; renvoie la taille, ou None si l'en-tête est invalide."""
    parts = line.split()
    try:
        size = int(parts[1])
        width, height = (int(v) for v in parts[2].split("x"))
    except (IndexError, ValueError):
        return None
    if len(parts) != 3 or not 0 < size <= MAX_IMAGE_BYTES:
        return None
    return size


def read_image(ser, size):
    """Lit exactement size octets, par morceaux ; None si la carte se tait."""
    data = bytearray()
    while len(data) < size:
        chunk = ser.read(size - len(data))
        if not chunk:
            print(f"[moniteur] Image interrompue : {len(data)} octets sur {size}", flush=True)
            return None
        data += chunk
    return bytes(data)


def save_image(jpeg):
    """Enregistre le JPEG dans firmware/output/ sous le nom cam_<AAAAMMJJ-HHMMSS>.jpg."""
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    path = OUTPUT_DIR / f"cam_{stamp}.jpg"
    path.write_bytes(jpeg)
    return path


def receive_image(ser, header_line):
    """Reçoit l'image annoncée, la contrôle et l'enregistre."""
    size = parse_header(header_line)
    if size is None:
        print(f"[moniteur] En-tête d'image invalide : {header_line}", flush=True)
        return
    data = read_image(ser, size)
    if data is None:
        return

    end_line = ser.readline().decode("ascii", errors="replace").strip()
    if not end_line.startswith("IMG_END"):
        print("[moniteur] IMG_END absent à la position annoncée (octet manquant ou en trop ?)", flush=True)
        return
    parts = end_line.split()
    transfer_ms = parts[1] if len(parts) > 1 else "?"

    end = data.rfind(b"\xff\xd9")
    jpeg = data[:end + 2] if end >= 0 else data
    path = save_image(jpeg)
    print(f"[moniteur] {path.name} : {len(jpeg)} octets, transfert carte {transfer_ms} ms", flush=True)


def monitor(ser):
    """Affiche le texte et reçoit les images jusqu'à la perte du port."""
    while True:
        raw = ser.readline()
        if not raw:
            continue  # délai de lecture écoulé sans donnée
        line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
        if line.startswith("IMG_BEGIN"):
            receive_image(ser, line)
        elif is_printable(line):
            print(line, flush=True)


def main():
    parser = argparse.ArgumentParser(description="Moniteur série Cypher Home")
    parser.add_argument("--port", help="port série, par exemple COM5")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD, help="débit, égal à SERIAL_BAUD de config.h")
    args = parser.parse_args()

    if args.baud == 1200:
        sys.exit("Débit de 1200 bauds refusé (passage de la carte en mode téléversement).")

    print(f"[moniteur] Images enregistrées dans {OUTPUT_DIR}", flush=True)
    last_error = None
    while True:
        port = args.port or find_port()
        try:
            if port is None:
                raise serial.SerialException("aucune carte Arduino détectée")
            with serial.Serial(port, args.baud, timeout=READ_TIMEOUT_S) as ser:
                print(f"[moniteur] Connecté à {port} ({args.baud} bauds). Ctrl+C pour quitter.", flush=True)
                last_error = None
                monitor(ser)
        except (serial.SerialException, OSError) as error:
            message = f"[moniteur] Port {port or '?'} indisponible : {error}. Nouvel essai chaque seconde..."
            if message != last_error:
                print(message, flush=True)
                last_error = message
            time.sleep(1)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[moniteur] Arrêt (Ctrl+C).")
