#pragma once
#include <Arduino.h>

constexpr uint32_t SERIAL_BAUD = 460800;


// Résolution du JPEG : copier une des constantes ci-dessous.
// #define et non constexpr : la valeur vient de la bibliothèque ArduCAM, incluse seulement dans camera.cpp.
// OV5642_320x240, OV5642_640x480, OV5642_1024x768, OV5642_1280x960, OV5642_1600x1200, OV5642_2048x1536, OV5642_2592x1944
// OV5642_1920x1080 existe dans la bibliothèque mais n'y est pas implémentée : ne pas l'utiliser.
#define CAMERA_RESOLUTION OV5642_1024x768

constexpr uint32_t CAMERA_SPI_FREQUENCY_HZ = 4000000;

constexpr size_t CAMERA_READ_BLOCK_BYTES = 512;