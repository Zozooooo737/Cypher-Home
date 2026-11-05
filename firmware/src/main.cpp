#include <Arduino.h>
#include "self_test.h"

// Développement, supprimée en P4.1
constexpr unsigned long SELF_TEST_PAUSE_MS = 5000;

// Délai maximal d'attente de l'ouverture du port série USB
constexpr unsigned long SERIAL_WAIT_MAX_MS = 3000;

void setup() {
    // Port série USB natif : attendre son ouverture, sans bloquer au-delà du délai
    Serial.begin(115200);
    unsigned long start_ms = millis();
    while (!Serial && (millis() - start_ms < SERIAL_WAIT_MAX_MS)) {
    }

    // Broches CS des périphériques SPI à HIGH, avant tout <composant>_setup()

    // Appels des <composant>_setup()
}

void loop() {
    // Développement : tests des composants validés, puis pause
    self_test_run();
    delay(SELF_TEST_PAUSE_MS);
}
