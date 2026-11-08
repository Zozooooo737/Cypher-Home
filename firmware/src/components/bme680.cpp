#include <Arduino.h>
#include <Adafruit_BME680.h>

#include "bme680.h"
#include "config.h"
#include "self_test.h"

constexpr uint8_t BME680_TEMPERATURE_OVERSAMPLING = BME68X_OS_8X;
constexpr uint8_t BME680_HUMIDITY_OVERSAMPLING = BME68X_OS_2X;
constexpr uint8_t BME680_PRESSURE_OVERSAMPLING = BME68X_OS_4X;
constexpr uint8_t BME680_IIR_FILTER_SIZE = BME68X_FILTER_SIZE_3;

constexpr uint16_t BME680_HEATER_TEMPERATURE_C = 320;
constexpr uint16_t BME680_HEATER_DURATION_MS = 150;

static Adafruit_BME680 s_bme;
static bool s_ready = false;

bool bme680_setup() {
    if (s_ready) {
        return true;
    }

    if (!s_bme.begin(BME680_I2C_ADDRESS)) {
        return false;
    }

    s_ready = s_bme.setTemperatureOversampling(BME680_TEMPERATURE_OVERSAMPLING)
              && s_bme.setHumidityOversampling(BME680_HUMIDITY_OVERSAMPLING)
              && s_bme.setPressureOversampling(BME680_PRESSURE_OVERSAMPLING)
              && s_bme.setIIRFilterSize(BME680_IIR_FILTER_SIZE)
              && s_bme.setGasHeater(BME680_HEATER_TEMPERATURE_C, BME680_HEATER_DURATION_MS);
    return s_ready;
}

bool bme680_read(Bme680Data &data) {
    data.temperature_c = NAN;
    data.humidity_pct = NAN;
    data.pressure_hpa = NAN;
    data.gas_kohm = NAN;

    if (!s_ready && !bme680_setup()) {
        return false;
    }

    s_bme.temperature = NAN;
    s_bme.pressure = 0;
    s_bme.gas_resistance = 0;

    if (!s_bme.performReading()) {
        s_ready = false;
        return false;
    }
    if (isnan(s_bme.temperature) || s_bme.pressure == 0) {
        return false;
    }

    data.temperature_c = s_bme.temperature;
    data.humidity_pct = s_bme.humidity;
    data.pressure_hpa = static_cast<float>(s_bme.pressure) / 100.0f;

    if (s_bme.gas_resistance == 0) {
        return false;
    }
    data.gas_kohm = static_cast<float>(s_bme.gas_resistance) / 1000.0f;
    return true;
}

bool bme680_test() {
    Bme680Data data;
    if (!bme680_read(data)) {
        Serial.println("[bme680] KO");
        return false;
    }

    self_test_print("bme680", "temperature", data.temperature_c, "C");
    self_test_print("bme680", "humidite", data.humidity_pct, "%");
    self_test_print("bme680", "pression", data.pressure_hpa, "hPa");
    self_test_print("bme680", "gaz", data.gas_kohm, "kOhm");
    return true;
}
