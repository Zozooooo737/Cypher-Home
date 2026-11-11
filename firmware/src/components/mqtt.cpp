#include <Arduino.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>

#include "mqtt.h"
#include "config.h"
#include "secrets.h"
#include "self_test.h"

// Sujet réservé au test
constexpr char TEST_TOPIC[] = "cypherhome/test";

// Connexion TCP (WiFiS3), et client MQTT qui parle à travers elle
static WiFiClient s_wifi_client;
static MqttClient s_mqtt_client(s_wifi_client);

// Durée de la dernière connexion réussie (TCP + CONNECT/CONNACK)
static unsigned long s_connect_ms = 0;

// connect() ouvre une connexion TCP neuve, envoie CONNECT et attend CONNACK
// au plus MQTT_CONNECT_TIMEOUT_MS ; il ferme d'abord l'éventuelle connexion précédente.
bool mqtt_setup() {
    unsigned long start_ms = millis();
    s_mqtt_client.setId(MQTT_CLIENT_ID);
    s_mqtt_client.setConnectionTimeout(MQTT_CONNECT_TIMEOUT_MS);
    if (!s_mqtt_client.connect(MQTT_BROKER_IP, MQTT_BROKER_PORT)) {
        return false;
    }
    s_connect_ms = millis() - start_ms;
    return true;
}

// Reconnexion automatique : poll() lit ce que le broker a envoyé et entretient le keep alive ;
// si la connexion est perdue, on la rouvre.
static bool mqtt_ensure_connected() {
    s_mqtt_client.poll();
    return s_mqtt_client.connected() || mqtt_setup();
}

// Message = en-tête PUBLISH (sujet, QoS 0 par défaut) + contenu, envoyé par endMessage()
bool mqtt_publish(const char *topic, const char *payload) {
    if (!mqtt_ensure_connected()) {
        return false;
    }
    s_mqtt_client.beginMessage(topic);
    s_mqtt_client.print(payload);
    return s_mqtt_client.endMessage();
}

// Publie un message daté (secondes depuis le démarrage) pour reconnaître chaque passe ; échec = une seule ligne KO
bool mqtt_test() {
    char payload[40];
    snprintf(payload, sizeof(payload), "bonjour de la carte, %lu s", millis() / 1000);
    if (!mqtt_publish(TEST_TOPIC, payload)) {
        Serial.println("[mqtt] KO");
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
