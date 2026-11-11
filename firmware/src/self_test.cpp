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

// Colonne où commencent les valeurs ; même valeur que VALUE_COLUMN de serial_monitor.py
constexpr int SELF_TEST_VALUE_COLUMN = 33;

constexpr unsigned long SELF_TEST_TOGETHER_HZ = 1000;
constexpr unsigned long SELF_TEST_TOGETHER_SOUND_MS = 1500;

static unsigned long s_pass_count = 0;

// Bannière affichée avant chaque test, même format pour tous les composants
static void self_test_banner(const char *name) {
    Serial.print("----- TEST ");
    Serial.print(name);
    Serial.println(" -----");
}

// Début de ligne : « [tag] libellé », points jusqu'à la colonne des valeurs, espace
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

static void self_test_together() {
    Bme680Data data;
    leds_set_all(true);
    self_test_print("ensemble", "leds", "4 allumees");
    buzzer_play(SELF_TEST_TOGETHER_HZ, SELF_TEST_TOGETHER_SOUND_MS);
    self_test_print("ensemble", "buzzer", SELF_TEST_TOGETHER_HZ, "Hz");
    if (!bme680_read(data)) {
        Serial.println("[bme680] KO");
    }
    display_show_measures(data, mq2_read());
    self_test_print("ensemble", "ecran", "mesures affichees");
    camera_test();
    leds_set_all(false);
}

// Chaque test : bannière, test, puis une ligne vide
void self_test_run() {
    s_pass_count++;
    Serial.print("\n=== Self-test : passe ");
    Serial.print(s_pass_count);
    Serial.print(", ");
    Serial.print(millis() / 1000);
    Serial.println(" s depuis le demarrage ===");
    Serial.println();

    self_test_banner("CAMERA");
    camera_test();
    Serial.println();

    self_test_banner("BME680");
    bme680_test();
    Serial.println();

    self_test_banner("MQ2");
    mq2_test();
    Serial.println();

    self_test_banner("DISPLAY");
    display_test();
    Serial.println();

    self_test_banner("LEDS");
    leds_test();
    Serial.println();

    self_test_banner("BUTTONS");
    buttons_test();
    Serial.println();

    self_test_banner("BUZZER");
    buzzer_test();
    Serial.println();

    self_test_banner("WIFI");
    wifi_test();
    Serial.println();

    self_test_banner("MQTT");
    mqtt_test();
    Serial.println();

    self_test_banner("ENSEMBLE");
    self_test_together();
    Serial.println();

    Serial.println("=== Self-test : fin ===");
}
