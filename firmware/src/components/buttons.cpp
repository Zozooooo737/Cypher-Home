#include <Arduino.h>

#include "buttons.h"
#include "config.h"
#include "pins.h"
#include "self_test.h"

constexpr unsigned long BUTTONS_TEST_DURATION_MS = 7000;

constexpr uint8_t BUTTONS_PINS[BUTTON_COUNT] = {
    BUTTONS_CENTER_PIN, BUTTONS_RIGHT_PIN, BUTTONS_LEFT_PIN, BUTTONS_UP_PIN, BUTTONS_DOWN_PIN};

static bool s_raw[BUTTON_COUNT];
static bool s_pressed[BUTTON_COUNT];
static unsigned long s_changed_ms[BUTTON_COUNT];

bool buttons_setup() {
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(BUTTONS_PINS[i], INPUT_PULLUP);
    }
    return true;
}

void buttons_update() {
    unsigned long now = millis();
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        bool raw = digitalRead(BUTTONS_PINS[i]) == LOW;
        if (raw != s_raw[i]) {
            s_raw[i] = raw;
            s_changed_ms[i] = now;
        } else if (now - s_changed_ms[i] >= BUTTONS_DEBOUNCE_MS) {
            s_pressed[i] = raw;
        }
    }
}

bool buttons_is_pressed(Button button) {
    return s_pressed[button];
}

bool buttons_test() {
    const char *const names[BUTTON_COUNT] = {"centre", "droite", "gauche", "haut", "bas"};
    bool previous[BUTTON_COUNT];

    buttons_update();
    delay(BUTTONS_DEBOUNCE_MS);
    buttons_update();

    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        previous[i] = buttons_is_pressed(static_cast<Button>(i));
        if (previous[i]) {
            Serial.println("[buttons] KO");
            return false;
        }
    }
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        self_test_print("buttons", names[i], "pas appuye");
    }

    unsigned long start = millis();
    while (millis() - start < BUTTONS_TEST_DURATION_MS) {
        buttons_update();
        for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
            bool pressed = buttons_is_pressed(static_cast<Button>(i));
            if (pressed != previous[i]) {
                previous[i] = pressed;
                self_test_print("buttons", names[i], pressed ? "appuye" : "pas appuye");
            }
        }
    }
    return true;
}
