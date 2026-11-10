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
#include "components/wifi.h"

constexpr unsigned long WIFI_WATCH_PERIOD_MS = 5000;

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
    wifi_setup();
}

void loop() {
    static bool s_tested = false;
    static bool s_up = false;
    static unsigned long s_lost_ms = 0;

    if (!s_tested) {
        Serial.println("----- TEST WIFI -----");
        s_up = wifi_test();
        Serial.println();
        s_tested = true;
        s_lost_ms = millis();
    }

    delay(WIFI_WATCH_PERIOD_MS);

    if (wifi_connected()) {
        s_up = true;
        return;
    }
    if (s_up) {
        self_test_print("wifi", "liaison", "perdue");
        s_lost_ms = millis();
        s_up = false;
    }
    if (wifi_ensure_connected()) {
        self_test_print("wifi", "reprise apres", millis() - s_lost_ms, "ms");
        s_up = true;
    }
}
