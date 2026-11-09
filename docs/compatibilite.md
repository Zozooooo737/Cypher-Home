# Matrice de compatibilité électrique : Cypher Home

> Étape P1.6 (`PLAN.md`), rédigée le 2026-11-09. Noms de broches : `CONTEXTE_TECHNIQUE.md`. Broches de la carte : `firmware/include/pins.h`, **provisoires tant que D3 est ouverte**.
> **Le fonctionnement simultané de tous les modules n'est pas encore validé** : c'est le rôle de P1.7 (essai d'ensemble, mesure du courant total). Les courants ci-dessous sont ceux des datasheets, pas des mesures.

## Légende des sources

| Marque | Signification |
|---|---|
| [DS] | datasheet ou documentation du fabricant (lien dans `CONTEXTE_TECHNIQUE.md`) |
| [ÉT] | indiqué par l'étudiant |
| [ST] | constaté avec `self_test_run()` (fiche `docs/tests/<composant>.md`) |
| [MES] | mesuré au multimètre par l'étudiant (fiche du composant) |
| [CALC] | calculé à partir des valeurs ci-dessus (calcul donné) |
| [pins.h] | câblage courant, provisoire (D3) |
| [H] | hypothèse, non vérifiée |

Documents lus : datasheet de l'UNO R4 WiFi (ABX00087), page docs.arducam.com du Mini 5MP Plus, manuel Joy-IT SEN-BME680, fiches Handsontec du MQ-2 et du buzzer passif, datasheet Philips du 74HC595. Non lisibles : PDF B0068 (image sans texte), page produit arducam.com (accès refusé), datasheet ST7735S de buydisplay.com (accès refusé), notice du module écran sur Scribd (contenu non accessible).

## Matrice

| Composant | Tension d'alimentation | Niveau logique | Courant | Bus | Broche CS ou adresse I2C | Source |
|---|---|---|---|---|---|---|
| Carte Arduino UNO R4 WiFi | USB-C 5 V (VUSB 4,8 à 5,5 V) ; environ 4,7 V au RA4M1 après la diode Schottky ; VIN 6 à 24 V (non utilisé). 3,3 V produit par un régulateur linéaire SGM2205-3.3, qui alimente aussi l'ESP32-S3 [DS] | 5 V (RA4M1, VDD relié au 5 V) ; ESP32-S3 en 3,3 V derrière un traducteur de niveaux interne [DS] | 8 mA max par GPIO [DS] ; courant max des broches 5V et 3.3V : non trouvé : à mesurer en P1.7 | Maître SPI (D11 MOSI, D12 MISO, D13 SCK) et I2C `Wire` (SDA, SCL reliées à A4, A5) [DS, pins.h] | Sans objet (maître des bus) | Datasheet ABX00087 §2, §11 ; `pins.h` |
| Capteur BME680 (Joy-IT SEN-BME680) | VIN sur 5V [ÉT, pins.h] ; 5V dans le câblage Arduino du manuel [DS] | Lignes I2C à 5 V : compatibilité déduite du schéma du manuel, non écrite [DS, fiche] | Non trouvé : à mesurer en P1.7 (absent du manuel Joy-IT) | I2C, partagé avec la caméra | 0x76 (SDO à GND ; 0x77 par défaut) [DS] ; répond à 0x76 sur 9 passes [ST] | Manuel Joy-IT §2, §3 ; `bme680.md` |
| Caméra ArduCAM Mini 5MP Plus OV5642 (B0068) | VCC sur 5V [ÉT] ; plage 3,3 à 5 V [DS] ; 4,57 V mesurés [MES] | « All IO ports are 5V/3.3V tolerant » [DS] ; niveau haut de MISO (sur D12 [ÉT]) : non trouvé : à mesurer en P1.7 ; lu correctement par la carte (JPEG valides) [ST] | 300 mA en fonctionnement, 20 mA en mode basse consommation, à 5 V [DS] | SPI mode 0, 8 MHz max [DS], 4 MHz utilisés (`config.h`) + I2C (capteur) | CS : D10 [pins.h] ; I2C : 0x78 / 0x79 (écriture / lecture), soit 0x3C sur 7 bits [DS, `firmware/lib/ArduCAM/ArduCAM.cpp` l. 189] ; répond [ST] | docs.arducam.com (Mini 5MP Plus) ; `camera.md` |
| LED ×4 (rouge LED5RL, bleue LED5BT, jaune LED5YL, orange LED5OR) | Alimentées par les sorties QB à QE du 74HC595 (5 V) à travers 270 Ω [ÉT, pins.h] | Sans objet (commandées par le 74HC595) | Par LED : non trouvé (tension directe absente du lien Adafruit, générique) : à mesurer en P1.7 ; borne haute 5 V / 270 Ω = 18,5 mA par LED, 74 mA pour 4 [CALC] | GPIO via 74HC595 | Sans objet : bits 1 à 4 de l'octet envoyé (QB à QE) [pins.h] | `leds.md` ; `pins.h` |
| Capteur de gaz MQ-2 | VCC sur 5V [ÉT, pins.h] ; « Power Supply: 5VDC » [DS] | A0 analogique : 0,41 à 2,83 V mesurés, dans la plage de l'ADC (référence 5 V) [MES]. La fiche annonce « 0.1 ~ 0.3 V » [DS], contredit par la mesure. D0 TTL 0 / 5 V [DS], non connectée | « Working Current: ~150mA » (chauffage compris) [DS] | Analogique | Sans objet : A0 [pins.h] | Handsontec MDU-1027 ; `mq2.md` |
| Buzzer passif SVM06N | V+ sur 5V [ÉT, `buzzer.md`] ; « Operation Voltage: 3~5V » [DS] | Signal carré 5 V produit par D0 avec `tone()` ; fonctionne [ST, à l'oreille] | Non trouvé : à mesurer en P1.7 (absent de la fiche Handsontec), y compris le courant tiré par Signal sur D0 | GPIO | Sans objet : Signal sur D0 [pins.h] | Handsontec MDU1126 ; `buzzer.md` |
| Boutons ×5 (pattes A1, A2, B1, B2) | Aucune : A1 sur la broche de la carte, B2 à GND ; A2 et B1 non câblées [ÉT] | 5 V par la résistance de tirage interne (`INPUT_PULLUP`) ; relâché = HIGH, appuyé = LOW [ST] | Seulement bouton appuyé ; valeur de la résistance de tirage interne non trouvée (absente de la datasheet de la carte) : à mesurer en P1.7 | GPIO en entrée | Sans objet : D2 centre, D3 droite, D4 gauche, D5 haut, D6 bas [pins.h] | `buttons.md` ; `pins.h` |
| Interrupteur DS12A | Non câblé, hors périmètre de P1.6 : traité avant P3.2 (D10), avec SunFlower et batterie. 6 A / 250 Vac, 10 A / 24 Vac (charge résistive), 3 A (charge inductive) | Sans objet (coupe l'alimentation) | Sans objet (ne consomme pas) | Aucun | Sans objet : Borne 1, Borne 2 (2 bornes selon l'étudiant, à confirmer au test de continuité) | `CONTEXTE_TECHNIQUE.md` §3.11 ; [ÉT] |
| Écran TFT ST7735S 1,8″ (DollaTek) | VCC et BLK sur 3.3V [ÉT, pins.h] (régulateur 3,3 V de la carte) | Module 3,3 V : SCL, SDA, RES, DC, CS ramenés de 5 V à 5 × 2 / 3 = 3,33 V par diviseurs [CALC] ; aucun composant chaud [ÉT] | Non trouvé : à mesurer en P1.7 (datasheet ST7735S et notice du module non accessibles) | SPI mode 0 sans MISO, partagé avec la caméra, 4 MHz après l'initialisation (`config.h`) | CS : D7 ; RES : D8 ; DC : D9 [pins.h] | `display.md` ; `pins.h` |
| Registre à décalage 74HC595 | VCC 2,0 à 6,0 V [DS] ; branché sur 5V d'après `pins.h` (« à confirmer », VCC non renseigné dans `74hc595.md`) | Entrées : VIH ≥ 3,15 V à VCC = 4,5 V [DS] : commande en 5 V correcte ; fonctionne (bonnes LED allumées) [ST, à l'œil] | Repos ≤ 80 µA ; ±35 mA max par sortie Qn ; ±70 mA max par VCC ou GND ; 500 mW max [DS] | GPIO, `shiftOut()` logiciel (pas le SPI matériel) | Sans objet : SER A1, RCLK A2, SRCLK A3 [pins.h] | Datasheet Philips 74HC595 (Limiting values, DC characteristics) |
| Résistances : 4 × 270 Ω (LED) et 5 diviseurs 1 kΩ / 2 kΩ (écran) | Diviseurs entre une broche de la carte (5 V) et GND | Sortie des diviseurs : 3,33 V [CALC] | Diviseur : 5 V / 3 kΩ = 1,67 mA quand le signal est HIGH, 8,3 mA au plus pour les 5 [CALC] ; 270 Ω : voir LED | Sans objet | Sans objet | `CONTEXTE_TECHNIQUE.md` §3.17 |
| Batterie LiPo AFTERTECH 3,7 V 5 000 mAh (BMS) | 3,7 V nominal, connecteur JST PHR-02 | BONUS, non testé (P11) | BONUS, non testé (P11) | BONUS, non testé (P11) | Sans objet | `CONTEXTE_TECHNIQUE.md` §3.15 |
| SunFlower DFR0579 et cellule 2 V / 160 mA | Entrées batterie 3,7 V, panneau solaire, USB ; sortie régulée 3,3 V | BONUS, non testé (P11) | Cellule : 160 mA ; reste : BONUS, non testé (P11) | BONUS, non testé (P11) | Sans objet | `CONTEXTE_TECHNIQUE.md` §3.14 |
| Convertisseur Step-Up 2–24 V vers 5–28 V | Entrée 2 à 24 V, sortie 5 à 28 V | BONUS, non testé (P11) | BONUS, non testé (P11) | BONUS, non testé (P11) | Sans objet | `CONTEXTE_TECHNIQUE.md` §3.13 |

## Conflits et solutions

| # | Conflit ou risque | Solution |
|---|---|---|
| C1 | Bus SPI partagé caméra / écran : un périphérique dont le CS est à LOW reçoit les échanges destinés à l'autre. Les diviseurs tirent le CS de l'écran à LOW tant que D7 n'est pas configurée. | Un CS par périphérique (D10 caméra, D7 écran) ; les deux CS mis à HIGH au tout début de `setup()`, avant tout `<composant>_setup()` (en place, `display.md`). Entre `camera_read_begin()` et `camera_read_end()`, le bus est réservé à la caméra. |
| C2 | Les diviseurs 1 kΩ / 2 kΩ de l'écran chargent SCK (D13) et MOSI (D11), communs avec la caméra. | Charge continue : 1,67 mA par ligne au niveau HIGH [CALC], sous la limite de 8 mA par GPIO [DS] ; MISO (D12) n'est pas relié à l'écran. D13 commande aussi la LED intégrée (courant non trouvé). Effet sur les fronts à 4 MHz non chiffré (capacités d'entrée non trouvées) [H : négligeable]. Avant `SPI.begin()`, les diviseurs tirent SCK et MOSI à LOW : sans effet en mode 0 (horloge au repos à LOW). À vérifier en P1.7 : photo valide pendant que l'écran est branché et affiche. Repli : réduire la fréquence SPI (`config.h`). |
| C3 | Écran en 3,3 V, carte en 5 V. | Diviseurs sur SCL, SDA, RES, DC, CS : 3,33 V [CALC] ; VCC et BLK sur 3.3V ; aucun composant chaud [ÉT]. |
| C4 | La bibliothèque de l'écran envoie son initialisation à sa vitesse par défaut (32 MHz demandés, `display.md`), au-delà des 8 MHz max de la caméra [DS]. | Sans effet sur la caméra : son CS est à HIGH pendant l'initialisation de l'écran (C1). L'écran fonctionne malgré cette vitesse [ÉT]. |
| C5 | Bus I2C partagé caméra / BME680. | Adresses distinctes : 0x3C (caméra) et 0x76 (BME680) [DS, code] : aucun conflit d'adresse. Niveau du bus au repos : 4,57 V [MES], accepté par les deux (caméra tolérante 5 V [DS] ; BME680 d'après le schéma du manuel). Résistances de rappel des deux modules en parallèle : valeurs non trouvées [H : sans effet, les deux composants répondent]. À confirmer en P1.7 : caméra et BME680 lus dans la même passe. |
| C6 | SDA et SCL sont reliées à A4 et A5 [DS]. | A4 et A5 ne sont pas utilisées en analogique (`pins.h`) ; MQ-2 sur A0, 74HC595 sur A1 à A3. |
| C7 | D0 (buzzer) est aussi le RX de `Serial1`. | `Serial1` n'est jamais initialisé. `Serial` (moniteur et téléversement) passe par l'UART relié à l'ESP32-S3, pas par D0 (`firmware/CLAUDE.md`). |
| C8 | Courant tiré par la broche Signal du buzzer sur D0 inconnu ; limite de 8 mA par GPIO [DS]. | Mesurer en P1.7 (multimètre en série sur Signal pendant `tone()`). Si plus de 8 mA : ajouter une résistance en série ou un transistor (à valider par l'étudiant). |
| C9 | Courant total des 4 LED par le 74HC595 : borne haute 74 mA [CALC] pour 70 mA max par VCC ou GND [DS]. La borne suppose une tension directe nulle : le courant réel est plus faible. | Mesurer en P1.7, les 4 LED allumées. Si plus de 70 mA : augmenter les résistances (à valider). Chaque sortie reste sous ±35 mA [DS]. |
| C10 | Régulateur 3,3 V de la carte partagé entre l'ESP32-S3 (Wi-Fi) et l'écran (VCC + rétroéclairage) ; courant max non trouvé. | Mesurer le courant de l'écran en P1.7 et surveiller les resets avec le Wi-Fi actif (P2.1). Si chute ou reset : alimenter l'écran par un régulateur 3,3 V séparé (achat à valider). |
| C11 | Budget du 5 V au-delà de la limite USB (voir section suivante). | Mesure du courant total en P1.7. Replis : port USB du PC plus puissant ou chargeur USB-C 5 V ; mode basse consommation de la caméra entre deux photos (20 mA [DS] ; prise en charge par la bibliothèque à vérifier). |
| C12 | D13 = SCK et LED intégrée : la LED clignote pendant les échanges SPI. | Sans conséquence ; D13 n'est pas utilisée comme sortie. |
| C13 | A0 du MQ-2 est aussi la sortie DAC de la carte. | DAC non utilisé (`pins.h`). |

## Budget du 5 V

Indication de l'étudiant : la plupart des composants alimentés par le 5 V de la carte consommeraient au total **0,25 A**. Origine non précisée : ce n'est **pas** une mesure.

| Consommateur sur le 5 V | Courant | Source |
|---|---|---|
| Caméra OV5642 | 300 mA (20 mA en basse consommation) | [DS] docs.arducam.com |
| MQ-2 (chauffage compris) | environ 150 mA | [DS] Handsontec |
| LED ×4 (via 74HC595) | 74 mA au plus (borne haute) | [CALC] |
| Diviseurs de l'écran (fournis par les broches de la carte) | 8,3 mA au plus | [CALC] |
| 74HC595 au repos | 0,08 mA au plus | [DS] Philips |
| BME680 | Non trouvé : à mesurer en P1.7 | — |
| Buzzer | Non trouvé : à mesurer en P1.7 | — |
| Écran (sur le 3,3 V, tiré du 5 V) | Non trouvé : à mesurer en P1.7 | — |
| Carte elle-même (RA4M1, ESP32-S3 et Wi-Fi, matrice de LED) | Non trouvé : à mesurer en P1.7 | — |

- Caméra et MQ-2 seuls : 450 mA d'après les datasheets [CALC], soit plus que les 0,25 A indiqués. L'écart n'est pas tranché ici (valeurs de datasheet maximales ou typiques, périmètre exact des 0,25 A inconnu) : la mesure de P1.7 fait foi.
- Limite USB : la datasheet de la carte impose au plus 5 V sur l'USB-C, sans courant maximal. Un port USB 2.0 de PC fournit normalement 500 mA (900 mA en USB 3.x) : norme USB, pas une donnée de la carte, à vérifier sur le PC utilisé. Avec le total des datasheets (plus de 450 mA, carte non comptée), la marge sous 500 mA est faible ou nulle.
- Mesure réelle : prévue en P1.7 (multimètre en série sur l'alimentation, tous les modules actifs, caméra en capture, Wi-Fi actif si possible).

## À collecter pour le compte rendu

- Datasheet ST7735S et notice du module écran (non accessibles en ligne) : à copier dans `hardware/datasheets/` si l'étudiant les a.
- Mesures de P1.7 : courant total, courant de l'écran, du BME680, du buzzer (Signal sur D0), des 4 LED allumées, niveau haut de MISO.
