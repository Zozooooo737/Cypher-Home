# CONTEXTE TECHNIQUE : Cypher Home

> Document de référence partagé entre Claude Desktop et Claude Code. Il décrit uniquement les **composants et leurs broches physiques**. Les décisions (choix technologiques, affectation des broches de la carte, protocole) sont dans `DECISIONS.md`, jamais ici.
> Les noms de broches ci-dessous sont ceux à utiliser dans tous les échanges, pour garder un nommage commun.
> À mettre à jour ici : références, nom des pattes après test de continuité, valeurs des résistances.

## 1. Périmètre
Voir `DECISIONS.md`, section « Périmètre ».

## 2. Choix technologiques
Voir `DECISIONS.md`, sections « Décidé » et « À valider ou à trancher ».

## 3. Composants et broches physiques

### 3.1 Carte de développement : Arduino UNO R4 WiFi
- Quantité : 1
- Datasheet : https://docs.arduino.cc/resources/datasheets/ABX00087-datasheet.pdf
- Broches : IOREF, RESET, 3.3V, 5V, GND, GND, VIN, A0, A1, A2, A3, A4, A5, D0 (RX), D1 (TX), D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, GND, AREF, SDA, SCL
- Correspondance avec le symbole KiCad (ARDUINO R4 WIFI) : 3.3V = +3V3 ; 5V = +5V ; GND = GND_JA6, GND_JA7, GND_JD15, GND_JO2 (le symbole a 4 GND) ; RESET = ~{RESET} ; A0 à A5 = A0/D14 à A5/D19 ; D0 (RX) = D0/RX0 ; D1 (TX) = D1/TX0 ; D10 = D10/CS/CANTX0 ; D11 = D11/COPI ; D12 = D12/CIPO ; D13 = D13/SCK/CANRX0 ; SDA = D18/SDA ; SCL = D19/SCL (A4/D18 et D18/SDA seraient le même signal [H] à confirmer dans la datasheet). Broches du symbole non listées ci-dessus, non utilisées : BOOT, OFF, VBATT/VRTC.

### 3.2 Capteur environnemental : BME680
- Quantité : 1
- Module : Joy-IT SEN-BME680
- Documentation : https://www.joy-it.net/files/files/Produkte/SEN-BME680/SEN-BME680_Manual-EN_2026-04-17.pdf
- Broches : VIN, 3Vo, GND, SCK, SDO, SDI, CS
- Correspondance avec le manuel Joy-IT : VIN = VCC, SDI = SDA, SCK = SCL, SDO = SD0 ; 3Vo non documenté.

### 3.3 Caméra : ArduCAM OV5642 (B0068)
- Quantité : 1
- Liens :
  - https://www.gotronic.fr/art-camera-arducam-ov5642-b0068-25252.htm?srsltid=AYCX9vKjcOwVlByCTJcQRcsWEVSv-NuT2110Uz62mhMJQQfFdgh8uPl1
  - https://www.arducam.com/arducam-5mp-plus-spi-cam-arduino-ov5642.html
  - https://docs.arducam.com/Arduino-SPI-camera/Legacy-SPI-camera/Introduction/
  - https://docs.arducam.com/Arduino-SPI-camera/Legacy-SPI-camera/Hardware/Arducam-Shield-Mini-5MP-Plus/
  - https://docs.arducam.com/Arduino-SPI-camera/Legacy-SPI-camera/Software/Quick-Start-Guide/
  - https://www.uctronics.com/download/Amazon/B0068.pdf
  - http://www.uctronics.com/download/Image_Sensor/OV5642_DS.pdf
  - https://www.arducam.com/downloads/shields/ArduCAM_Camera_Shield_Software_Application_Note.pdf
- Broches : CS, MOSI, MISO, SCK, GND, VCC, SDA, SCL

### 3.4 LED rouge (réf. LED5RL)
- Quantité : 1
- Type : LED 5 mm classique, 2 pattes
- Broches : Anode (+), Cathode (−)
- Correspondance avec le symbole KiCad LED : Anode (+) = A (broche 2), Cathode (−) = K (broche 1).

### 3.5 LED bleue (réf. LED5BT)
- Quantité : 1
- Type : LED 5 mm classique, 2 pattes
- Broches : Anode (+), Cathode (−)

### 3.6 LED jaune (réf. LED5YL)
- Quantité : 1
- Type : LED 5 mm classique, 2 pattes
- Broches : Anode (+), Cathode (−)

### 3.7 LED orange (réf. LED5OR)
- Quantité : 1
- Type : LED 5 mm classique, 2 pattes
- Broches : Anode (+), Cathode (−)

### 3.8 Capteur de gaz : MQ-2
- Quantité : 1
- Datasheet : https://www.handsontec.com/dataspecs/MQ2-Gas%20Sensor.pdf
- Broches : A0, D0, GND, VCC

### 3.9 Buzzer : SVM06N (buzzer passif)
- Quantité : 1
- Datasheet : https://www.handsontec.com/dataspecs/module/passive%20buzzer.pdf
- Broches : GND, V+, Signal

### 3.10 Boutons poussoirs
- Quantité : 5
- Type : boutons poussoirs classiques, 4 pattes
- Broches : A1, A2, B1, B2 (4 pattes par bouton). Seules A1 (reliée à la broche de la carte) et B2 (reliée à GND) sont câblées (test de continuité du 2026-11-09).

### 3.11 Interrupteur général : DS12A
- Quantité : 1
- Type : interrupteur ON-OFF unipolaire rond clipsable à bascule, marquage 0/I, noir
- Caractéristiques : 6 A/250 Vac (charge résistive), 10 A/24 Vac (charge résistive), 3 A (charge inductive) ; perçage 20,5 mm ; hauteur encastrable 16 mm ; hauteur des cosses 8 mm
- Broches : Borne 1, Borne 2 : reliées sur ON, déconnectées sur OFF (test de continuité : ON le 2026-11-12, OFF le 2026-11-13).

### 3.12 Écran couleur : TFT ST7735S 1,8″ 128×160, 3,3 V (DollaTek)
- Quantité : 1
- Liens :
  - https://www.scribd.com/document/1023783680/M180-128160-RGB-8
  - https://www.buydisplay.com/download/ic/ST7735S.pdf
- Broches : GND, VCC, SCL, SDA, RES, DC, CS, BLK

### 3.13 Convertisseur de tension : Step-Up Boost 2–24 V vers 5–28 V
- Quantité : 1
- Datasheet : https://www.lcsc.com/datasheet/C84817.pdf
- Broches : VOUT+, VOUT-, VIN+, VIN-

### 3.14 Gestionnaire solaire et cellule photovoltaïque : SunFlower 2 V / 160 mA (DFR0579)
- Quantité : 1 (ensemble composé d'une cellule et d'un module de gestion d'alimentation)
- Documentation : https://wiki.dfrobot.com/dfr0579/
- Broches de la cellule : pin+, pin-
- Broches du module :
  - 3.7V Li Battery IN (connecteur JST)
  - 3.7V Li Battery IN (2 pads : + et −)
  - Solar Panel IN (2 pads : + et −)
  - Regulated 3.3V Output (2 pads : + et −)
  - USB Charge IN
- Correspondance avec les symboles KiCad : cellule (AK50X50) : pin+ et pin- identiques. Module (DFR0579) : Solar Panel IN (+ et −) = VCC (numéro +) et GND (numéro -) ; 3.7V Li Battery IN, connecteur JST = + et - (numéros JST+ / JST-) et pads = + et - (numéros BAT+ / BAT-) [H] attribution déduite du dessin du symbole, à confirmer sur le module ; Regulated 3.3V Output (+ et −) = VCC (OUT+) / GND (OUT-). USB Charge IN : absente du symbole.

### 3.15 Batterie LiPo : AFTERTECH 3,7 V, 5 000 mAh, BMS
- Quantité : 1
- Connecteur : JST PHR-02
- Broches : JST OUT
- Correspondance avec le symbole KiCad : JST OUT = JST_OUT_+ (numéro BAT+) et JST_OUT_- (numéro BAT-). Polarité réelle du connecteur à vérifier en P11.1.

### 3.16 Registre à décalage : 74HC595
- Quantité : 1
- Référence : 74HC595
- Datasheet : https://passionelectronique.fr/wp-content/uploads/74HC595_datasheet.pdf?pseSrc=pg74HC595arduino
- Broches : VCC, GND, SER, SRCLK, RCLK, SRCLR, OE, QA, QB, QC, QD, QE, QF, QG, QH, QH'
- Correspondance avec la datasheet (Philips) : SER = DS, SRCLK = SH_CP, RCLK = ST_CP, SRCLR = MR, QA à QH = Q0 à Q7, QH' = Q7'.

### 3.17 Résistances et câblage
- LED : 4 × 270 Ω, une en série avec chaque LED (validé le 2026-11-09)
- Écran ST7735S : aucune résistance ; SCL, SDA, RES, DC et CS reliés directement à la carte
