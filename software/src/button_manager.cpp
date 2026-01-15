#include "button_manager.h"
#include "pins.h"
#include "usb_manager.h"

/**
 * @brief Array of GPIO pins for each button.
 */
static const uint8_t buttonPins[BUTTON_COUNT] = {
    PIN_BTN_OPEN,        // BTN_OPEN
    PIN_BTN_CLOSE,       // BTN_CLOSE
    PIN_BTN_LIGHT_ON,    // BTN_LIGHT_ON
    PIN_BTN_LIGHT_OFF    // BTN_LIGHT_OFF
};

/**
 * @brief Debounce time in milliseconds.
 */
static const uint32_t DEBOUNCE_MS = 40;

static uint32_t lastPoticheck = 0;
#define POTI_CHECK_INTERVAL_MS 500

/**
 * @struct ButtonState
 * @brief Stores the debounced state and event information for a button.
 */
struct ButtonState {
    bool stableState;          ///< Debounced button state
    bool lastReportedState;    ///< Last reported stable state
    uint32_t lastChange;       ///< Timestamp of last state change
    bool rawState;             ///< Raw button state
    ButtonEvent event;         ///< Last detected button event
};

/**
 * @brief Array holding the state for each button.
 */
static ButtonState btn[BUTTON_COUNT];

/**
 * @brief Initializes all button inputs and debouncing logic.
 *
 * Sets up the button pins as inputs with pull-up resistors and initializes their states.
 * Also initializes the potentiometer input pin.
 */
void initButtons() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);

        btn[i].stableState = HIGH;
        btn[i].lastReportedState = HIGH;
        btn[i].rawState = HIGH;
        btn[i].lastChange = millis();
        btn[i].event = BUTTON_NONE;
    }

    // Initialize potentiometer pin
    pinMode(PIN_POT_LIGHT, INPUT);
}

/**
 * @brief Updates the button states and processes debouncing.
 *
 * Reads the raw state of each button, applies debouncing logic,
 * and updates the event type for each button.
 * Should be called regularly in the main loop.
 */
void updateButtons() {
    uint32_t now = millis();

    for (int i = 0; i < BUTTON_COUNT; i++) {
        bool raw = digitalRead(buttonPins[i]);

        if (raw != btn[i].rawState) {
            btn[i].rawState = raw;
            btn[i].lastChange = now;
        }

        if ((now - btn[i].lastChange) >= DEBOUNCE_MS) {
            if (btn[i].stableState != raw) {
                btn[i].stableState = raw;

                if (raw == LOW) {
                    btn[i].event = BUTTON_PRESSED;
                } else {
                    if (btn[i].lastReportedState == LOW) {
                        btn[i].event = BUTTON_CLICK;
                    } else {
                        btn[i].event = BUTTON_RELEASED;
                    }
                }

                btn[i].lastReportedState = raw;
            } else {
                btn[i].event = BUTTON_NONE;
            }
        }
    }
}

/**
 * @brief Checks if the specified button was pressed.
 *
 * @param id Logical button identifier.
 * @return true if the button was pressed, false otherwise.
 */
bool buttonPressed(ButtonId id) {
    return btn[id].event == BUTTON_PRESSED;
}

/**
 * @brief Checks if the specified button was released.
 *
 * @param id Logical button identifier.
 * @return true if the button was released, false otherwise.
 */
bool buttonReleased(ButtonId id) {
    return btn[id].event == BUTTON_RELEASED;
}

/**
 * @brief Checks if the specified button was clicked (short press).
 *
 * @param id Logical button identifier.
 * @return true if the button was clicked, false otherwise.
 */
bool buttonClicked(ButtonId id) {
    return btn[id].event == BUTTON_CLICK;
}

/**
 * @brief Checks if the specified button is currently held down.
 *
 * @param id Logical button identifier.
 * @return true if the button is down, false otherwise.
 */
bool buttonIsDown(ButtonId id) {
    return btn[id].stableState == LOW;
}

// ---------------- POTI HANDLING ----------------
static uint8_t potiBrightness = 0; 
static int lastRaw = -1; 
static uint8_t lastSentBrightness = 255; // impossible value → forces first update

/**
 * @brief Updates the potentiometer value.
 *
 * Reads the analog value from the potentiometer pin, applies a noise filter,
 * and scales the value to a range of 0–255 for brightness control.
 * Should be called regularly to keep the brightness value updated.
 */
void updatePoti() {
    int raw = analogRead(PIN_POT_LIGHT);  // 0..4095

    // Noise filter: only update if change > 8 ADC counts
    if (abs(raw - lastRaw) < 8) {
        return;
    }

    lastRaw = raw;

    // Scale to 0..255 and invert
    potiBrightness = 255 - map(raw, 0, 4095, 0, 255);
}

/**
 * @brief Gets the current brightness value from the potentiometer.
 *
 * @return Brightness value (0–255).
 */
uint8_t getPotiBrightness() {
    return potiBrightness;
}

/**
 * @brief Updates the panel brightness, if panel was switched on
 *
 * @return void
 */
void handlePotiBrightness() {
    uint32_t now = millis();
    if( now - lastPoticheck < POTI_CHECK_INTERVAL_MS ) {
        return;
    }
    lastPoticheck = now;
    usb_manager_set_brightness(getPotiBrightness());
    
}
