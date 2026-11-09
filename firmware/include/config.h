#pragma once
#include <Arduino.h>

constexpr uint32_t SERIAL_BAUD = 460800;


// Résolution du JPEG : copier une des constantes ci-dessous.
// #define et non constexpr : la valeur vient de la bibliothèque ArduCAM, incluse seulement dans camera.cpp.
// OV5642_320x240, OV5642_640x480, OV5642_1024x768, OV5642_1280x960, OV5642_1600x1200, OV5642_2048x1536, OV5642_2592x1944
// OV5642_1920x1080 existe dans la bibliothèque mais n'y est pas implémentée : ne pas l'utiliser.
#define CAMERA_RESOLUTION OV5642_1024x768

constexpr uint32_t CAMERA_SPI_FREQUENCY_HZ = 4000000;

constexpr size_t CAMERA_READ_BLOCK_BYTES = 512;


// BME680 : adresse I2C (SDO relié à GND)
constexpr uint8_t BME680_I2C_ADDRESS = 0x76;


// MQ-2 : seuils sur l'échelle de mq2_read() (0 à 16383), tirés du démarrage à froid du 2026-11-08 (P1.3).
// Préchauffage : aucune alerte avant MQ2_PREHEAT_S depuis la mise sous tension.
constexpr uint32_t MQ2_PREHEAT_S = 300;

// Alerte au-dessus de MQ2_ALERT_THRESHOLD sur N échantillons consécutifs ; fin sous MQ2_ALERT_END_THRESHOLD.
constexpr uint16_t MQ2_ALERT_THRESHOLD = 3000;
constexpr uint16_t MQ2_ALERT_END_THRESHOLD = 2500;
constexpr uint8_t MQ2_ALERT_CONFIRM_SAMPLES = 2;


// Écran ST7735S 128x160 : variante d'initialisation de la bibliothèque Adafruit ST7735.
// #define et non constexpr : la valeur vient de la bibliothèque, incluse seulement dans display.cpp.
// Variantes 128x160 : INITR_BLACKTAB, INITR_GREENTAB, INITR_REDTAB
#define DISPLAY_INIT_VARIANT INITR_BLACKTAB

// Fréquence SPI de l'écran après l'initialisation (la bibliothèque envoie l'initialisation à sa vitesse par défaut).
constexpr uint32_t DISPLAY_SPI_FREQUENCY_HZ = 4000000;

// Début de la zone visible dans la mémoire du contrôleur, en colonnes et lignes du portrait (rotation 0).
// Constat du 2026-11-08 : avec 0 et 0, affichage décalé de 1 pixel à gauche et en haut.
// En rotation 1 et 3, la bibliothèque applique le décalage de colonne en vertical et celui de ligne en horizontal.
constexpr int8_t DISPLAY_COL_OFFSET = 1;
constexpr int8_t DISPLAY_ROW_OFFSET = 1;

// Orientation : 0 ou 2 = portrait 128x160 ; 1 ou 3 = paysage 160x128 (changer 1 <-> 3 si l'image est à l'envers).
constexpr uint8_t DISPLAY_ROTATION = 3;


// Boutons : durée pendant laquelle une lecture doit rester stable pour être prise en compte (anti-rebond).
constexpr unsigned long BUTTONS_DEBOUNCE_MS = 20;