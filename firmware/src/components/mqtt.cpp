#include <Arduino.h>
#include <WiFiS3.h>

#include "mqtt.h"
#include "config.h"
#include "secrets.h"
#include "self_test.h"

// Connexion TCP vers le broker, gardée d'un appel à l'autre
static WiFiClient s_client;

// Ferme l'éventuelle connexion précédente, puis en ouvre une neuve.
// Sans stop(), une connexion restée ouverte cacherait un pare-feu refermé entre-temps.
bool mqtt_setup() {
    s_client.stop();
    return s_client.connect(MQTT_BROKER_IP, MQTT_BROKER_PORT);
}

// Chronomètre une connexion neuve ; échec = une seule ligne KO
bool mqtt_test() {
    unsigned long start_ms = millis();
    if (!mqtt_setup()) {
        Serial.println("[mqtt] KO");
        return false;
    }
    unsigned long connect_ms = millis() - start_ms;

    // Adresse affichée sous la forme IP:port
    char broker[24];
    snprintf(broker, sizeof(broker), "%s:%u", MQTT_BROKER_IP, MQTT_BROKER_PORT);
    self_test_print("mqtt", "broker", broker);
    self_test_print("mqtt", "connexion tcp", connect_ms, "ms");
    return true;
}
