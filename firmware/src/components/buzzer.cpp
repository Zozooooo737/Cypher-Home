#include <Arduino.h>

#include "buzzer.h"
#include "pins.h"
#include "self_test.h"

constexpr unsigned long BUZZER_TEST_LOW_HZ = 1000;
constexpr unsigned long BUZZER_TEST_HIGH_HZ = 2000;
constexpr unsigned long BUZZER_TEST_SOUND_MS = 300;
constexpr unsigned long BUZZER_TEST_PAUSE_MS = 200;

bool buzzer_setup() {
    pinMode(BUZZER_SIGNAL_PIN, OUTPUT);
    return true;
}

void buzzer_play(unsigned int freq_hz, unsigned long duration_ms) {
    tone(BUZZER_SIGNAL_PIN, freq_hz, duration_ms);
}

bool buzzer_test() {
    buzzer_play(BUZZER_TEST_LOW_HZ, BUZZER_TEST_SOUND_MS);
    self_test_print("buzzer", "son 1", BUZZER_TEST_LOW_HZ, "Hz");
    delay(BUZZER_TEST_SOUND_MS + BUZZER_TEST_PAUSE_MS);

    buzzer_play(BUZZER_TEST_HIGH_HZ, BUZZER_TEST_SOUND_MS);
    self_test_print("buzzer", "son 2", BUZZER_TEST_HIGH_HZ, "Hz");
    delay(BUZZER_TEST_SOUND_MS);
    return true;
}
