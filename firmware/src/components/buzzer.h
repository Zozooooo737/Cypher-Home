#pragma once
#include <Arduino.h>

// Met la broche Signal en sortie, au repos ; toujours true : l'absence du buzzer n'est pas détectable
bool buzzer_setup();

// Joue un son de freq_hz pendant duration_ms sans bloquer (tone() : un nouveau son remplace le précédent)
void buzzer_play(unsigned int freq_hz, unsigned long duration_ms);

// Test : 1000 Hz 300 ms, pause 200 ms, puis 2000 Hz 300 ms ; résultat à vérifier à l'oreille
bool buzzer_test();
