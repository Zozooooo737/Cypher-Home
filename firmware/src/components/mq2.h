#pragma once
#include <Arduino.h>

// Règle l'ADC sur 14 bits ; toujours true : une entrée analogique ne permet pas de détecter l'absence du capteur
bool mq2_setup();

// Moyenne de plusieurs lectures de A0, de 0 à 16383 (plus de gaz ou de fumée = valeur plus haute)
uint16_t mq2_read();

// Ajoute un échantillon (un toutes les 5 s) et met à jour l'alerte ; aucune alerte pendant le préchauffage
void mq2_update(uint16_t value);

// true une fois MQ2_PREHEAT_S écoulé depuis la mise sous tension, jusqu'au reset
bool mq2_ready();

// true pendant une alerte gaz ou fumée
bool mq2_gas_detected();

// Test : lit et ajoute un échantillon, puis affiche valeur, tension, état et alerte
bool mq2_test();
