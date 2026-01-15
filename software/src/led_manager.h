#pragma once

#include <Arduino.h>

/**
 * @file led_manager.h
 * @brief Central LED control for status indicators (WiFi, System, Light)
 *
 * This module encapsulates all LED logic:
 *  - PWM brightness via LEDC
 *  - Blink modes (slow / fast)
 *  - State-based control (no delay!)
 *
 * The LEDs only respond to system states,
 * not to time-critical logic in setup().
 */

/**
 * @enum LedId
 * @brief Logical LED IDs
 */
enum LedId {
    LED_WLAN = 0,
    LED_STATUS,
    LED_LIGHT,
    LED_COUNT
};

/**
 * @enum LedMode
 * @brief Operating modes of an LED
 */
enum LedMode {
    LED_MODE_OFF,         ///< LED off
    LED_MODE_ON,          ///< LED always on
    LED_MODE_BLINK_SLOW,  ///< slow blinking (e.g. WiFi connecting)
    LED_MODE_BLINK_FAST   ///< fast blinking (e.g. OTA active)
};

/**
 * @brief Initializes all LED pins and LEDC channels
 *
 * Must be called once in setup(),
 * after initPins() has been executed.
 */
void initLeds();

/**
 * @brief Sets the operating mode of an LED
 *
 * @param led   Logical LED ID (LedId)
 * @param mode  Desired LED mode (LedMode)
 */
void setLedMode(LedId led, LedMode mode);

/**
 * @brief Sets the maximum brightness of all LEDs (global)
 *
 * @param brightness PWM value 0–255
 */
void setGlobalLedBrightness(uint8_t brightness);

/**
 * @brief Updates all LEDs (blink logic, PWM)
 *
 * Must be called cyclically in loop().
 * This function is completely non-blocking.
 */
void updateLeds();

/**
 * @brief Returns the current mode of an LED
 *
 * @param led Logical LED ID (LedId)
 * @return Current LED mode (LedMode)
 */
LedMode getLedMode(LedId led);
