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
// D10        : CAMERA_CS
// D11        : bus SPI, MOSI (caméra)
// D12        : bus SPI, MISO (caméra)
// D13        : bus SPI, SCK (caméra) ; aussi la LED intégrée de la carte
// SDA (= A4) : bus I2C, SDA (caméra) ; A4 n'est plus utilisable en analogique
// SCL (= A5) : bus I2C, SCL (caméra) ; A5 n'est plus utilisable en analogique

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
// Modèle de bloc par composant (à recopier, ne pas décommenter tel quel)
// ---------------------------------------------------------------------------
// <Composant> (CONTEXTE_TECHNIQUE.md, § x.x)
// Broche officielle -> broche de la carte
//   <BROCHE_1> -> Dx   : définie dans le code
//   <BROCHE_2> -> D13  : bus imposé par la carte (commentaire seulement)
//   VCC        -> 3V3 ou 5V
//   GND        -> GND
// constexpr uint8_t <COMPOSANT>_<BROCHE_1>_PIN = x;
