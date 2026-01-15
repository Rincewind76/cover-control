#pragma once
#include <Arduino.h>

/**
 * @brief Status der Dew-Heater-Regelung
 */
struct DewStatus {
    float temperature;   ///< °C
    float humidity;      ///< %
    float dewPoint;      ///< °C
    int dew1Power;       ///< %
    int dew2Power;       ///< %
    bool active;         ///< true = Heizung aktiv
    unsigned long lastUpdateMs;
};

/**
 * @brief Initialisiert die Dew-Regelung.
 *        Der BME280 muss vorher über bme280_manager initiiert worden sein.
 * @return true wenn Sensor verfügbar
 */
bool dew_init();

/**
 * @brief Muss zyklisch aufgerufen werden (z.B. alle 1s)
 *        Führt Messung, Taupunktberechnung und PWM-Regelung aus.
 */
void dew_update();

/**
 * @brief Liefert den aktuellen Status der Dew-Heater-Regelung
 */
DewStatus dew_getStatus();
