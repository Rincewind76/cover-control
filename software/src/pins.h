#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// --- ANALOG INPUTS ---
extern const uint8_t PIN_POT_LIGHT;        // GPIO1
extern const uint8_t PIN_VIN_12V_SENSE;    // GPIO4

// --- DIGITAL OUTPUTS ---
extern const uint8_t PIN_LED_STATUS;       // GPIO2
extern const uint8_t PIN_LED_WLAN;         // GPIO18
extern const uint8_t PIN_LED_LIGHT;        // GPIO8

extern const uint8_t PIN_DEW1_OUT;         // GPIO5 (PWM)
extern const uint8_t PIN_DEW2_OUT;         // GPIO7 (PWM)
extern const uint8_t PIN_PAN_OUT;          // GPIO16

// --- BUTTON INPUTS (active low, pullup) ---
extern const uint8_t PIN_BTN_OPEN;         // GPIO42
extern const uint8_t PIN_BTN_CLOSE;        // GPIO41
extern const uint8_t PIN_BTN_LIGHT_ON;     // GPIO40
extern const uint8_t PIN_BTN_LIGHT_OFF;    // GPIO39

// --- SD CARD (SPI) ---
extern const uint8_t PIN_SD_MISO;          // GPIO13
extern const uint8_t PIN_SD_MOSI;          // GPIO11
extern const uint8_t PIN_SD_SCK;           // GPIO12
extern const uint8_t PIN_SD_CS;            // GPIO46

// --- USB HOST (WandererCover) ---
extern const uint8_t PIN_USB_D_MINUS;      // GPIO19
extern const uint8_t PIN_USB_D_PLUS;       // GPIO20

// --- I2C BUS ---
extern const uint8_t PIN_I2C_SDA;          // GPIO9
extern const uint8_t PIN_I2C_SCL;          // GPIO10

// --- INITIALIZATION ---
void initPins();

#endif
