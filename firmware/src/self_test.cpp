#include <Arduino.h>
#include "self_test.h"
#include "components/camera.h"
#include "components/bme680.h"
#include "components/mq2.h"
#include "components/display.h"
#include "components/leds.h"
#include "components/buttons.h"
#include "components/buzzer.h"
#include "components/wifi.h"
#include "components/mqtt.h"

constexpr int SELF_TEST_VALUE_COLUMN = 33;

static unsigned int s_ok_count = 0;
static unsigned int s_ko_count = 0;

static void self_test_banner(const char *name) {
    Serial.print("----- TEST ");
    Serial.print(name);
    Serial.println(" -----");
}

static void self_test_print_label(const char *tag, const char *label) {
    char line[64];
    int used = snprintf(line, sizeof(line), "[%s] %s ", tag, label);
    Serial.print(line);
    for (int i = used; i < SELF_TEST_VALUE_COLUMN - 1; i++) {
        Serial.print('.');
    }
    Serial.print(' ');
}

void self_test_print(const char *tag, const char *label, const char *value) {
    self_test_print_label(tag, label);
    Serial.println(value);
}

void self_test_print(const char *tag, const char *label, unsigned long value, const char *unit) {
    self_test_print_label(tag, label);
    Serial.print(value);
    Serial.print(' ');
    Serial.println(unit);
}

void self_test_print(const char *tag, const char *label, float value, const char *unit) {
    self_test_print_label(tag, label);
    Serial.print(value, 2);
    Serial.print(' ');
    Serial.println(unit);
}

static bool self_test_step(const char *name, bool (*test)()) {
    self_test_banner(name);
    bool ok = test();
    Serial.println();
    return ok;
}

static bool self_test_skip(const char *name, const char *line) {
    self_test_banner(name);
    Serial.println(line);
    Serial.println();
    return false;
}

static void self_test_recap(const char *name, bool ok) {
    self_test_print("recap", name, ok ? "OK" : "KO");
    if (ok) {
        s_ok_count++;
    } else {
        s_ko_count++;
    }
}

void self_test_run() {
    Serial.println("\n=== Self-test au demarrage ===\n");

    bool camera_ok = self_test_step("CAMERA", camera_test);
    bool bme680_ok = self_test_step("BME680", bme680_test);
    bool mq2_ok = self_test_step("MQ2", mq2_test);
    bool wifi_ok = self_test_step("WIFI", wifi_test);
    bool mqtt_ok = wifi_ok ? self_test_step("MQTT", mqtt_test) : self_test_skip("MQTT", "[mqtt] KO (wifi)");
    bool photo_ok = !wifi_ok     ? self_test_skip("PHOTO HTTP", "[http] KO (wifi)")
                    : !camera_ok ? self_test_skip("PHOTO HTTP", "[http] KO (camera)")
                                 : self_test_step("PHOTO HTTP", camera_http_test);
    bool display_ok = self_test_step("DISPLAY", display_test);
    bool leds_ok = self_test_step("LEDS", leds_test);
    bool buzzer_ok = self_test_step("BUZZER", buzzer_test);
    bool buttons_ok = self_test_step("BUTTONS", buttons_test);

    self_test_banner("RECAPITULATIF");
    self_test_recap("camera", camera_ok);
    self_test_recap("bme680", bme680_ok);
    self_test_recap("mq2", mq2_ok);
    self_test_recap("wifi", wifi_ok);
    self_test_recap("mqtt", mqtt_ok);
    self_test_recap("photo http", photo_ok);
    self_test_recap("ecran", display_ok);
    self_test_recap("leds", leds_ok);
    self_test_recap("buzzer", buzzer_ok);
    self_test_recap("boutons", buttons_ok);

    char total[24];
    snprintf(total, sizeof(total), "%u OK, %u KO", s_ok_count, s_ko_count);
    self_test_print("recap", "total", total);
    self_test_print("recap", "duree", millis() / 1000, "s");
    Serial.println();
}
