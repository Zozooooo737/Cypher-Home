#pragma once

// Met en place la caméra ; false si elle ne répond pas
bool camera_setup();

// Capture une image ; jpeg_length dépasse de quelques octets la fin du JPEG (FF D9)
bool camera_capture(uint32_t &jpeg_length);

// Début de lecture de la capture : réserve le bus SPI jusqu'à camera_read_end()
void camera_read_begin();

// Lit au plus max_len octets de l'image ; renvoie 0 quand elle est entièrement lue
size_t camera_read_block(uint8_t *buf, size_t max_len);

// Fin de lecture : libère le bus SPI
void camera_read_end();

// Test : une capture envoyée au PC par le port série, puis ses caractéristiques, ou « [camera] KO »
bool camera_test();

// Test : une photo envoyée en HTTP, puis taille, capture, envoi, total et débit, ou « [camera] KO » / « [http] KO » et le code d'erreur
bool camera_http_test();

// Banc P2.5 : 3 photos envoyées en HTTP dans chaque résolution, puis retour à celle de config.h ; mesures, ou « [camera] KO » / « [http] KO » et le code d'erreur
void camera_http_bench();
