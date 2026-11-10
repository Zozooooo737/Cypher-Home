#pragma once

// Se connecte au réseau de secrets.h (WIFI_CONNECT_ATTEMPTS tentatives au plus) ; false si échec
bool wifi_setup();

// Indique si la carte est connectée au réseau
bool wifi_connected();

// Relance la connexion si la liaison est perdue ; false si elle n'a pas pu être rétablie
bool wifi_ensure_connected();

// Test : version du firmware Wi-Fi, SSID, durée et tentatives de connexion, IP, RSSI, ou « [wifi] KO »
bool wifi_test();
