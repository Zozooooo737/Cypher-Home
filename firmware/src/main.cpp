#include <Arduino.h>
#include "config.h"
#include "self_test.h"
#include "components/camera.h"

constexpr unsigned long SELF_TEST_PAUSE_MS = 5000;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);

    camera_setup();
}

void loop() {
    self_test_run();
    delay(SELF_TEST_PAUSE_MS);
}
