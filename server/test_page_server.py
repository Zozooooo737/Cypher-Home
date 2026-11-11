# Serveur de test de P2.4 : une page HTML servie par le PC, à ouvrir depuis un iPhone connecté au partage de connexion.
#
# Lancement, depuis la racine du dépôt :
#   server\.venv\Scripts\python server\test_page_server.py
# Arrêt : Ctrl+C.
#
# Même port que test_upload_server.py (8000) : ne pas lancer les deux serveurs en même temps.
# Seule la page / est servie en GET ; aucun fichier du disque n'est lu.
# Pas d'authentification ni de TLS : serveur de test provisoire, sans lien avec le futur dashboard (D5).

import socket
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

PORT = 8000

PAGE = """<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Cypher Home - test P2.4</title>
</head>
<body>
<h1>Cypher Home - test P2.4</h1>
<p>Heure du serveur : {time}</p>
<p>Adresse IP du client : {client_ip}</p>
</body>
</html>
"""


# IP du PC sur le réseau actif : un socket UDP « connecté » choisit l'interface de sortie, sans envoyer de paquet
def local_ip():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]


class PageHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != "/":
            self.send_error(404)
            return
        body = PAGE.format(time=datetime.now().strftime("%H:%M:%S"), client_ip=self.client_address[0]).encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    # Autres méthodes courantes : 405 (une méthode inconnue reçoit 501 de http.server)
    def refuse_method(self):
        self.send_error(405)

    do_POST = do_PUT = do_DELETE = do_PATCH = do_HEAD = do_OPTIONS = refuse_method


if __name__ == "__main__":
    # ThreadingHTTPServer : un navigateur peut ouvrir une connexion à l'avance sans rien y envoyer ;
    # HTTPServer, qui traite une connexion à la fois, l'attendrait indéfiniment.
    server = ThreadingHTTPServer(("0.0.0.0", PORT), PageHandler)
    print(f"ecoute sur 0.0.0.0:{PORT} (Ctrl+C pour arreter), pages a tester :")
    print(f"  http://localhost:{PORT}")
    print(f"  http://{local_ip()}:{PORT}")
    print(f"  http://{socket.gethostname()}.local:{PORT}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    server.server_close()
