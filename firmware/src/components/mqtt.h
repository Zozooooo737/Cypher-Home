#pragma once

// Se connecte au broker (IP de secrets.h, port et identifiant de config.h) ; false si échec
bool mqtt_setup();

// Publie un message en QoS 0, en se reconnectant d'abord si besoin ; false si échec
bool mqtt_publish(const char *topic, const char *payload);

// Test : publie sur cypherhome/test et affiche broker, durée de connexion, sujet et message, ou « [mqtt] KO »
bool mqtt_test();
