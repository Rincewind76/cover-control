/**
 * @file config_manager.cpp
 * @brief Implementation of the configuration parser
 *
 * Supported entries in config.txt:
 * - wifi=SSID;PASS
 * - ota_password=...
 * - led_brightness=0..255
 * - led_brightness_dark=0..255
 * - autoclose_cover=0|1
 * - autoclose_time=HH:MM
 * - dew1_level=0..100 Percent PWM level for dew heater 1
 * - dew2_level=0..100 Percent PWM level for dew heater 2
 */

#include "config_manager.h"
#include "sdcard.h"
#include "web_log.h"

// --------------------
// Global configuration values
// --------------------

WifiEntry wifiList[10];
int wifiCount = 0;

String otaPassword = "update123";

uint8_t ledBrightnessNormal = 80;
uint8_t ledBrightnessDark   = 20;

bool autoCloseCover = false; // autoclose disabled by default
int autoCloseHour = 5; // autoclose default time 05:00
int autoCloseMinute = 0;

int dew1Level = 70; // default PWM to 70%
int dew2Level = 70;

// --------------------

bool loadConfigFromSD() {
    File file = openConfigFile();
    if (!file) {
        LOG("No config.txt found");
        return false;
    }

    wifiCount = 0;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();

        // Ignore comments and empty lines
        if (line.length() == 0 || line.startsWith("#")) {
            continue;
        }

        // WiFi
        if (line.startsWith("wifi=")) {
            if (wifiCount >= 10) continue;

            line.remove(0, 5);
            int sep = line.indexOf(';');
            if (sep < 0) continue;

            wifiList[wifiCount].ssid = line.substring(0, sep);
            wifiList[wifiCount].password = line.substring(sep + 1);
            wifiList[wifiCount].ssid.trim();
            wifiList[wifiCount].password.trim();

            wifiCount++;
            continue;
        }

        // OTA password
        if (line.startsWith("ota_password=")) {
            otaPassword = line.substring(strlen("ota_password="));
            otaPassword.trim();
            continue;
        }

        // LED normal brightness
        if (line.startsWith("led_brightness=")) {
            ledBrightnessNormal = constrain(
                line.substring(strlen("led_brightness=")).toInt(), 0, 255);
            continue;
        }

        // LED dark mode brightness
        if (line.startsWith("led_brightness_dark=")) {
            ledBrightnessDark = constrain(
                line.substring(strlen("led_brightness_dark=")).toInt(), 0, 255);
            continue;
        }

        // Enable/disable AutoClose
        if (line.startsWith("autoclose_cover=")) {
            String val = line.substring(strlen("autoclose_cover="));
            val.trim();
            autoCloseCover = (val == "1" || val.equalsIgnoreCase("true"));
            continue;
        }

        // AutoClose time (HH:MM)
        if (line.startsWith("autoclose_time=")) {
            String val = line.substring(strlen("autoclose_time="));
            val.trim();

            int sep = val.indexOf(':');
            if (sep > 0) {
                autoCloseHour   = val.substring(0, sep).toInt();
                autoCloseMinute = val.substring(sep + 1).toInt();

                // Safety: limit values
                autoCloseHour   = constrain(autoCloseHour,   0, 23);
                autoCloseMinute = constrain(autoCloseMinute, 0, 59);
            }
            continue;
        }
        
        // Dew Heater 1 Level (0-100%)
        if (line.startsWith("dew1_level=")) {
            dew1Level = constrain(
                line.substring(strlen("dew1_level=")).toInt(), 0, 100
            );
            continue;
        }

        // Dew Heater 2 Level (0-100%)
        if (line.startsWith("dew2_level=")) {
            dew2Level = constrain(
                line.substring(strlen("dew2_level=")).toInt(), 0, 100
            );
            continue;
        }

    }

    file.close();
    LOG("Config loaded");
    return true;
}
