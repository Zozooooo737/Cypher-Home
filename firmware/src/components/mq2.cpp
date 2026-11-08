#include <Arduino.h>

#include "mq2.h"
#include "config.h"
#include "pins.h"
#include "self_test.h"

constexpr int MQ2_ADC_RESOLUTION_BITS = 14;
constexpr uint8_t MQ2_SAMPLE_COUNT = 16;

static bool s_ready = false;
static uint8_t s_above_count = 0;
static bool s_gas_detected = false;

bool mq2_setup() {
    analogReadResolution(MQ2_ADC_RESOLUTION_BITS);
    return true;
}

uint16_t mq2_read() {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < MQ2_SAMPLE_COUNT; i++) {
        sum += analogRead(MQ2_A0_PIN);
    }
    return sum / MQ2_SAMPLE_COUNT;
}

void mq2_update(uint16_t value) {
    if (s_gas_detected) {
        s_gas_detected = value >= MQ2_ALERT_END_THRESHOLD;
        s_above_count = 0;
    } else if (mq2_ready()) {
        s_above_count = value > MQ2_ALERT_THRESHOLD ? s_above_count + 1 : 0;
        s_gas_detected = s_above_count >= MQ2_ALERT_CONFIRM_SAMPLES;
    }
}

bool mq2_ready() {
    s_ready = s_ready || millis() >= MQ2_PREHEAT_S * 1000UL;
    return s_ready;
}

bool mq2_gas_detected() {
    return s_gas_detected;
}

bool mq2_test() {
    uint16_t raw = mq2_read();
    mq2_update(raw);
    float voltage = raw * analogReference() / (1 << MQ2_ADC_RESOLUTION_BITS);

    self_test_print("mq2", "depuis demarrage", millis() / 1000, "s");
    self_test_print("mq2", "A0 brut", static_cast<unsigned long>(raw), "/ 16383");
    self_test_print("mq2", "A0 tension", voltage, "V");
    self_test_print("mq2", "etat", mq2_ready() ? "PRET" : "PRECHAUFFAGE");
    self_test_print("mq2", "alerte", mq2_gas_detected() ? "OUI" : "NON");
    return true;
}
