#pragma once

#include <Arduino.h>
#include "pins.h"

// --- PWM configuration (must match your .cpp setup) ---
#ifndef PWM_FREQUENCY
#define PWM_FREQUENCY 20000   // 20 kHz recommended for heaters
#endif

#ifndef PWM_RESOLUTION
#define PWM_RESOLUTION 10     // 10-bit resolution (0..1023)
#endif

// PWM channels (must match your .cpp)
#ifndef PWM_CHANNEL_DEW1
#define PWM_CHANNEL_DEW1 5
#endif

#ifndef PWM_CHANNEL_DEW2
#define PWM_CHANNEL_DEW2 6
#endif

/**
 * @brief Initializes all power-related GPIOs and PWM channels.
 *
 * - Configures VIN sense pin
 * - Initializes PWM for Dew Heater 1 and 2
 * - Initializes Flat Panel 12V output
 */
void power_init();

/**
 * @brief Reads the 12V supply voltage using the analog input.
 *
 * Uses the voltage divider 100k / 22k.
 *
 * @return float Voltage in volts.
 */
float power_readSupplyVoltage();

/**
 * @brief Sets the PWM level for Dew Heater 1.
 *
 * @param percent 0..100 (%)
 */
void power_setDew1(int percent);

/**
 * @brief Sets the PWM level for Dew Heater 2.
 *
 * @param percent 0..100 (%)
 */
void power_setDew2(int percent);

/**
 * @brief Returns the current PWM level of Dew Heater 1.
 *
 * @return int 0..100 (%)
 */
int power_getDew1Level();

/**
 * @brief Returns the current PWM level of Dew Heater 2.
 *
 * @return int 0..100 (%)
 */
int power_getDew2Level();

/**
 * @brief Enables or disables the flat panel 12V output.
 *
 * @param on true = enable, false = disable
 */
void power_setPanel(bool on);

/**
 * @brief Returns the current state of the flat panel 12V output.
 *
 * @return true if enabled
 */
bool power_isPanelOn();
