#pragma once
#include <Arduino.h>

/**
 * @struct ScheduledConfig
 * @brief Structure for scheduled actions configuration (currently empty).
 */
struct ScheduledConfig {

};

/**
 * @brief Initializes the time manager and sets up NTP synchronization.
 */
void initTimeManager();

/**
 * @brief Returns the current time as a string in HH:MM format.
 * @return String representation of the current time.
 */
String getTimeString();

/**
 * @brief Retrieves the current hour and minute.
 * @param[out] hour   Reference to store the current hour.
 * @param[out] minute Reference to store the current minute.
 * @return true if the time was successfully retrieved, false otherwise.
 */
bool getTime(int &hour, int &minute);

/**
 * @brief Checks and executes scheduled actions based on the current time.
 * @param cfg Reference to the scheduled actions configuration.
 */
void checkScheduledActions();
