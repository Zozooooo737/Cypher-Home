#include <Arduino.h>
#include <WiFiS3.h>

#include "http.h"
#include "wifi.h"
#include "config.h"
#include "secrets.h"

constexpr int HTTP_ERROR_CONNECT = -1;
constexpr int HTTP_ERROR_SEND = -2;
constexpr int HTTP_ERROR_RESPONSE = -3;

static WiFiClient s_client;
static uint8_t s_block[HTTP_BLOCK_BYTES];

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
