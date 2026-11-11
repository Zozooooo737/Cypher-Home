#pragma once

// Copier ce fichier en secrets.h (ignoré par Git) et remplacer les valeurs factices.

// Wi-Fi : partage de connexion iPhone (2,4 GHz)
constexpr char WIFI_SSID[] = "MonReseau";
constexpr char WIFI_PASSWORD[] = "MonMotDePasse";

// MQTT : IP du PC qui fait tourner le broker, sur le Wi-Fi de l'iPhone (change à chaque session)
constexpr char MQTT_BROKER_IP[] = "172.20.10.3";

// MQTT : identifiants de la carte auprès du broker (utilisateur créé avec mosquitto_passwd)
constexpr char MQTT_USERNAME[] = "cypherhome-board";
constexpr char MQTT_PASSWORD[] = "MonMotDePasseMqtt";

// HTTP : IP du PC qui fait tourner le serveur de réception (même PC que le broker ; change à chaque session)
constexpr char HTTP_SERVER_IP[] = "172.20.10.3";
