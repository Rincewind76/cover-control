#include "led_manager.h"
#include "pins.h"

/**
 * @struct LedState
 * @brief Internal state of an LED
 */
struct LedState {
    uint8_t pin;           ///< GPIO pin
    uint8_t channel;       ///< LEDC channel
    LedMode mode;          ///< Current mode
    uint32_t lastToggle;   ///< Timestamp for blink logic
    bool state;            ///< Current on/off state
};

/**
 * @brief Internal LED table
 *
 * The channels are intentionally chosen >1
 * to avoid conflicts with internal ESP32 functions.
 */
static LedState leds[LED_COUNT] = {
    { PIN_LED_WLAN,   2, LED_MODE_OFF, 0, false },
    { PIN_LED_STATUS, 3, LED_MODE_OFF, 0, false },
    { PIN_LED_LIGHT,  4, LED_MODE_OFF, 0, false }
};

/**
 * @brief Global LED brightness (PWM scaling)
 */
static uint8_t globalBrightness = 255;

/**
 * @brief Initializes LEDC and attaches pins
 */
void initLeds() {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        ledcSetup(leds[i].channel, 5000, 8);   // 5 kHz, 8 bit
        ledcAttachPin(leds[i].pin, leds[i].channel);
        ledcWrite(leds[i].channel, 0);
    }
}

/**
 * @brief Sets the mode of an LED
 */
void setLedMode(LedId led, LedMode mode) {
    if (led >= LED_COUNT) return;
    if (getLedMode(led) == mode) return;
    
    leds[led].mode = mode;
    leds[led].lastToggle = millis();
    leds[led].state = false;
}

/**
 * @brief Returns the current mode of an LED
 */
LedMode getLedMode(LedId led) {
    if (led >= LED_COUNT) {
        return LED_MODE_OFF;
    }
    return leds[led].mode;
}


/**
 * @brief Sets the global LED brightness
 */
void setGlobalLedBrightness(uint8_t brightness) {
    globalBrightness = brightness;
}

/**
 * @brief Updates all LEDs (blinking / steady on)
 */
void updateLeds() {
    uint32_t now = millis();

    for (uint8_t i = 0; i < LED_COUNT; i++) {
        LedState &led = leds[i];

        switch (led.mode) {

        case LED_MODE_OFF:
            ledcWrite(led.channel, 0);
            break;

        case LED_MODE_ON:
            ledcWrite(led.channel, globalBrightness);
            break;

        case LED_MODE_BLINK_SLOW:
            if (now - led.lastToggle >= 500) {
                led.lastToggle = now;
                led.state = !led.state;
            }
            ledcWrite(led.channel, led.state ? globalBrightness : 0);
            break;

        case LED_MODE_BLINK_FAST:
            if (now - led.lastToggle >= 150) {
                led.lastToggle = now;
                led.state = !led.state;
            }
            ledcWrite(led.channel, led.state ? globalBrightness : 0);
            break;
        }
    }
}
