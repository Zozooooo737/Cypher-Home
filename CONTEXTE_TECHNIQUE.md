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
- Broches : 4 pattes par bouton. Noms validés au test de continuité (2026-11-09) : A1 (reliée à la broche de la carte) et B2 (reliée à GND) ; noms des deux autres pattes : à compléter

### 3.11 Interrupteur général : DS12A
- Quantité : 1
- Type : interrupteur ON-OFF unipolaire rond clipsable à bascule, marquage 0/I, noir
- Caractéristiques : 6 A/250 Vac (charge résistive), 10 A/24 Vac (charge résistive), 3 A (charge inductive) ; perçage 20,5 mm ; hauteur encastrable 16 mm ; hauteur des cosses 8 mm
- Broches : 3 pattes (nommage à définir lors du test de continuité)

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

### 3.15 Batterie LiPo : AFTERTECH 3,7 V, 5 000 mAh, BMS
- Quantité : 1
- Connecteur : JST PHR-02
- Broches : JST OUT

### 3.16 Registre à décalage : 74HC595
- Quantité : 1
- Référence : à noter
- Datasheet : à noter
- Broches : VCC, GND, SER, SRCLK, RCLK, SRCLR, OE, QA, QB, QC, QD, QE, QF, QG, QH, QH'

### 3.17 Résistances et câblage
- LED : 4 × 270 Ω, une en série avec chaque LED (validé le 2026-11-09)
- Écran ST7735S : diviseurs 1 kΩ (côté carte) / 2 kΩ (vers GND) sur les 5 signaux SCL, SDA, RES, DC et CS
- Références : à noter
