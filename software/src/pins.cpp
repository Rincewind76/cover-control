#include "pins.h"

// --- ANALOG INPUTS ---
const uint8_t PIN_POT_LIGHT        = 1;
const uint8_t PIN_VIN_12V_SENSE    = 4;

// --- DIGITAL OUTPUTS ---
const uint8_t PIN_LED_STATUS       = 2;
const uint8_t PIN_LED_WLAN         = 18;
const uint8_t PIN_LED_LIGHT        = 8;

const uint8_t PIN_DEW1_OUT         = 5;
const uint8_t PIN_DEW2_OUT         = 7;
const uint8_t PIN_PAN_OUT          = 16;

// --- BUTTON INPUTS ---
const uint8_t PIN_BTN_OPEN         = 42;
const uint8_t PIN_BTN_CLOSE        = 41;
const uint8_t PIN_BTN_LIGHT_ON     = 40;
const uint8_t PIN_BTN_LIGHT_OFF    = 39;

// --- SD CARD (SPI) ---
const uint8_t PIN_SD_MISO          = 13;
const uint8_t PIN_SD_MOSI          = 11;
const uint8_t PIN_SD_SCK           = 12;
const uint8_t PIN_SD_CS            = 46;

// --- USB HOST ---
const uint8_t PIN_USB_D_MINUS      = 19;
const uint8_t PIN_USB_D_PLUS       = 20;

// --- I2C BUS ---
const uint8_t PIN_I2C_SDA          = 9;
const uint8_t PIN_I2C_SCL          = 10;

// --- INITIALIZATION ---
void initPins() {
    // Digital outputs
    pinMode(PIN_LED_STATUS, OUTPUT);
    pinMode(PIN_LED_WLAN, OUTPUT);
    pinMode(PIN_LED_LIGHT, OUTPUT);

    pinMode(PIN_DEW1_OUT, OUTPUT);
    pinMode(PIN_DEW2_OUT, OUTPUT);
    pinMode(PIN_PAN_OUT, OUTPUT);

    // Buttons (active low)
    pinMode(PIN_BTN_OPEN, INPUT_PULLUP);
    pinMode(PIN_BTN_CLOSE, INPUT_PULLUP);
    pinMode(PIN_BTN_LIGHT_ON, INPUT_PULLUP);
    pinMode(PIN_BTN_LIGHT_OFF, INPUT_PULLUP);

    // Analog inputs → no pinMode() needed

    // SD‑Card pins → handled by SPIClass.begin()

    // USB pins → handled by USB Host stack

    // I2C pins → handled by Wire.begin()

    // Optional: set LEDs off at startup
    digitalWrite(PIN_LED_STATUS, LOW);
    digitalWrite(PIN_LED_WLAN, LOW);
    digitalWrite(PIN_LED_LIGHT, LOW);
}
