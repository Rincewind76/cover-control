/**
 * @file wifi_config.h
 * @brief WiFi initialization and reconnect logic
 */

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>

/**
 * @brief Initializes the WiFi module.
 */
void initWiFi();

/**
 * @brief Handles WiFi connection management and reconnection.
 */
void handleWiFi();

/**
 * @brief Checks if the device is currently connected to WiFi.
 * @return true if connected to WiFi, false otherwise.
 */
bool isWiFiConnected();

#endif
