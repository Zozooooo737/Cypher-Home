#pragma once
#include <Arduino.h>

// Met en place la caméra ; false si elle ne répond pas (raison : camera_last_error())
bool camera_setup();

// Indique si la caméra est prête
bool camera_is_ready();

// Capture une image ; jpeg_length dépasse de quelques octets la fin du JPEG (FF D9)
bool camera_capture(uint32_t &jpeg_length);

// Début de lecture de la capture : réserve le bus SPI jusqu'à camera_read_end()
void camera_read_begin();

// Lit au plus max_len octets de l'image ; renvoie 0 quand elle est entièrement lue
size_t camera_read_block(uint8_t *buf, size_t max_len);

// Fin de lecture : libère le bus SPI
void camera_read_end();

// Texte de la dernière erreur
const char *camera_last_error();

// Test : une capture envoyée au PC par le port série, puis une ligne de synthèse
bool camera_test();
