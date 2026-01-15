#pragma once
#include <Arduino.h>

/**
 * @file webserver.h
 * @brief Web interface for configuration and WiFi scanning.
 *
 * This module provides:
 *  - A configuration page at /config
 *  - Display and editing of config.txt from the SD card
 *  - WiFi network scanning with RSSI display
 *  - Buttons for reloading and saving configuration
 *  - Non-blocking operation using ESPAsyncWebServer
 */

/**
 * @brief Initializes the web server and registers all routes.
 *
 * Must be called once after WiFi initialization.
 */
void initWebServer();

/**
 * @brief Performs a WiFi scan and updates the HTML table content.
 *
 * Called automatically at startup and when the user presses
 * the "Rescan WiFi Networks" button.
 */
void updateWiFiScanResults();
