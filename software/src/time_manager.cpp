/**
 * @file time_manager.cpp
 * @brief Time management and scheduled actions
 *
 * Handles NTP time synchronization and scheduled actions such as auto-closing the cover.
 */

#include "time_manager.h"
#include "led_manager.h"
#include "pins.h"
#include "config_manager.h"
#include "usb_manager.h"
#include "web_log.h"

static bool actionExecutedToday = false;
static int lastDay = -1;

/**
 * @brief Initializes the time manager and sets up NTP synchronization.
 *
 * Configures the system to use NTP servers for time synchronization (CET + DST).
 */
void initTimeManager() {
    // Initialize NTP (CET + DST)
    configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");
    LOG("Time Manager initialised");
}

/**
 * @brief Returns the current time as a string in HH:MM format.
 *
 * @return String representation of the current time, or "00:00" if time is not available.
 */
String getTimeString() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "00:00";
    }

    char buffer[6];
    strftime(buffer, sizeof(buffer), "%H:%M", &timeinfo);
    return String(buffer);
}

/**
 * @brief Retrieves the current hour and minute.
 *
 * @param[out] hour   Reference to store the current hour.
 * @param[out] minute Reference to store the current minute.
 * @return true if the time was successfully retrieved, false otherwise.
 */
bool getTime(int &hour, int &minute) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return false;
    }

    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    return true;
}

/**
 * @brief Checks and executes scheduled actions based on the current time.
 *
 * If auto-close is enabled and the scheduled time is reached, triggers the auto-close action.
 * Ensures the action is only executed once per day.
 */
void checkScheduledActions() {
    if (!autoCloseCover) return;

    int hour, minute;
    if (!getTime(hour, minute)) return;

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return;

    int today = timeinfo.tm_mday;

    // Day change → allow action again
    if (today != lastDay) {
        lastDay = today;
        actionExecutedToday = false;
    }

    if (actionExecutedToday) return;

    if (hour == autoCloseHour &&
        minute == autoCloseMinute) {

        LOG("Scheduled auto-close triggered!");

        // Close the cover here
        usb_manager_close_cover();

        actionExecutedToday = true;
    }
}
