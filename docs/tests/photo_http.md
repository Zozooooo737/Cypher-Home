# Fiche de test : photo de bout en bout, carte vers PC en HTTP

## En-tête

| | |
|---|---|
| Composant | Caméra OV5642 (ArduCAM Mini 5MP Plus) → envoi HTTP par `WiFiClient` de WiFiS3 → serveur de test du PC |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.1 (carte), § 3.3 (caméra) |
| Étape du plan | P2.5 |
| Date du test | 2026-11-11 |
| Datasheet | N/A (voir `camera.md` et `http.md`) |
| **Verdict** | **OK avec réserve** |

Réserve : ouverture visuelle des images et correctif du serveur (un fil d'exécution par envoi) à confirmer sur la carte.

## Alimentation

N/A : carte sur USB, caméra alimentée comme dans `camera.md`.

## Câblage et réseau testés

| Élément | Valeur |
|---|---|
| Caméra | câblage de `pins.h` à la date du test : CS sur D10, SPI sur D11 à D13, I2C sur SDA/SCL ; SPI à 4 MHz |
| Réseau | partage de connexion de l'iPhone, 2,4 GHz |
| Carte | 172.20.10.2 (adresse vue par le serveur) |
| PC | `server/test_upload_server.py` sur 0.0.0.0:8000, règle de pare-feu « Cypher Home - HTTP 8000 » (profil privé) |
| Requête | `POST /upload HTTP/1.1`, Content-Length = longueur lue dans le tampon de la caméra, corps par blocs de 2048 octets (`HTTP_BLOCK_BYTES`) |

- Conditions : scène fixe ; éclairage : à compléter ; débit série 460800 bauds (`SERIAL_BAUD`).
- Cette table est le câblage **testé** à la date ci-dessus. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Fichiers du dépôt : `firmware/src/components/camera.cpp` (`camera_http_test()`, banc `camera_http_bench()`), `firmware/src/components/http.cpp` (`http_upload()`), `server/test_upload_server.py`.
- Banc : 7 résolutions × 3 envois, 1 s de stabilisation après chaque changement de résolution, retour à la résolution de `config.h` en fin de passe ; lancé par `loop()` à la place de `self_test_run()` pendant les mesures. Envoi unique à la résolution de `config.h` (`camera_http_test()`) ajouté ensuite à `self_test_run()`.
- Mesures (`millis()`) : capture = `camera_capture()` ; envoi = de la lecture du tampon de la caméra jusqu'à la réponse du serveur ; total = capture + envoi ; débit = taille / envoi (1 ko = 1024 octets) ; taille = octets envoyés.
- Serveur : FF D8 en tête, FF D9 présent, taille reçue = Content-Length, 1 Mo au plus ; photo enregistrée telle que reçue dans `server/output/` (ignoré par Git) ; un fil d'exécution par envoi.
- Résolution retenue (D9) : `OV5642_640x480`, la plus grande dont l'envoi reste sous la période de mesure de 5 s du CC (2,6 à 3,2 s mesurés, 27 à 33 ko). 1024x768, choix provisoire de P1.1 : 5,3 à 5,9 s mesurés.
- Alternatives écartées : module `photo_http` séparé (aurait exposé la table des résolutions de `camera.cpp`) ; script serveur dédié (un seul serveur sur le port 8000, qui reçoit aussi le motif de P2.3).

## Résultat

- Intégrité : chaque photo envoyée est arrivée sur le serveur, y compris celles des envois en échec ; contrôle par script des 115 photos du soir : FF D8 en tête, FF D9 présent, dimensions du marqueur SOF = résolution demandée, taille du fichier = taille annoncée, toutes conformes. Ouverture visuelle : à compléter.
- Médianes de la passe de 22:27 (chaque colonne calculée séparément) et estimation du pire cas :

| Résolution | Taille JPEG (octets) | Capture (ms) | Envoi (ms) | Total (ms) | Débit (ko/s) | Taille max `camera.md` (octets) | Envoi estimé (s) |
|---|---|---|---|---|---|---|---|
| 320x240 * | 10 256 | 157 | 2 949 | 3 106 | 5,44 | 8 367 | 0,9 |
| 640x480 | 27 256 | 198 | 2 632 | 2 859 | 10,11 | 26 396 | 2,5 |
| 1024x768 | 56 408 | 181 | 5 304 | 5 506 | 10,44 | 67 780 | 6,3 |
| 1280x960 | 82 712 | 192 | 7 715 | 7 887 | 10,48 | 109 533 | 10,2 |
| 1600x1200 | 121 480 | 139 | 11 311 | 11 443 | 10,52 | 174 673 | 16,2 |
| 2048x1536 | 189 448 | 138 | 17 507 | 17 645 | 10,57 | 298 496 | 27,6 |
| 2592x1944 | 300 040 | 215 | 27 616 | 27 831 | 10,61 | 424 224 | 39,0 |

\* 320x240 : 2 mesures (1er envoi en échec), médiane de 2 = moyenne ; le 2e envoi a attendu le serveur (4 759 ms, 2,11 ko/s), le 3e non (1 139 ms, total 1 304 ms, 8,77 ko/s).
Envoi estimé = taille max de `camera.md` / débit médian (8,77 ko/s en 320x240) : calcul, pas mesure. La taille dépend de la scène : en 320x240 et 640x480, ce test dépasse déjà `camera.md`.

- Sortie type de `camera_http_bench()` :

```
[photo] resolution ............. 1024x768
[photo] taille ................. 56376 octets
[photo] capture ................ 159 ms
[photo] envoi .................. 5347 ms
[photo] total .................. 5506 ms
```

- Photo : à compléter.

## Pièges et limites

- RESET de la carte pendant un envoi → 1er envoi suivant en `[http] KO` / -3, 2e envoi lent (3,9 à 4,8 s en 320x240). Le serveur de test traitait une connexion à la fois et attendait la suite de l'envoi coupé jusqu'à 30 s sans octet : la photo « en échec » arrive entière, enregistrée environ 30 s après la coupure, en même temps que la suivante. Reproduit 3 fois sur la carte (22:47 à 22:48) et sur le PC → serveur passé à un fil d'exécution par envoi (envoi suivant traité en 0,01 s sur le PC).
- `[http] KO` / -1 : serveur pas encore lancé (première photo reçue aussitôt après), comportement de P2.3.
- La carte est bloquée pendant tout l'envoi : 27,6 s (médiane) en 2592x1944, environ 39 s estimées pour la plus grosse image de `camera.md`.
- Débit plafonné vers 10,5 ko/s quelle que soit la résolution, comme en P2.3 (`http.md`) ; la capture (130 à 260 ms) pèse peu dans le total.
- Remplissage après le dernier FF D9 : 8 à 2 049 octets selon la photo, dans toutes les résolutions (au moins 1 034 en 2048x1536 et 2592x1944) ; envoyé, enregistré et compris dans les tailles.
- Non testé : retour à la résolution de `config.h` à la fin du banc (invisible dans sa sortie) ; serveur coupé pendant le banc ; changement d'IP du PC.

## À reporter (si applicable)

- [ ] Décision D9 (640x480) → `DECISIONS.md`
- [x] Envoi unique dans `self_test_run()` (`camera_http_test()`), à valider sur la carte
- [ ] Éléments pour le compte rendu (P12.1) : tableau des temps par résolution
