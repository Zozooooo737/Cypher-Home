#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <ArduCAM.h>

#include "camera.h"
#include "config.h"
#include "pins.h"

static_assert(CAMERA_RESOLUTION <= OV5642_2592x1944, "CAMERA_RESOLUTION : valeur non disponible, voir la liste dans config.h");

// Largeur et hauteur : l'ordre doit suivre les constantes OV5642_* de ArduCAM.h (0 à 6)
constexpr uint16_t CAMERA_SIZES[][2] = {{320, 240}, {640, 480}, {1024, 768}, {1280, 960}, {1600, 1200}, {2048, 1536}, {2592, 1944}};
constexpr uint16_t CAMERA_WIDTH_PX = CAMERA_SIZES[CAMERA_RESOLUTION][0];
constexpr uint16_t CAMERA_HEIGHT_PX = CAMERA_SIZES[CAMERA_RESOLUTION][1];

constexpr unsigned long CAMERA_CAPTURE_TIMEOUT_MS = 10000;

constexpr unsigned long CAMERA_SETTLE_MS = 1000;

constexpr uint8_t CAMERA_CPLD_RESET_REG = 0x07;
constexpr unsigned long CAMERA_RESET_MS = 100;

constexpr uint8_t CAMERA_SENSOR_ADDRESS = 0x3C;
constexpr uint8_t CAMERA_SENSOR_ID_HIGH = 0x56;
constexpr uint8_t CAMERA_SENSOR_ID_LOW = 0x42;

static ArduCAM s_camera(OV5642, CAMERA_CS_PIN);
static const SPISettings s_spi_settings(CAMERA_SPI_FREQUENCY_HZ, MSBFIRST, SPI_MODE0);
static bool s_buses_started = false;
static bool s_ready = false;
static uint32_t s_captured_length = 0;
static uint32_t s_read_remaining = 0;
static const char *s_error = "";

static bool camera_fail(const char *error) {
    s_error = error;
    return false;
}

static void camera_write_reg(uint8_t address, uint8_t value) {
    SPI.beginTransaction(s_spi_settings);
    s_camera.write_reg(address, value);
    SPI.endTransaction();
}

static uint8_t camera_read_reg(uint8_t address) {
    SPI.beginTransaction(s_spi_settings);
    uint8_t value = s_camera.read_reg(address);
    SPI.endTransaction();
    return value;
}

static bool camera_sensor_read(uint16_t reg, uint8_t &value) {
    Wire.beginTransmission(CAMERA_SENSOR_ADDRESS);
    Wire.write(static_cast<uint8_t>(reg >> 8));
    Wire.write(static_cast<uint8_t>(reg & 0xFF));
    if (Wire.endTransmission() != 0 || Wire.requestFrom(CAMERA_SENSOR_ADDRESS, static_cast<size_t>(1)) != 1) {
        return false;
    }
    value = static_cast<uint8_t>(Wire.read());
    return true;
}

bool camera_setup() {
    if (s_ready) {
        return true;
    }

    pinMode(CAMERA_CS_PIN, OUTPUT);
    digitalWrite(CAMERA_CS_PIN, HIGH);
    if (!s_buses_started) {
        Wire.begin();
        SPI.begin();
        s_buses_started = true;
    }

    camera_write_reg(CAMERA_CPLD_RESET_REG, 0x80);
    delay(CAMERA_RESET_MS);
    camera_write_reg(CAMERA_CPLD_RESET_REG, 0x00);
    delay(CAMERA_RESET_MS);

    camera_write_reg(ARDUCHIP_TEST1, 0x55);
    if (camera_read_reg(ARDUCHIP_TEST1) != 0x55) {
        return camera_fail("bus SPI : registre de test mal relu (verifier CS, SCK, MOSI, MISO)");
    }

    uint8_t id_high = 0;
    uint8_t id_low = 0;
    if (!camera_sensor_read(0x300A, id_high) || !camera_sensor_read(0x300B, id_low)) {
        return camera_fail("bus I2C : le capteur ne repond pas (verifier SDA, SCL)");
    }
    if (id_high != CAMERA_SENSOR_ID_HIGH || id_low != CAMERA_SENSOR_ID_LOW) {
        return camera_fail("identifiant du capteur inattendu (OV5642 attendu)");
    }

    SPI.beginTransaction(s_spi_settings);
    s_camera.set_format(JPEG);
    s_camera.InitCAM();
    s_camera.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
    s_camera.OV5642_set_JPEG_size(CAMERA_RESOLUTION);
    SPI.endTransaction();
    delay(CAMERA_SETTLE_MS);

    SPI.beginTransaction(s_spi_settings);
    s_camera.clear_fifo_flag();
    s_camera.write_reg(ARDUCHIP_FRAMES, 0x00);
    SPI.endTransaction();

    s_error = "";
    s_ready = true;
    return true;
}

bool camera_is_ready() {
    return s_ready;
}

const char *camera_last_error() {
    return s_error;
}

bool camera_capture(uint32_t &jpeg_length) {
    jpeg_length = 0;
    s_captured_length = 0;
    if (!s_ready) {
        return camera_fail("camera non prete");
    }

    SPI.beginTransaction(s_spi_settings);
    s_camera.flush_fifo();
    s_camera.clear_fifo_flag();
    s_camera.start_capture();
    SPI.endTransaction();

    unsigned long start_ms = millis();
    while ((camera_read_reg(ARDUCHIP_TRIG) & CAP_DONE_MASK) == 0) {
        if (millis() - start_ms > CAMERA_CAPTURE_TIMEOUT_MS) {
            camera_write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
            s_ready = false;
            return camera_fail("capture : delai maximal depasse");
        }
    }

    SPI.beginTransaction(s_spi_settings);
    uint32_t length = s_camera.read_fifo_length();
    SPI.endTransaction();
    if (length == 0 || length >= MAX_FIFO_SIZE) {
        camera_write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
        return camera_fail("capture : longueur invalide");
    }

    s_captured_length = length;
    jpeg_length = length;
    return true;
}

void camera_read_begin() {
    s_read_remaining = s_captured_length;
    if (s_read_remaining == 0) {
        return;
    }
    SPI.beginTransaction(s_spi_settings);
    s_camera.CS_LOW();
    s_camera.set_fifo_burst();
}

size_t camera_read_block(uint8_t *buf, size_t max_len) {
    size_t count = (s_read_remaining < max_len) ? s_read_remaining : max_len;
    if (count == 0) {
        return 0;
    }
    memset(buf, 0, count);
    SPI.transfer(buf, count);
    s_read_remaining -= count;
    return count;
}

void camera_read_end() {
    if (s_captured_length == 0) {
        return;
    }
    s_camera.CS_HIGH();
    s_camera.clear_fifo_flag();
    SPI.endTransaction();
    s_captured_length = 0;
    s_read_remaining = 0;
}

bool camera_test() {
    char line[160];

    if (!s_ready && !camera_setup()) {
        Serial.print("[camera] KO : ");
        Serial.println(s_error);
        return false;
    }

    uint32_t length = 0;
    unsigned long start_ms = millis();
    bool captured = camera_capture(length);
    unsigned long capture_ms = millis() - start_ms;
    if (!captured) {
        Serial.print("[camera] KO : ");
        Serial.println(s_error);
        return false;
    }

    snprintf(line, sizeof(line), "IMG_BEGIN %lu %ux%u\n", static_cast<unsigned long>(length),
             CAMERA_WIDTH_PX, CAMERA_HEIGHT_PX);
    Serial.print(line);

    // Envoi par blocs ; flush() évite d'écraser le dernier octet d'un bloc.
    // Le premier bloc doit commencer par FF D8 (début de JPEG) ; l'image est envoyée en entier dans tous les cas.
    static uint8_t block[CAMERA_READ_BLOCK_BYTES];
    bool first_block = true;
    bool soi_ok = false;
    size_t count = 0;
    start_ms = millis();
    camera_read_begin();
    while ((count = camera_read_block(block, sizeof(block))) > 0) {
        if (first_block) {
            soi_ok = (count >= 2 && block[0] == 0xFF && block[1] == 0xD8);
            first_block = false;
        }
        Serial.write(block, count);
        Serial.flush();
    }
    camera_read_end();
    unsigned long transfer_ms = millis() - start_ms;

    snprintf(line, sizeof(line), "IMG_END %lu\n", transfer_ms);
    Serial.print(line);

    if (!soi_ok) {
        Serial.println("[camera] KO : le tampon ne commence pas par un JPEG (FF D8 absent)");
        return false;
    }

    snprintf(line, sizeof(line), "[camera] resolution %ux%u | format JPEG | capture %lu ms",
             CAMERA_WIDTH_PX, CAMERA_HEIGHT_PX, capture_ms);
    Serial.println(line);
    return true;
}
