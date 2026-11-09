#pragma once
#include <Arduino.h>

// Broches de la carte Arduino UNO R4 WiFi : câblage courant.
// Un bloc par composant, avec les noms officiels de CONTEXTE_TECHNIQUE.md.
// Tant que D3 est ouverte (DECISIONS.md), les numéros sont provisoires et
// chaque affectation est validée par l'étudiant avant câblage.
// Aucun numéro de broche en dur dans les modules : tout vient d'ici.

// ---------------------------------------------------------------------------
// Index des broches de la carte (une ligne par broche, ex. « D10 : CAMERA_CS »)
// Sert à repérer les doublons et à préparer D3.
// ---------------------------------------------------------------------------
// D0         : BUZZER_SIGNAL ; aussi le RX de Serial1 (non utilisé)
// D2         : BUTTONS_CENTER
// D3         : BUTTONS_RIGHT
// D4         : BUTTONS_LEFT
// D5         : BUTTONS_UP
// D6         : BUTTONS_DOWN
// D7         : DISPLAY_CS
// D8         : DISPLAY_RES
// D9         : DISPLAY_DC
// D10        : CAMERA_CS
// D11        : bus SPI, MOSI (caméra, écran)
// D12        : bus SPI, MISO (caméra)
// D13        : bus SPI, SCK (caméra, écran) ; aussi la LED intégrée de la carte
// SDA (= A4) : bus I2C, SDA (caméra, BME680) ; A4 n'est plus utilisable en analogique
// SCL (= A5) : bus I2C, SCL (caméra, BME680) ; A5 n'est plus utilisable en analogique
// A0         : MQ2_A0 ; aussi la sortie DAC de la carte (non utilisée)
// A1         : LEDS_SER (74HC595, données)
// A2         : LEDS_RCLK (74HC595, verrouillage)
// A3         : LEDS_SRCLK (74HC595, horloge de décalage)
// Adresses I2C (7 bits) : 0x3C caméra, 0x76 BME680

// ---------------------------------------------------------------------------
// Caméra ArduCAM Mini 5MP Plus, OV5642 (CONTEXTE_TECHNIQUE.md, § 3.3)
// Provisoire tant que D3 est ouverte.
// Bus : SPI mode 0 (registres et tampon image) + I2C (capteur, adresse 0x78/0x79,
// soit 0x3C sur 7 bits). Entrées/sorties tolérantes 5 V : pas de convertisseur.
// Broche officielle -> broche de la carte
//   CS   -> D10           : définie dans le code
//   MOSI -> D11           : bus SPI imposé par la carte
//   MISO -> D12           : bus SPI imposé par la carte
//   SCK  -> D13           : bus SPI imposé par la carte
//   SDA  -> SDA (près d'AREF), reliée à A4 : objet Wire (broche 18 du variant)
//   SCL  -> SCL (près d'AREF), reliée à A5 : objet Wire (broche 19 du variant)
//   VCC  -> 5V            : environ 300 mA en fonctionnement (doc Arducam)
//   GND  -> GND
constexpr uint8_t CAMERA_CS_PIN = 10;

// ---------------------------------------------------------------------------
// Capteur environnemental BME680, module Joy-IT SEN-BME680 (CONTEXTE_TECHNIQUE.md, § 3.2)
// Provisoire tant que D3 est ouverte ; bus I2C proposé, D1 non tranchée.
// Bus : I2C partagé avec la caméra, adresse 0x76 (BME680_I2C_ADDRESS dans config.h).
// Aucune broche à définir dans le code : bus I2C imposé par la carte.
// Broche officielle -> broche de la carte
//   VIN -> 5V                : alimentation du module
//   3Vo -> non connectée
//   GND -> GND
//   SCK -> SCL (près d'AREF) : horloge I2C, objet Wire
//   SDI -> SDA (près d'AREF) : données I2C, objet Wire
//   SDO -> GND               : fixe l'adresse I2C à 0x76
//   CS  -> non connectée

// ---------------------------------------------------------------------------
// Capteur de gaz et fumée MQ-2 (CONTEXTE_TECHNIQUE.md, § 3.8)
// Provisoire tant que D3 est ouverte (câblage de test P1.3).
// Sortie analogique 0 à 5 V lue par l'ADC (référence : alimentation 5 V de la carte).
// Broche officielle -> broche de la carte
//   VCC -> 5V  : alimente aussi le chauffage du capteur
//   GND -> GND
//   A0  -> A0  : définie dans le code
//   D0  -> non connectée : sortie tout-ou-rien, seuil réglé par le potentiomètre du module
constexpr uint8_t MQ2_A0_PIN = A0;

// ---------------------------------------------------------------------------
// Écran TFT ST7735S 1,8" 128x160, 3,3 V, DollaTek (CONTEXTE_TECHNIQUE.md, § 3.12)
// Provisoire tant que D3 est ouverte (câblage de test P1.4).
// Bus : SPI mode 0 partagé avec la caméra, sans MISO. Module en 3,3 V : SCL, SDA, RES, DC
// et CS passent chacun par un diviseur 1 kOhm (côté carte) / 2 kOhm (vers GND), 5 V -> environ 3,3 V.
// Broche officielle -> broche de la carte
//   GND -> GND
//   VCC -> 3.3V
//   SCL -> D13 (diviseur) : bus SPI imposé par la carte (SCK)
//   SDA -> D11 (diviseur) : bus SPI imposé par la carte (MOSI)
//   RES -> D8  (diviseur) : définie dans le code
//   DC  -> D9  (diviseur) : définie dans le code
//   CS  -> D7  (diviseur) : définie dans le code
//   BLK -> 3.3V           : rétroéclairage toujours allumé
constexpr uint8_t DISPLAY_CS_PIN = 7;
constexpr uint8_t DISPLAY_RES_PIN = 8;
constexpr uint8_t DISPLAY_DC_PIN = 9;

// ---------------------------------------------------------------------------
// LED rouge, bleue, jaune, orange (CONTEXTE_TECHNIQUE.md, § 3.4 à 3.7), via un registre à décalage 74HC595
// Provisoire tant que D3 est ouverte (câblage de test P1.5).
// 74HC595 absent de CONTEXTE_TECHNIQUE.md : noms de broches de sa datasheet.
// Commandé en logiciel avec shiftOut, pas par le bus SPI matériel (partagé par la caméra et l'écran).
// Broche du 74HC595 -> broche de la carte
//   SER   -> A1  : données, définie dans le code
//   RCLK  -> A2  : verrouillage des sorties, défini dans le code
//   SRCLK -> A3  : horloge de décalage, définie dans le code
//   OE    -> GND : sorties toujours actives
//   SRCLR -> 5V  : registre jamais remis à zéro
//   VCC   -> 5V (à confirmer)
//   GND   -> GND
// Sorties actives à l'état haut ; chaque LED en série avec une résistance de 270 Ohm vers GND
//   QA        -> non connectée
//   QB        -> LED rouge (température)
//   QC        -> LED bleue (humidité)
//   QD        -> LED jaune (qualité de l'air)
//   QE        -> LED orange (mode sécurité)
//   QF à QH'  -> non connectées
constexpr uint8_t LEDS_SER_PIN = A1;
constexpr uint8_t LEDS_RCLK_PIN = A2;
constexpr uint8_t LEDS_SRCLK_PIN = A3;

// Bit de chaque LED dans l'octet envoyé (shiftOut en MSBFIRST : bit 0 -> QA, bit 1 -> QB, ..., bit 7 -> QH)
constexpr uint8_t LEDS_RED_BIT = 1;
constexpr uint8_t LEDS_BLUE_BIT = 2;
constexpr uint8_t LEDS_YELLOW_BIT = 3;
constexpr uint8_t LEDS_ORANGE_BIT = 4;

// ---------------------------------------------------------------------------
// Boutons poussoirs (CONTEXTE_TECHNIQUE.md, § 3.10)
// Provisoire tant que D3 est ouverte (câblage de test P1.5).
// Entrées en INPUT_PULLUP (résistance de tirage interne) : relâché = HIGH, appuyé = LOW.
// Chaque bouton : patte A1 sur la broche de la carte, patte B2 au GND (test de continuité du 2026-11-09).
// Bouton -> broche de la carte
//   CENTRE (validation) -> D2 : définie dans le code
//   DROITE              -> D3 : définie dans le code
//   GAUCHE              -> D4 : définie dans le code
//   HAUT                -> D5 : définie dans le code
//   BAS                 -> D6 : définie dans le code
constexpr uint8_t BUTTONS_CENTER_PIN = 2;
constexpr uint8_t BUTTONS_RIGHT_PIN = 3;
constexpr uint8_t BUTTONS_LEFT_PIN = 4;
constexpr uint8_t BUTTONS_UP_PIN = 5;
constexpr uint8_t BUTTONS_DOWN_PIN = 6;

// ---------------------------------------------------------------------------
// Buzzer passif SVM06N (CONTEXTE_TECHNIQUE.md, § 3.9)
// Provisoire tant que D3 est ouverte (câblage de test P1.5).
// Passif : la carte produit le signal carré avec tone().
// D0 est aussi le RX de Serial1 (UART2) : ne pas utiliser Serial1 (Serial passe par d'autres broches).
// Broche officielle -> broche de la carte
//   Signal -> D0 : définie dans le code
//   V+     -> 5V (à confirmer)
//   GND    -> GND
constexpr uint8_t BUZZER_SIGNAL_PIN = 0;

// ---------------------------------------------------------------------------
// Modèle de bloc par composant (à recopier, ne pas décommenter tel quel)
// ---------------------------------------------------------------------------
// <Composant> (CONTEXTE_TECHNIQUE.md, § x.x)
// Broche officielle -> broche de la carte
//   <BROCHE_1> -> Dx   : définie dans le code
//   <BROCHE_2> -> D13  : bus imposé par la carte (commentaire seulement)
//   VCC        -> 3V3 ou 5V
//   GND        -> GND
// constexpr uint8_t <COMPOSANT>_<BROCHE_1>_PIN = x;
