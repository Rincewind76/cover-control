#include "dew_controller.h"
#include "bme280_manager.h"   // liefert bme_getTemperature(), bme_getHumidity(), bme_isAvailable()
#include "power_control.h"
#include "config_manager.h"    // dew1Level / dew2Level
#include "web_log.h"
#include <math.h>

// Update-Intervall in Millisekunden
#define DEW_UPDATE_INTERVAL 5000
#define DEW_FULL_ON_DELTA 2.0f  // ΔT 100% PWM erreicht wird
#define DEW_START_DELTA    4.0f   // ab 3°C Abstand anfangen

// ---------------- Status ----------------
static DewStatus status = {
    .temperature = 0,
    .humidity = 0,
    .dewPoint = 0,
    .dew1Power = 0,
    .dew2Power = 0,
    .active = false,
    .lastUpdateMs = 0
};

// ---------------- Taupunkt-Berechnung ----------------
static float calculateDewPoint(float tempC, float humidity)
{
    const float a = 17.62f;
    const float b = 243.12f;

    if (humidity <= 0.0f || humidity > 100.0f) return NAN;

    float gamma = (a * tempC) / (b + tempC) + log(humidity / 100.0f);
    return (b * gamma) / (a - gamma);
}

// ---------------- Init ----------------
bool dew_init()
{
    BmeStatus bme = bme_getStatus();
    if (!bme.present) {
        LOG("DewCtrl: BME280 not available");
        power_setDew1(0);
        power_setDew2(0);
        return false;
    }

    LOG("DewCtrl: using BME280 manager");
    return true;
}

// ---------------- Update ----------------
void dew_update()
{
    unsigned long now = millis();
    if (now - status.lastUpdateMs < DEW_UPDATE_INTERVAL) return;
    status.lastUpdateMs = now;
    BmeStatus bme = bme_getStatus();

    if (!bme.present) {
        power_setDew1(0);
        power_setDew2(0);
        status.active = false;
        return;
    }

    float t = bme.temperature;
    float h = bme.humidity;

    if (isnan(t) || isnan(h) || h <= 0.0f) {
        power_setDew1(0);
        power_setDew2(0);
        status.active = false;
        LOG("DewCtrl: Invalid BME reading");
        return;
    }

    float td = calculateDewPoint(t, h);
    float delta = t - td;  // Temperatur nähert sich Taupunkt

    int max1 = constrain(dew1Level, 0, 100);
    int max2 = constrain(dew2Level, 0, 100);

    int p1 = 0;
    int p2 = 0;

    if (delta < DEW_START_DELTA) {
        float factor = (DEW_START_DELTA - delta) / DEW_FULL_ON_DELTA;
        p1 = constrain(factor * max1, 0, max1);
        p2 = constrain(factor *  max2, 0, max2);
    }

    power_setDew1(p1);
    power_setDew2(p2);

    status.temperature = t;
    status.humidity = h;
    status.dewPoint = td;
    status.dew1Power = p1;
    status.dew2Power = p2;
    status.active = (p1 > 0 || p2 > 0);

    LOGF(
        "DewCtrl: T=%.1fC RH=%.1f%% Td=%.1fC Δ=%.2f → D1=%d%% D2=%d%%",
        t, h, td, delta, p1, p2
    );
}

// ---------------- Status Getter ----------------
DewStatus dew_getStatus()
{
    return status;
}
