/**
 * @file wifi_config.cpp
 * @brief WiFi management
 *
 * - initWiFi(): Sets the WiFi mode
 * - handleWiFi(): Cyclically connects to known networks
 */

#include "wifi_config.h"
#include "config_manager.h"
#include "led_manager.h"
#include "time_manager.h"
#include <WiFi.h>
#include "web_log.h"

static unsigned long lastCheck = 0;
static const unsigned long CHECK_INTERVAL = 10000;

/**
 * @brief Initializes the WiFi module.
 *
 * Sets the WiFi mode to station, disconnects any previous connections,
 * and turns off the WiFi LED indicator.
 */
void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    setLedMode(LED_WLAN, LED_MODE_OFF);
    delay(100);
}

/**
 * @brief Handles WiFi connection management.
 *
 * Periodically scans for available networks and attempts to connect to the strongest known network.
 * Updates the WiFi LED indicator based on connection status.
 * Initializes the time manager upon successful connection.
 */
void handleWiFi() {
    static wl_status_t lastStatus = WL_IDLE_STATUS;

    if (millis() - lastCheck < CHECK_INTERVAL) return;
    lastCheck = millis();

    wl_status_t status = WiFi.status();

    // --- Detect status change ---
    if (status != lastStatus) {
        lastStatus = status;

        if (status == WL_CONNECTED) {
            LOG("WiFi connected");
            setLedMode(LED_WLAN, LED_MODE_ON);

            initTimeManager();
            return;
        } else {
            setLedMode(LED_WLAN, LED_MODE_BLINK_SLOW);
        }
    }

    if (status == WL_CONNECTED) return;
    if (wifiCount == 0) return;

    setLedMode(LED_WLAN, LED_MODE_BLINK_SLOW);

    LOG("WiFi scanning...");

    int n = WiFi.scanNetworks();
    if (n <= 0) {
        LOG("No networks found");
        return;
    }

    int bestIndex = -1;
    int bestRssi = -1000;
    LOG("Found following networks:");
    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);

        LOG(ssid + " (RSSI " + String(rssi) + ")");

        for (int j = 0; j < wifiCount; j++) {
            if (ssid == wifiList[j].ssid && rssi > bestRssi) {
                bestRssi = rssi;
                bestIndex = j;
            }
        }
    }

    if (bestIndex < 0) {
        LOG("No known networks found");
        return;
    }

    LOG("Connecting to " + wifiList[bestIndex].ssid + " (RSSI " + String(bestRssi) + ")");

    WiFi.begin(
        wifiList[bestIndex].ssid.c_str(),
        wifiList[bestIndex].password.c_str()
    );
}

/**
 * @brief Checks if the device is currently connected to WiFi.
 *
 * @return true if connected to WiFi, false otherwise.
 */
bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}
