/**
 * @file config_manager.h
 * @brief Central configuration management
 *
 * Reads all configuration values from config.txt (SD card)
 * and makes them available globally.
 *
 * Does NOT contain logic for WiFi, OTA, or LEDs.
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

/**
 * @brief WiFi credentials
 */
struct WifiEntry {
    String ssid;
    String password;
};

extern WifiEntry wifiList[10];
extern int wifiCount;

/**
 * @brief OTA password (plain text from config.txt)
 */
extern String otaPassword;

/**
 * @brief LED brightness for normal and dark mode
 */
extern uint8_t ledBrightnessNormal;
extern uint8_t ledBrightnessDark;

/**
 * @brief Auto Close Cover at a defined time
 */
extern bool autoCloseCover;     // true/false
extern int autoCloseHour;       // 0–23
extern int autoCloseMinute;     // 0–59
    
/**
 * @brief PWM default values for DEW1/2
 */
extern int dew1Level;
extern int dew2Level;

/**
 * @brief Loads config.txt from SD card
 * @return true if successful
 */
bool loadConfigFromSD();


#endif
