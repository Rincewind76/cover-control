#include "power_control.h"

/**
 * @brief Initializes all power-related GPIOs and PWM channels.
 */
void power_init()
{
    // --- Supply voltage sense ---
    pinMode(PIN_VIN_12V_SENSE, INPUT);

    // --- PWM for Dew Heater 1 ---
    ledcSetup(PWM_CHANNEL_DEW1, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PIN_DEW1_OUT, PWM_CHANNEL_DEW1);
    ledcWrite(PWM_CHANNEL_DEW1, 0);

    // --- PWM for Dew Heater 2 ---
    ledcSetup(PWM_CHANNEL_DEW2, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PIN_DEW2_OUT, PWM_CHANNEL_DEW2);
    ledcWrite(PWM_CHANNEL_DEW2, 0);

    // --- Flat Panel Output ---
    pinMode(PIN_PAN_OUT, OUTPUT);
    digitalWrite(PIN_PAN_OUT, LOW);
}

/**
 * @brief Reads the 12V supply voltage using the analog input.
 */
float power_readSupplyVoltage()
{
    const float dividerFactor = (100000.0f + 22000.0f) / 22000.0f; // 5.545
    const float adcMax = 4095.0f;
    const float vRef = 3.3f;

    int raw = analogRead(PIN_VIN_12V_SENSE);
    return (raw / adcMax) * vRef * dividerFactor;
}

/**
 * @brief Sets the PWM level for Dew Heater 1.
 */
void power_setDew1(int percent)
{
    percent = constrain(percent, 0, 100);
    uint16_t pwm = (percent * ((1 << PWM_RESOLUTION) - 1)) / 100;
    ledcWrite(PWM_CHANNEL_DEW1, pwm);
}

/**
 * @brief Sets the PWM level for Dew Heater 2.
 */
void power_setDew2(int percent)
{
    percent = constrain(percent, 0, 100);
    uint16_t pwm = (percent * ((1 << PWM_RESOLUTION) - 1)) / 100;
    ledcWrite(PWM_CHANNEL_DEW2, pwm);
}

/**
 * @brief Returns the current PWM level of Dew Heater 1.
 */
int power_getDew1Level()
{
    uint32_t raw = ledcRead(PWM_CHANNEL_DEW1);
    int percent = (raw * 100) / ((1 << PWM_RESOLUTION) - 1);
    return constrain(percent, 0, 100);
}

/**
 * @brief Returns the current PWM level of Dew Heater 2.
 */
int power_getDew2Level()
{
    uint32_t raw = ledcRead(PWM_CHANNEL_DEW2);
    int percent = (raw * 100) / ((1 << PWM_RESOLUTION) - 1);
    return constrain(percent, 0, 100);
}

/**
 * @brief Enables or disables the flat panel output.
 */
void power_setPanel(bool on)
{
    digitalWrite(PIN_PAN_OUT, on ? HIGH : LOW);
}

/**
 * @brief Returns the current state of the flat panel output.
 */
bool power_isPanelOn()
{
    return digitalRead(PIN_PAN_OUT) == HIGH;
}
