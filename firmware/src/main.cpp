#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "self_test.h"
#include "components/camera.h"
#include "components/bme680.h"
#include "components/mq2.h"
#include "components/display.h"
#include "components/leds.h"
#include "components/buttons.h"
#include "components/buzzer.h"

constexpr unsigned long SELF_TEST_PAUSE_MS = 5000;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);

    pinMode(CAMERA_CS_PIN, OUTPUT);
    digitalWrite(CAMERA_CS_PIN, HIGH);
    pinMode(DISPLAY_CS_PIN, OUTPUT);
    digitalWrite(DISPLAY_CS_PIN, HIGH);

    camera_setup();
    bme680_setup();
    mq2_setup();
    display_setup();
    leds_setup();
    buttons_setup();
    buzzer_setup();
}

void loop() {
    self_test_run();
    delay(SELF_TEST_PAUSE_MS);
}
