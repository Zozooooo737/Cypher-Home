# Fiche de test : envoi HTTP de la carte vers le PC

## En-tête

| | |
|---|---|
| Composant | Envoi HTTP (POST) par `WiFiClient` de WiFiS3, module Wi-Fi intégré de l'Arduino UNO R4 WiFi (ESP32-S3) |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.1 |
| Étape du plan | P2.3 |
| Date du test | 2026-11-11 |
| Datasheet | N/A (protocole HTTP/1.1 ; carte : voir `wifi.md`) |
| **Verdict** | **OK** |

## Alimentation

N/A : module Wi-Fi intégré à la carte (voir `wifi.md`).

## Câblage et réseau testés

Aucune broche des connecteurs : la carte parle à l'ESP32-S3 par `Serial2` (liaison interne).

| Élément | Valeur |
|---|---|
| Réseau | partage de connexion de l'iPhone, 2,4 GHz, « Maximiser la compatibilité » activé |
| Carte | 172.20.10.2 (attribuée par l'iPhone) |
| PC | 172.20.10.3, `server/test_upload_server.py` à l'écoute sur 0.0.0.0:8000 |
| Pare-feu Windows | règle entrante « Cypher Home - HTTP 8000 », TCP 8000, profil privé |
| Requête | `POST /upload HTTP/1.1`, en-têtes Host, Content-Length et Connection: close, corps brut |

- Réseau du test à la date ci-dessus : l'IP du PC change à chaque session (`HTTP_SERVER_IP` dans `secrets.h`).

## Logiciel

- Bibliothèque : WiFiS3 (cœur Renesas 1.6.0), requête HTTP écrite à la main, sans bibliothèque HTTP. Ligne `lib_deps` : aucune. Compatibilité UNO R4 WiFi : oui.
- Fichiers du dépôt : `firmware/src/components/http.cpp` ; serveur `server/test_upload_server.py` (Python 3.15.2, bibliothèque standard, `.venv` de `server/`).
- Paramètres clés (`config.h`) : port 8000, chemin `/upload`, délais de connexion et de réponse de 5 s, taille de bloc `HTTP_BLOCK_BYTES` (512 octets pour les mesures de référence ; 1024 et 2048 essayés ; 2048 retenu le 2026-11-11).
- Corps de test : généré bloc par bloc sur la carte, sans tampon de la taille totale ; octet i = ((i × 31 + 7) XOR (partie entière de i / 256)) modulo 256.
- Contrôles du serveur : Content-Length obligatoire (411), entier positif (400), 512 ko au plus (413), lecture bornée, SHA-256, comparaison au motif attendu (200 conforme, 422 non conforme).
- Débit : taille / durée, 1 ko = 1024 octets. Durée carte : de la connexion à la réception du code HTTP. Durée serveur : réception du corps seul.
- Alternatives écartées : motif (i × 31 + 7) modulo 256 (période de 256 octets : tous les blocs de 512 octets identiques, un bloc perdu compensé par un doublon passerait inaperçu) ; framework web côté serveur (D5 et P5.2 non tranchés).

## Résultat

- Intégrité : à chaque passe, les 4 tailles sont reçues intactes : taille exacte, motif conforme, SHA-256 identiques aux empreintes de référence calculées sur le PC (16 ko `2cf6d7e5…`, 32 ko `f2fde589…`, 64 ko `7bc6ec89…`, 128 ko `6f2a3bf1…`).
- Mesures avec des blocs de 512 octets (plages minimum–maximum sur toutes les passes) :

| Taille | Durée carte | Débit carte | Réception serveur | Débit serveur |
|---|---|---|---|---|
| 16 384 octets | 1707–1733 ms | 9,23–9,37 ko/s | 1577–1642 ms | 9,74–10,15 ko/s |
| 32 768 octets | 3229–3257 ms | 9,82–9,91 ko/s | 3153–3185 ms | 10,05–10,15 ko/s |
| 65 536 octets | 6337–6439 ms | 9,94–10,10 ko/s | 6200–6371 ms | 10,05–10,32 ko/s |
| 131 072 octets | 12 490 ms | 10,25 ko/s | 12 415–12 418 ms | 10,31 ko/s |

- Taille de bloc, 64 ko côté carte : 512 octets 6337–6439 ms ; 1024 octets 6138 ms (10,43 ko/s) ; 2048 octets 6040 ms (10,60 ko/s). 128 ko avec des blocs de 2048 octets, côté serveur : 11 798 et 11 890 ms (10,85 et 10,77 ko/s). Gain d'environ 5 %.
- Cas d'échec : serveur arrêté → `[http] KO` puis `code erreur -1` ; règle de pare-feu désactivée → `-1`, retour à 200 une fois réactivée ; serveur arrêté en plein envoi de 128 ko → `-3` (2 fois).
- Sortie type de `http_test()` (blocs de 512 octets) :

```
[http] serveur ................. 172.20.10.3:8000
[http] taille .................. 131072 octets
[http] code http ............... 200
[http] duree ................... 12490 ms
[http] debit ................... 10.25 ko/s
```

- Photo ou mesure : N/A

## Pièges et limites

- Débit plafonné vers 10 à 11 ko/s, quelle que soit la taille de bloc : proche du maximum de la liaison série interne carte–ESP32-S3 à 115200 bauds (11,25 ko/s) [H : cause tirée du code de WiFiS3, non mesurée]. Estimation pour une photo 1024x768 (67 780 octets, `camera.md`) : environ 6 à 6,5 s d'envoi (D9, à confirmer en P2.5).
- La carte est bloquée pendant tout l'envoi (12,5 s pour 128 ko).
- Serveur arrêté en plein envoi : la carte ne le voit qu'à la fin (code -3, pas -2) ; [H] le module Wi-Fi accepte les blocs même après la coupure.
- Deux envois interrompus pendant les manipulations : le serveur les abandonne au bout de 30 s (« Request timed out ») et accepte les suivants.
- Le serveur de test répond en HTTP/1.0 : la carte lit le code dans les deux versions.
- Pas d'authentification ni de TLS (P10) ; port 8000 ouvert en profil privé seulement ; serveur lancé seulement pendant les essais.
- Non testé : codes 400, 411, 413 et 422 avec la carte (vérifiés seulement en local, par un script de Claude Code) ; durée avant le KO pour le code -1.

## À reporter (si applicable)

- [x] Envoi HTTP et exception d'affichage (http) → `DECISIONS.md`
- [x] Taille de bloc retenue (2048 octets) → `config.h` et `DECISIONS.md`
- [ ] Éléments pour le compte rendu (P12.1) : tableau des débits, schéma du réseau (à faire)
