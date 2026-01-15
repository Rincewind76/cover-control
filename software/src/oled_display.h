#pragma once

#include <Arduino.h>
#include "bme280_manager.h"
#include "dew_controller.h"
#include "power_control.h"
#include "wifi_config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "usb_manager.h"
#include "button_manager.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C

extern Adafruit_SSD1306 display;

void oled_init();
void oled_update();
void drawWifiIcon(int x, int y, bool connected);
