#include "bme280_manager.h"
#include "web_log.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include "pins.h"

#define BME_ADDR 0x76

static Adafruit_BME280 bme;
static BmeStatus status;
static unsigned long lastRead = 0;

void bme_init() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    scanI2C();
    
    if (!bme.begin(BME_ADDR)) {
        LOG("BME280 not found");
        status.present = false;
        return;
    }

    status.present = true;
    LOG("BME280 initialized");
}

void bme_loop() {
    if (!status.present) return;

    if (millis() - lastRead < 2000) return; // 2s Intervall
    lastRead = millis();

    status.temperature = bme.readTemperature();
    status.humidity    = bme.readHumidity();
    status.pressure    = bme.readPressure() / 100.0f;

    LOGF("BME280 T=%.1fC H=%.1f%% P=%.1fhPa",
         status.temperature,
         status.humidity,
         status.pressure);
}

BmeStatus bme_getStatus() {
    return status; // Kopie
}

#include <Wire.h>

void scanI2C() {
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            LOG("I2C device at 0x" + String(addr, HEX));
        }
    }
}
