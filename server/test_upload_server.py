# Serveur de test de P2.3 et P2.5 : reçoit en POST /upload le corps envoyé par la carte.
#
# Lancement, depuis la racine du dépôt :
#   server\.venv\Scripts\python server\test_upload_server.py
# Arrêt : Ctrl+C.
#
# Deux corps acceptés, distingués par leurs 2 premiers octets :
# - photo JPEG (P2.5), qui commence par FF D8 : doit contenir FF D9 (fin d'image) ; enregistrée
#   telle que reçue (octets de remplissage après FF D9 compris) dans
#   server/output/photo_<AAAAMMJJ-HHMMSS-mmm>_<taille>.jpg (dossier ignoré par Git) ;
# - motif de test (P2.3, http_test() dans firmware/src/components/http.cpp) :
#   octet i = ((i * 31 + 7) ^ (i >> 8)) & 0xFF
# Réponses : 200 photo enregistrée ou motif conforme, 422 ni JPEG complet ni motif conforme,
# 400 Content-Length invalide, 411 Content-Length absent, 413 corps trop gros, 404 autre chemin.
# Délai : un envoi est abandonné après SOCKET_TIMEOUT_S sans aucun octet reçu. Ce n'est pas une
# limite de durée totale : un envoi lent mais continu n'est jamais coupé.
# Chaque envoi est traité dans son propre fil d'exécution : un envoi coupé par un redémarrage de
# la carte ne bloque pas les suivants pendant SOCKET_TIMEOUT_S.
# Pas d'authentification ni de TLS : serveur de test, à lancer seulement pendant les essais.

import hashlib
import time
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

PORT = 8000
MAX_BODY_BYTES = 1024 * 1024
READ_CHUNK_BYTES = 4096
SOCKET_TIMEOUT_S = 30
OUTPUT_DIR = Path(__file__).parent / "output"
JPEG_START = b"\xff\xd8"
JPEG_END = b"\xff\xd9"


# Motif attendu sur length octets
def expected_bytes(length):
    return bytes(((i * 31 + 7) ^ (i >> 8)) & 0xFF for i in range(length))


class UploadHandler(BaseHTTPRequestHandler):
    # Un envoi bloqué est abandonné au bout de SOCKET_TIMEOUT_S sans activité
    timeout = SOCKET_TIMEOUT_S

    # Réponse en texte simple
    def reply(self, code, text):
        body = (text + "\n").encode()
        self.send_response(code)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_POST(self):
        if self.path != "/upload":
            self.reply(404, "chemin inconnu")
            return

        # Validation de Content-Length avant toute lecture du corps
        length_header = self.headers.get("Content-Length")
        if length_header is None:
            self.reply(411, "Content-Length obligatoire")
            return
        try:
            length = int(length_header)
        except ValueError:
            length = 0
        if length <= 0:
            self.reply(400, "Content-Length invalide")
            return
        if length > MAX_BODY_BYTES:
            self.reply(413, f"taille maximale : {MAX_BODY_BYTES} octets")
            return

        # Lecture bornée à length octets, par blocs
        body = bytearray()
        start = time.perf_counter()
        while len(body) < length:
            chunk = self.rfile.read(min(READ_CHUNK_BYTES, length - len(body)))
            if not chunk:
                break
            body += chunk
        duration_s = time.perf_counter() - start

        if len(body) < length:
            print(f"reception interrompue : {len(body)} / {length} octets ({self.client_address[0]})")
            return

        if body.startswith(JPEG_START):
            self.save_photo(body)
        else:
            self.check_pattern(body, duration_s)

    # Photo : FF D9 présent, puis enregistrement telle que reçue
    def save_photo(self, body):
        if JPEG_END not in body:
            print(f"photo refusee : FF D9 absent ({len(body)} octets)")
            self.reply(422, "JPEG incomplet : FF D9 absent")
            return
        now = datetime.now()
        path = OUTPUT_DIR / f"photo_{now:%Y%m%d-%H%M%S}-{now.microsecond // 1000:03d}_{len(body)}.jpg"
        OUTPUT_DIR.mkdir(exist_ok=True)
        path.write_bytes(body)
        print(f"{now:%H:%M:%S} photo recue : {len(body)} octets -> {path.name}")
        self.reply(200, f"{len(body)} octets recus, photo enregistree")

    # Motif de P2.3 : empreinte et comparaison au motif attendu
    def check_pattern(self, body, duration_s):
        pattern_ok = body == expected_bytes(len(body))
        verdict = "conforme" if pattern_ok else "non conforme (ni JPEG ni motif de test)"
        print(f"taille recue : {len(body)} octets")
        print(f"sha-256 : {hashlib.sha256(body).hexdigest()}")
        print(f"duree de reception : {duration_s * 1000:.0f} ms")
        print(f"debit : {len(body) / 1024 / duration_s:.2f} ko/s")
        print(f"motif : {verdict}")
        self.reply(200 if pattern_ok else 422, f"{len(body)} octets recus, motif {verdict}")


if __name__ == "__main__":
    server = ThreadingHTTPServer(("0.0.0.0", PORT), UploadHandler)
    print(f"ecoute sur 0.0.0.0:{PORT}, POST /upload (Ctrl+C pour arreter)")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    server.server_close()
