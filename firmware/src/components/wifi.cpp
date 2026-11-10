#include <Arduino.h>
#include <WiFiS3.h>

#include "wifi.h"
#include "config.h"
#include "secrets.h"
#include "self_test.h"

static unsigned long s_connect_ms = 0;
static uint8_t s_attempts = 0;

bool wifi_setup() {
    unsigned long start_ms = millis();
    WiFi.setTimeout(WIFI_CONNECT_TIMEOUT_MS);
    for (s_attempts = 1; s_attempts <= WIFI_CONNECT_ATTEMPTS; s_attempts++) {
        if (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
            continue;
        }
        unsigned long dhcp_start_ms = millis();
        while (WiFi.localIP() == INADDR_NONE && millis() - dhcp_start_ms < WIFI_CONNECT_TIMEOUT_MS) {
        }
        if (WiFi.localIP() != INADDR_NONE) {
            s_connect_ms = millis() - start_ms;
            return true;
        }
    }
    return false;
}

bool wifi_connected() {
    return WiFi.status() == WL_CONNECTED;
}

bool wifi_ensure_connected() {
    return wifi_connected() || wifi_setup();
}

bool wifi_test() {
    if (!wifi_ensure_connected()) {
        Serial.println("[wifi] KO");
        return false;
    }

    char attempts[4];
    snprintf(attempts, sizeof(attempts), "%u", s_attempts);
    char rssi[16];
    snprintf(rssi, sizeof(rssi), "%ld dBm", static_cast<long>(WiFi.RSSI()));

    self_test_print("wifi", "firmware module", WiFi.firmwareVersion());
    self_test_print("wifi", "ssid", WIFI_SSID);
    self_test_print("wifi", "connexion", s_connect_ms, "ms");
    self_test_print("wifi", "tentatives", attempts);
    self_test_print("wifi", "adresse ip", WiFi.localIP().toString().c_str());
    self_test_print("wifi", "rssi", rssi);
    return true;
}
