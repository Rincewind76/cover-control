/**
 * @file ota.h
 * @brief OTA update support
 *
 * Enables firmware updates over WiFi (ArduinoOTA).
 */

#ifndef OTA_H
#define OTA_H

#include <Arduino.h>

// OTA configuration
extern String otaPassword;

void initOTA();
void handleOTA();

#endif
