# Fiche de test : caméra ArduCAM OV5642

> [H] = hypothèse, non vérifiée. Tout le reste vient du dépôt ou des relevés de l'étudiant.

## En-tête

| Composant                    | ArduCAM Mini 5MP Plus, OV5642 (réf. B0068)          |
| ---------------------------- | --------------------------------------------------- |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.3                                               |
| Étape du plan                | P1.1                                                |
| Date du test                 | 2026-11-06 (relevés complémentaires du 2026-11-05)  |
| Datasheet                    | https://www.uctronics.com/download/Amazon/B0068.pdf |
| **Verdict**                  | **OK**                                              |

Raisons du verdict : Caméra fonctionnelle et enregistrement opérationnel.

## Alimentation

| Élément | Valeur | Source |
|---|---|---|
| Tension d'alimentation | Broche 5V ; environ 4,57 V mesurés | `pins.h` ; mesure avant mise sous tension du code |
| Niveau logique | SDA et SCL au repos : environ 4,57 V. Entrées/sorties de la caméra tolérantes 5 V, pas de convertisseur [H] | mesure ; `pins.h` |
| Courant | Environ 300 mA en fonctionnement [H], non mesuré | doc Arducam citée dans `pins.h` |
| Démarrage ou préchauffage | Reset : 2 fois 100 ms ; stabilisation : 1 s après configuration | `camera.cpp` |

## Câblage testé

| Broche du composant (nom officiel) | Broche de la carte | Remarque |
|---|---|---|
| CS | D10 | Broche choisie (`CAMERA_CS_PIN`) |
| MOSI | D11 | Bus SPI imposé par la carte |
| MISO | D12 | Bus SPI imposé par la carte |
| SCK | D13 | Bus SPI imposé par la carte ; aussi la LED intégrée |
| GND | GND | |
| VCC | 5V | |
| SDA | SDA (près d'AREF) | Objet `Wire` ; reliée à A4 [H], continuité non testée |
| SCL | SCL (près d'AREF) | Objet `Wire` ; reliée à A5 [H], continuité non testée |

- Bus et adressage : SPI mode 0 (registres et tampon image) + I2C (capteur, adresse 0x3C sur 7 bits).
- La tension de SDA et SCL au repos montre que des résistances de rappel sont présentes.
- Cette table est le câblage **testé** le 2026-11-06, provisoire tant que D3 est ouverte. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Bibliothèque : fork communautaire https://github.com/keeeal/ArduCAM-Arduino-Uno-R4 (dossier `ArduCAM/`), commit `8b276d39420b5d57f31b2f9a290a2fc9586e527f`, version annoncée V4.1.3. Base : https://github.com/ArduCAM/Arduino, commit `066a7ea`.
- Licence : LGPL 2.1 ; le fichier MIT de la racine du dépôt d'origine est conservé.
- Copie locale : `firmware/lib/ArduCAM/` (2026-11-05), origine détaillée dans `CYPHER_SOURCE.md`. `memorysaver.h` : `OV5642_MINI_5MP_PLUS` actif.
- Correctifs : aucun, pas de `CYPHER-PATCH` dans le code. Un correctif ajouté le 2026-11-05 a été retiré le même jour pour garder la copie identique à l'amont. Avertissement GCC connu dans `wrSensorRegs16_8()`, non corrigé, sans défaut observé.
- Ligne `lib_deps` : N/A (copie locale).
- Compatibilité UNO R4 WiFi : oui sur cette carte (images reçues les 2026-11-05 et 2026-11-06) ; le fork ajoute l'accès à la broche CS pour la R4.
- Fichiers du dépôt : `firmware/src/components/camera.cpp`, `firmware/scripts/serial_monitor.py`.
- Paramètres du test : SPI 4 MHz, blocs de 512 octets, série 115200, 230400 et 460800 bauds, JPEG dans les 7 résolutions de 320x240 à 2592x1944.
- Résolution retenue pour le projet : `OV5642_1024x768`, provisoire, à revalider avec le débit Wi-Fi réel en P2.5 (repli : `OV5642_640x480`). À 460800 bauds : 67 780 octets, transfert 1 627 ms.
- Débit série retenu : 460800 bauds (provisoire), le plus rapide des trois essayés, sans erreur observée sur les images enregistrées. Contrôle faible : marqueurs FF D8 / FF D9 seulement, pas de CRC.
- Formules utiles : N/A.
- Alternatives écartées (chiffres à 460800 bauds) :
  - 640x480 : plus rapide (26 396 octets, 635 ms). Moins de détail [H : effet sur la reconnaissance d'une personne non mesuré].
  - 1280x960 (109 533 octets, 2 635 ms), 1600x1200 (174 673 octets, 4 204 ms), 2048x1536 (298 496 octets, 6 773 ms) : plus lourdes, donc envoi plus long pendant lequel la carte est bloquée.
  - 2592x1944 : la plus lourde (424 224 octets, 10 386 ms).
  - Bibliothèque : Bibliothèque ArduCAM d’origine : non essayée. Le fork keeeal ne diffère de l’original que par le support UNO R4 (macros ajoutées dans `ArduCAM.h`) et a fonctionné sans correctif.

## Résultat

- Intégrité : avant la mise sous tension du code, continuité correcte et aucun court-circuit.
- Sortie type de `camera_test()` (3 à 5 lignes) :

```
[moniteur] Images enregistrées dans C:\Users\enzob\Documents\CypherHome\firmware\output
[moniteur] Connecté à COM3 (460800 bauds). Ctrl+C pour quitter.

=== Self-test : debut ===

----- TEST CAMERA -----
[moniteur] cam_20261106-153621.jpg : 248804 octets, transfert carte 5984 ms
[camera] resolution 2592x1944 | format JPEG | capture 255 ms

=== Self-test : fin ===
```

- Mesures :

| Date       | Série (bauds) | Résolution    | Taille (octets) | Capture (ms) | Transfert carte |
| ---------- | ------------- | ------------- | --------------- | ------------ | --------------- |
| 2026-11-06 | 115 200       | **2592x1944** | 225 351         | 244          | **22 139**      |
| 2026-11-06 | 115 200       | **2048x1536** | 144 788         | 261          | **13 058**      |
| 2026-11-06 | 115 200       | **1600x1200** | 98 921          | 153          | **8 822**       |
| 2026-11-06 | 115 200       | **1280x960**  | 73 032          | 256          | **6 679**       |
| 2026-11-06 | 115 200       | **1024x768**  | 49 139          | 218          | **4 294**       |
| 2026-11-06 | 115 200       | **640x480**   | 22 752          | 148          | **2 031**       |
| 2026-11-06 | 115 200       | **320x240**   | 7 830           | 155          | **693**         |

| Date       | Série (bauds) | Résolution    | Taille (octets) | Capture (ms) | Transfert carte |
| ---------- | ------------- | ------------- | --------------- | ------------ | --------------- |
| 2026-11-06 | 230 400       | **2592x1944** | 387 704         | 216          | **17 745**      |
| 2026-11-06 | 230 400       | **2048x1536** | 265 425         | 262          | **12 232**      |
| 2026-11-06 | 230 400       | **1600x1200** | 166 037         | 235          | **7 599**       |
| 2026-11-06 | 230 400       | **1280x960**  | 103 110         | 222          | **4 720**       |
| 2026-11-06 | 230 400       | **1024x768**  | 63 846          | 234          | **2 923**       |
| 2026-11-06 | 230 400       | **640x480**   | 25 219          | 231          | **1 138**       |
| 2026-11-06 | 230 400       | **320x240**   | 8 233           | 187          | **376**         |

| Date       | Série (bauds) | Résolution | Taille (octets) | Capture (ms) | Transfert carte |
| ---------- | ------------- | ---------- | --------------- | ------------ | --------------- |
| 2026-11-06 | 460 800       | **2592x1944**  | 424 224         | 196          | **10 386**      |
| 2026-11-06 | 460 800       | **2048x1536**  | 298 496         | 208          | **6 773**       |
| 2026-11-06 | 460 800       | **1600x1200**  | 174 673         | 150          | **4 204**       |
| 2026-11-06 | 460 800       | **1280x960**   | 109 533         | 236          | **2 635**       |
| 2026-11-06 | 460 800       | **1024x768**   | 67 780          | 163          | **1 627**       |
| 2026-11-06 | 460 800       | **640x480**    | 26 396          | 223          | **635**         |
| 2026-11-06 | 460 800       | **320x240**    | 8 367           | 257          | **201**         |

- Valeurs à revérifier dans la sortie brute, sans effet sur le choix de résolution : 115200 bauds / 1024x768 (11,4 Ko/s, proche du maximum théorique d'environ 11,5 Ko/s) ; 115200 bauds / 2592x1944 (10,2 Ko/s) et 460800 bauds / 2048x1536 (44,1 Ko/s), qui s'écartent des lignes voisines.
- Photo : `docs/tests/camera.jpg`

## Pièges et limites

- La longueur lue dans le tampon dépasse la fin du JPEG de quelques octets → pas une erreur, le script coupe le fichier au dernier FF D9.
- Écriture série rapprochée : le dernier octet d'un bloc peut être écrasé → `Serial.flush()` après chaque bloc (`camera.cpp`).
- `OV5642_1920x1080` existe dans la bibliothèque mais n'y est pas implémentée → ne pas l'utiliser (`config.h`).
- `-D OV5642_MINI_5MP_PLUS` dans `platformio.ini` → double définition, ne pas l'ajouter (`CYPHER_SOURCE.md`).
- Non testé :
  - connexion du script au milieu d'une image ;
  - reset de la carte pendant un transfert ;
  - cas KO « FF D8 absent » (non déclenchable proprement) ;
  - continuité SDA vers A4 et SCL vers A5 ;
  - courant consommé par la caméra.

## À reporter (si applicable)

- [ ] Décision D2 → `DECISIONS.md`
- [ ] Ligne du composant → `docs/compatibilite.md` (P1.6)
- [ ] Éléments pour le compte rendu (P12.1) : à compléter
