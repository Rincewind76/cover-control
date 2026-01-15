/**
 * @file ota.cpp
 * @brief ArduinoOTA integration
 *
 * OTA is only started if WiFi is correctly initialized.
 */

#include "ota.h"
#include "config_manager.h"
#include <ArduinoOTA.h>
#include <WiFi.h>
#include "web_log.h"

static bool otaStarted = false;

/**
 * @brief Initializes OTA update functionality.
 *
 * Sets the OTA hostname and password, and starts the ArduinoOTA service.
 * OTA will only be started if WiFi is initialized in station mode.
 */
void initOTA() {
    if (WiFi.getMode() != WIFI_STA) {
        LOG("OTA skipped: WiFi not ready");
        return;
    }

    ArduinoOTA.setHostname("cover-controller");
    ArduinoOTA.setPassword(otaPassword.c_str());
    ArduinoOTA.begin();

    LOG("OTA ready");
}

/**
 * @brief Handles OTA update requests.
 *
 * Starts the OTA service if WiFi is connected and not already started.
 * Should be called regularly in the main loop to process OTA events.
 */
void handleOTA() {
    if (!otaStarted && WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.setHostname("cover-controller");
        ArduinoOTA.setPassword(otaPassword.c_str());
        ArduinoOTA.begin();
        otaStarted = true;
        LOG("OTA ready");
    }

    if (otaStarted) {
        ArduinoOTA.handle();
    }
}
