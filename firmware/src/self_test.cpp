#include <Arduino.h>
#include "self_test.h"

// Appelle les <composant>_test() des composants validés, l'un après l'autre
void self_test_run() {
    Serial.println("=== Self-test : debut ===");

    // Ajouter ici une ligne par composant validé sur le matériel

    Serial.println("=== Self-test : fin ===");
}
