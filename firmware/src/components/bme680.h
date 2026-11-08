#pragma once

// Une mesure complète du BME680 ; NAN pour une valeur non lue
struct Bme680Data {
    float temperature_c;
    float humidity_pct;
    float pressure_hpa;
    float gas_kohm;
};

// Met en place le bus I2C et le capteur ; false s'il ne répond pas
bool bme680_setup();

// Lance une mesure complète (bloquante, chauffage du gaz compris) et remplit data ; false si une valeur manque
bool bme680_read(Bme680Data &data);

// Test : une mesure, les 4 valeurs sur le port série, ou « [bme680] KO »
bool bme680_test();
