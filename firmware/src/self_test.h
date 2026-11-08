#pragma once
#include <Arduino.h>

// Enchaîne les tests des composants validés sur le matériel
void self_test_run();

// Affiche « [tag] libellé ..... valeur unité » : les points alignent toutes les valeurs
void self_test_print(const char *tag, const char *label, const char *value);
void self_test_print(const char *tag, const char *label, unsigned long value, const char *unit);
void self_test_print(const char *tag, const char *label, float value, const char *unit);
