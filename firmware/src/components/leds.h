#pragma once
#include <Arduino.h>

// Met les broches du 74HC595 en sortie et éteint les 4 LED ; toujours true : l'absence du registre n'est pas détectable
bool leds_setup();

// Allume ou éteint une LED (led : LEDS_RED_BIT, LEDS_BLUE_BIT, LEDS_YELLOW_BIT ou LEDS_ORANGE_BIT de pins.h) sans toucher aux autres
void leds_set(uint8_t led, bool on);

// Allume ou éteint les 4 LED
void leds_set_all(bool on);

// Test : tout éteint, chaque LED seule 500 ms, les 4 allumées 1 s, puis éteintes 1 s ; résultat à vérifier à l'oeil
bool leds_test();
