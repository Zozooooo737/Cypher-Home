# Fiche de test : essai d'ensemble sur breadboard

## En-tête

| | |
|---|---|
| Composant | Tous les modules câblés en même temps : caméra OV5642, BME680, MQ-2, écran ST7735S, 74HC595 + 4 LED, 5 boutons, buzzer SVM06N |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.1 à 3.12, § 3.16, § 3.17 |
| Étape du plan | P1.7 |
| Date du test | 2026-11-10 |
| Datasheet | voir `docs/compatibilite.md` |
| **Verdict** | **OK avec réserve** |

Réserve : essai de 4 min 45 s environ (10 passes) au lieu des 10 min prévues ; courant total non mesuré.

## Alimentation

| Élément | Valeur | Source |
|---|---|---|
| Tension d'alimentation | USB-C du PC (5 V) ; modules sur 5V, sauf l'écran sur 3.3V | câblage testé |
| Niveau logique | 5 V, y compris sur les signaux de l'écran (module 3,3 V, reliés directement, sans diviseur) | câblage testé |
| Courant total | Non mesuré | — |
| Démarrage ou préchauffage | MQ-2 : 300 s (`MQ2_PREHEAT_S`) | `config.h` |

## Câblage testé

Câblage testé le 2026-11-10. Proposition d'affectation soumise à D3 (non validée).

| Broche de la carte | Composant | Broche officielle | Bus ou fonction | Remarque |
|---|---|---|---|---|
| D0 | Buzzer | Signal | GPIO, `tone()` | RX de `Serial1`, jamais initialisé |
| D2 | Bouton centre | A1 | GPIO, `INPUT_PULLUP` | B2 à GND (idem pour les 5 boutons) |
| D3 | Bouton droite | A1 | GPIO, `INPUT_PULLUP` | |
| D4 | Bouton gauche | A1 | GPIO, `INPUT_PULLUP` | |
| D5 | Bouton haut | A1 | GPIO, `INPUT_PULLUP` | |
| D6 | Bouton bas | A1 | GPIO, `INPUT_PULLUP` | |
| D7 | Écran | CS | SPI | relié directement ; mis à HIGH au début de `setup()` |
| D8 | Écran | RES | GPIO | relié directement |
| D9 | Écran | DC | GPIO | relié directement |
| D10 | Caméra | CS | SPI | mis à HIGH au début de `setup()` |
| D11 | Caméra / Écran | MOSI / SDA | SPI MOSI | écran relié directement |
| D12 | Caméra | MISO | SPI MISO | écran sans MISO |
| D13 | Caméra / Écran | SCK / SCL | SPI SCK | écran relié directement ; LED intégrée de la carte |
| SDA (= A4) | Caméra / BME680 | SDA / SDI | I2C | adresses 0x3C et 0x76 ; A4 inutilisable en analogique |
| SCL (= A5) | Caméra / BME680 | SCL / SCK | I2C | A5 inutilisable en analogique |
| A0 | MQ-2 | A0 | analogique 14 bits | aussi sortie DAC (non utilisée) |
| A1 | 74HC595 | SER | GPIO, `shiftOut()` | |
| A2 | 74HC595 | RCLK | GPIO | |
| A3 | 74HC595 | SRCLK | GPIO | |
| 5V | Caméra, BME680, MQ-2, buzzer, 74HC595 | VCC, VIN, VCC, V+, VCC et SRCLR | alimentation | VCC du 74HC595 sur 5V : à confirmer |
| 3.3V | Écran | VCC, BLK | alimentation | régulateur partagé avec l'ESP32-S3 |
| GND | tous | GND ; SDO (BME680), OE (74HC595), B2 (boutons) | masse | |

- Libres : D1 (TX de `Serial1`, utilisable en GPIO si `Serial1` reste inutilisé) ; connecteur Qwiic (`Wire1`, 3,3 V) ; sorties QA, QF, QG, QH du 74HC595. Le Wi-Fi passe par l'ESP32-S3 interne : aucune broche.
- LED : QB rouge, QC bleue, QD jaune, QE orange, 270 Ω chacune (`leds.md`).
- `pins.h` donne le câblage courant ; la table D3 validée fait foi.

## Logiciel

- Fichiers du dépôt : `firmware/src/self_test.cpp` (`self_test_run()`), `firmware/include/pins.h`, `firmware/include/config.h`.
- Chaque passe : en-tête avec numéro de passe et secondes depuis le démarrage (un reset remet les deux à zéro), tests des 7 modules, puis bloc ENSEMBLE : 4 LED allumées et buzzer actif pendant la lecture BME680 + MQ-2, leur affichage à l'écran et une capture caméra envoyée au PC.
- Durée prévue : 10 min sans reset, `self_test_run()` en boucle.

## Résultat

- Passes sans reset : 10 passes, numéro de passe croissant, de 33 s (passe 2) à 258 s (passe 10) depuis le démarrage ; aucun retour à zéro.
- Caméra : toutes les images reçues enregistrées, aucun `[camera] KO`, y compris dans le bloc ENSEMBLE juste après l'affichage (bus SPI partagé) ; capture 144 à 260 ms ; transfert 826 à 1145 ms ; 34 289 à 47 569 octets en 1024x768.
- BME680 lu dans la même passe que la caméra (bus I2C partagé) : 23,01 à 23,39 C ; 43,96 à 44,67 % ; 998,09 à 998,14 hPa ; gaz 195,27 à 229,70 kOhm (hausse continue).
- MQ-2 (préchauffage, jamais PRET pendant l'essai) : 1674 à 1759 sur les passes 2 à 9, puis 1362 à la passe 10 ; aucune alerte.
- Écran : 160x128, affichage du test en 164 ms à chaque passe ; mesures affichées pendant le bloc ENSEMBLE (photo).
- Boutons : les 5 lus appuyés puis relâchés (passe 9).
- Début de la sortie : la passe 1 est tronquée (moniteur lancé pendant l'envoi de sa première image, octets affichés comme texte).
- Photo du montage : `docs/tests/essai_ensemble.jpg`.

## Pièges et limites

- Wi-Fi non actif pendant l'essai : courant de pointe Wi-Fi non mesuré (à refaire en P2.1, conflit C10 de `docs/compatibilite.md`).
- DS12A non testé : reporté avant P3.2 (D10).
- Courant total et courants détaillés (écran, BME680, buzzer sur D0, 4 LED, niveau haut de MISO) : non mesurés.
- Préchauffage du MQ-2 (300 s) non atteint : seuil et alerte non exercés pendant l'essai.
- Baisse du MQ-2 à 1362 à la passe 10 : cause non établie (hypothèse : variation pendant le préchauffage).
