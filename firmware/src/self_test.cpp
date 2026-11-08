#include <Arduino.h>
#include "self_test.h"
#include "components/camera.h"
#include "components/bme680.h"

// Colonne où commencent les valeurs ; même valeur que VALUE_COLUMN de serial_monitor.py
constexpr int SELF_TEST_VALUE_COLUMN = 33;

// Bannière affichée avant chaque test, même format pour tous les composants
static void self_test_banner(const char *name) {
    Serial.print("----- TEST ");
    Serial.print(name);
    Serial.println(" -----");
}

// Début de ligne : « [tag] libellé », points jusqu'à la colonne des valeurs, espace
static void self_test_print_label(const char *tag, const char *label) {
    char line[64];
    int used = snprintf(line, sizeof(line), "[%s] %s ", tag, label);
    Serial.print(line);
    for (int i = used; i < SELF_TEST_VALUE_COLUMN - 1; i++) {
        Serial.print('.');
    }
    Serial.print(' ');
}

void self_test_print(const char *tag, const char *label, const char *value) {
    self_test_print_label(tag, label);
    Serial.println(value);
}

void self_test_print(const char *tag, const char *label, unsigned long value, const char *unit) {
    self_test_print_label(tag, label);
    Serial.print(value);
    Serial.print(' ');
    Serial.println(unit);
}

void self_test_print(const char *tag, const char *label, float value, const char *unit) {
    self_test_print_label(tag, label);
    Serial.print(value, 2);
    Serial.print(' ');
    Serial.println(unit);
}

// Chaque test : bannière, test, puis une ligne vide
void self_test_run() {
    Serial.println("\n=== Self-test : debut ===");
    Serial.println();

    self_test_banner("CAMERA");
    camera_test();
    Serial.println();

    self_test_banner("BME680");
    bme680_test();
    Serial.println();

    Serial.println("=== Self-test : fin ===");
}
