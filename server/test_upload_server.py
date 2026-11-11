# Serveur de test de P2.3 : reçoit en POST /upload le corps envoyé par la carte et vérifie qu'il est intact.
#
# Lancement, depuis la racine du dépôt :
#   server\.venv\Scripts\python server\test_upload_server.py
# Arrêt : Ctrl+C.
#
# Corps attendu (motif de http_test() dans firmware/src/components/http.cpp) :
#   octet i = ((i * 31 + 7) ^ (i >> 8)) & 0xFF
# Réponses : 200 motif conforme, 422 motif non conforme, 400 Content-Length invalide,
# 411 Content-Length absent, 413 corps trop gros, 404 autre chemin.
# Pas d'authentification ni de TLS : serveur de test, à lancer seulement pendant les essais.

import hashlib
import time
from http.server import BaseHTTPRequestHandler, HTTPServer

PORT = 8000
MAX_BODY_BYTES = 512 * 1024
READ_CHUNK_BYTES = 4096
SOCKET_TIMEOUT_S = 30


# Octets attendus aux positions start à start + length - 1
def expected_bytes(start, length):
    return bytes(((i * 31 + 7) ^ (i >> 8)) & 0xFF for i in range(start, start + length))


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

        # Lecture bornée à length octets, par blocs, avec empreinte et contrôle du motif au fil de l'eau
        sha256 = hashlib.sha256()
        received = 0
        pattern_ok = True
        start = time.perf_counter()
        while received < length:
            chunk = self.rfile.read(min(READ_CHUNK_BYTES, length - received))
            if not chunk:
                break
            sha256.update(chunk)
            pattern_ok = pattern_ok and chunk == expected_bytes(received, len(chunk))
            received += len(chunk)
        duration_s = time.perf_counter() - start

        if received < length:
            print(f"reception interrompue : {received} / {length} octets ({self.client_address[0]})")
            return

        verdict = "conforme" if pattern_ok else "non conforme"
        print(f"taille recue : {received} octets")
        print(f"sha-256 : {sha256.hexdigest()}")
        print(f"duree de reception : {duration_s * 1000:.0f} ms")
        print(f"debit : {received / 1024 / duration_s:.2f} ko/s")
        print(f"motif : {verdict}")
        self.reply(200 if pattern_ok else 422, f"{received} octets recus, motif {verdict}")


if __name__ == "__main__":
    server = HTTPServer(("0.0.0.0", PORT), UploadHandler)
    print(f"ecoute sur 0.0.0.0:{PORT}, POST /upload (Ctrl+C pour arreter)")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    server.server_close()
