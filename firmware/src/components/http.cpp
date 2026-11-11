#include <Arduino.h>
#include <WiFiS3.h>

#include "http.h"
#include "wifi.h"
#include "config.h"
#include "secrets.h"
#include "self_test.h"

constexpr uint32_t TEST_SIZES_BYTES[] = {16384, 32768, 65536, 131072};

constexpr int HTTP_ERROR_CONNECT = -1;
constexpr int HTTP_ERROR_SEND = -2;
constexpr int HTTP_ERROR_RESPONSE = -3;

static WiFiClient s_client;
static uint8_t s_block[HTTP_BLOCK_BYTES];
static uint32_t s_pattern_index = 0;

void http_setup() {
    s_client.setConnectionTimeout(HTTP_CONNECT_TIMEOUT_MS);
    s_client.setTimeout(HTTP_RESPONSE_TIMEOUT_MS);
}

static int http_post(uint32_t length, size_t (*read_block)(uint8_t *buf, size_t max_len)) {
    if (!wifi_ensure_connected() || !s_client.connect(HTTP_SERVER_IP, HTTP_SERVER_PORT)) {
        return HTTP_ERROR_CONNECT;
    }

    char header[128];
    size_t header_length = snprintf(header, sizeof(header),
                                    "POST %s HTTP/1.1\r\nHost: %s:%u\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n",
                                    HTTP_UPLOAD_PATH, HTTP_SERVER_IP, HTTP_SERVER_PORT, static_cast<unsigned long>(length));
    if (s_client.write(reinterpret_cast<uint8_t *>(header), header_length) != header_length) {
        return HTTP_ERROR_SEND;
    }

    uint32_t sent = 0;
    while (sent < length) {
        size_t n = read_block(s_block, min(HTTP_BLOCK_BYTES, static_cast<size_t>(length - sent)));
        if (n == 0 || s_client.write(s_block, n) != n) {
            return HTTP_ERROR_SEND;
        }
        sent += n;
    }

    if (!s_client.find("HTTP/1.") || !s_client.find(" ")) {
        return HTTP_ERROR_RESPONSE;
    }
    return s_client.parseInt();
}

bool http_upload(uint32_t length, size_t (*read_block)(uint8_t *buf, size_t max_len), int &status) {
    status = http_post(length, read_block);
    s_client.stop();
    return status == 200;
}

static size_t http_pattern_block(uint8_t *buf, size_t max_len) {
    for (size_t i = 0; i < max_len; i++, s_pattern_index++) {
        buf[i] = ((s_pattern_index * 31 + 7) ^ (s_pattern_index >> 8)) & 0xFF;
    }
    return max_len;
}

bool http_test() {
    for (uint32_t size : TEST_SIZES_BYTES) {
        s_pattern_index = 0;
        int status;
        unsigned long start_ms = millis();
        bool ok = http_upload(size, http_pattern_block, status);
        unsigned long duration_ms = millis() - start_ms;

        char code[8];
        snprintf(code, sizeof(code), "%d", status);
        if (!ok) {
            Serial.println("[http] KO");
            self_test_print("http", "code erreur", code);
            return false;
        }

        if (size == TEST_SIZES_BYTES[0]) {
            char server[24];
            snprintf(server, sizeof(server), "%s:%u", HTTP_SERVER_IP, HTTP_SERVER_PORT);
            self_test_print("http", "serveur", server);
        }
        self_test_print("http", "taille", size, "octets");
        self_test_print("http", "code http", code);
        self_test_print("http", "duree", duration_ms, "ms");
        self_test_print("http", "debit", size / 1024.0f / (duration_ms / 1000.0f), "ko/s");
    }
    return true;
}
