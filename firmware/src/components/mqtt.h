#pragma once

// Ouvre une connexion neuve vers le broker (IP de secrets.h, port de config.h) ; false si échec
bool mqtt_setup();

// Test : adresse du broker et durée de la connexion, ou « [mqtt] KO »
bool mqtt_test();
