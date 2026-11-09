#include <Arduino.h>

#include "leds.h"
#include "pins.h"
#include "self_test.h"

constexpr unsigned long LEDS_TEST_STEP_MS = 500;
constexpr unsigned long LEDS_TEST_ALL_MS = 1000;

constexpr uint8_t LEDS_ALL_MASK = bit(LEDS_RED_BIT) | bit(LEDS_BLUE_BIT) | bit(LEDS_YELLOW_BIT) | bit(LEDS_ORANGE_BIT);

static uint8_t s_state = 0;

static void leds_write() {
    shiftOut(LEDS_SER_PIN, LEDS_SRCLK_PIN, MSBFIRST, s_state);
    digitalWrite(LEDS_RCLK_PIN, HIGH);
    digitalWrite(LEDS_RCLK_PIN, LOW);
}

bool leds_setup() {
    pinMode(LEDS_SER_PIN, OUTPUT);
    pinMode(LEDS_RCLK_PIN, OUTPUT);
    pinMode(LEDS_SRCLK_PIN, OUTPUT);
    leds_set_all(false);
    return true;
}

void leds_set(uint8_t led, bool on) {
    if (on) {
        s_state |= bit(led);
    } else {
        s_state &= ~bit(led);
    }
    leds_write();
}

void leds_set_all(bool on) {
    s_state = on ? LEDS_ALL_MASK : 0;
    leds_write();
}

bool leds_test() {
    const uint8_t leds[] = {LEDS_RED_BIT, LEDS_BLUE_BIT, LEDS_YELLOW_BIT, LEDS_ORANGE_BIT};
    const char *const names[] = {"rouge", "bleue", "jaune", "orange"};

    leds_set_all(false);
    self_test_print("leds", "toutes", "eteintes");

    for (uint8_t i = 0; i < 4; i++) {
        leds_set(leds[i], true);
        self_test_print("leds", names[i], "allumee seule");
        delay(LEDS_TEST_STEP_MS);
        leds_set(leds[i], false);
    }

    leds_set_all(true);
    self_test_print("leds", "toutes", "allumees");
    delay(LEDS_TEST_ALL_MS);

    leds_set_all(false);
    self_test_print("leds", "toutes", "eteintes");
    delay(LEDS_TEST_ALL_MS);
    return true;
}
