#include <Arduino.h>
#include "self_test.h"
#include "components/camera.h"
// Colonne où commencent les valeurs ; même valeur que VALUE_COLUMN de serial_monitor.py
constexpr int SELF_TEST_VALUE_COLUMN = 33;

// Bannière affichée avant chaque test, même format pour tous les composants
static void self_test_banner(const char *name) {
    Serial.print("----- TEST ");
    Serial.print(name);
    Serial.println(" -----");
}

// Chaque test : bannière, test, puis une ligne vide
void self_test_run() {
    Serial.println("\n=== Self-test : debut ===");
    Serial.println();

    self_test_banner("CAMERA");
    camera_test();
    Serial.println();

    Serial.println("=== Self-test : fin ===");
}
