#pragma once
#include <Arduino.h>
#include "bme680.h"

// Met en place l'écran sur le bus SPI ; toujours true : sans MISO, l'absence de l'écran n'est pas détectable
bool display_setup();

// Efface l'écran et affiche les mesures du BME680 et la valeur brute du MQ-2
void display_show_measures(const Bme680Data &data, uint16_t mq2_raw);

// Test : fond noir, texte, puis « Rouge », « Vert », « Bleu » dans leur couleur ; résultat à vérifier à l'oeil
bool display_test();
