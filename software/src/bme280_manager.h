#pragma once
#include <Arduino.h>

struct BmeStatus {
    bool present;
    float temperature;   // °C
    float humidity;      // %
    float pressure;      // hPa
};

void bme_init();
void bme_loop();
BmeStatus bme_getStatus();
void scanI2C();