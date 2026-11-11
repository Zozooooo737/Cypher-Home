#include <Arduino.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>

#include "mqtt.h"
#include "config.h"
#include "secrets.h"
#include "self_test.h"

constexpr char TEST_TOPIC[] = "cypherhome/test";

static WiFiClient s_wifi_client;
static MqttClient s_mqtt_client(s_wifi_client);

static unsigned long s_connect_ms = 0;

bool mqtt_setup() {
    unsigned long start_ms = millis();
    s_mqtt_client.setId(MQTT_CLIENT_ID);
    s_mqtt_client.setUsernamePassword(MQTT_USERNAME, MQTT_PASSWORD);
    s_mqtt_client.setConnectionTimeout(MQTT_CONNECT_TIMEOUT_MS);
    if (!s_mqtt_client.connect(MQTT_BROKER_IP, MQTT_BROKER_PORT)) {
        return false;
    }
    s_connect_ms = millis() - start_ms;
    return true;
}

static bool mqtt_ensure_connected() {
    s_mqtt_client.poll();
    return s_mqtt_client.connected() || mqtt_setup();
}

bool mqtt_publish(const char *topic, const char *payload) {
    if (!mqtt_ensure_connected()) {
        return false;
    }
    s_mqtt_client.beginMessage(topic);
    s_mqtt_client.print(payload);
    return s_mqtt_client.endMessage();
}

bool mqtt_test() {
    char payload[40];
    snprintf(payload, sizeof(payload), "bonjour de la carte, %lu s", millis() / 1000);
    if (!mqtt_publish(TEST_TOPIC, payload)) {
        Serial.println("[mqtt] KO");
        char code[8];
        snprintf(code, sizeof(code), "%d", s_mqtt_client.connectError());
        self_test_print("mqtt", "code erreur", code);
        return false;
    }

    char broker[24];
    snprintf(broker, sizeof(broker), "%s:%u", MQTT_BROKER_IP, MQTT_BROKER_PORT);
    self_test_print("mqtt", "broker", broker);
    self_test_print("mqtt", "connexion", s_connect_ms, "ms");
    self_test_print("mqtt", "sujet", TEST_TOPIC);
    self_test_print("mqtt", "message", payload);
    return true;
}
