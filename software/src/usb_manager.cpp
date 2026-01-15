#include "usb_manager.h"
#include <USBHostSerial.h>
#include <string.h>  // For strlen, strcpy, strtok, strcmp
#include <stdlib.h>  // For atof, atoi
#include <stdio.h>   // For sprintf
#include "web_log.h"  // For LOG macro
#include "led_manager.h"

static USBHostSerial usbSerial;
static char current_status[256] = {0};  // Buffer for the latest raw status message
static char read_buffer[512] = {0};     // Temporary read buffer
static size_t buf_idx = 0;
static WandererStatus parsed_status = {0};  // Parsed status (zero-initialized)
static bool device_connected = false;
static uint32_t lastmessage = 0;

/**
 * Initializes the USB serial host connection.
 */
void usb_manager_init() {
    device_connected = usbSerial.begin(19200, 0, 0, 8);  // Baud 19200, parity none, stop 1, data 8
    if (device_connected) {
        LOG("USB Serial device connected");
        setLedMode(LED_STATUS, LED_MODE_ON);
    } else {
        LOG("USB Serial device not connected");
        setLedMode(LED_STATUS, LED_MODE_BLINK_FAST);
    }  
    parsed_status.connection_status = device_connected;
}

/**
 * Non-blocking update function to be called in the main loop.
 * Reads available data byte-by-byte, accumulates until newline, then updates & parses.
 */
void usb_manager_update() {
    uint32_t now = millis();
    
    if(!device_connected) {
        usb_manager_init();
        LOG("Re-initializing USB Serial device");
        return;
    }

    if(now - lastmessage > 5000) {
        // No message received for 5 seconds
        device_connected = false;
        parsed_status.connection_status = device_connected;
        setLedMode(LED_STATUS, LED_MODE_BLINK_FAST);
    };

    while (usbSerial.available()) {
        int c = usbSerial.read();
        if (c == -1) break;

        lastmessage = now;

        if (buf_idx < sizeof(read_buffer) - 1) {
            read_buffer[buf_idx++] = (char)c;
        }
        strcpy(current_status, read_buffer);      // Save raw message
        if(buf_idx > 200) {
            //string too long, re-init USB
            device_connected = false;
            parsed_status.connection_status = device_connected;
            setLedMode(LED_STATUS, LED_MODE_BLINK_FAST);
        }

        if ((char)c == '\n') {
            read_buffer[buf_idx] = '\0';              // Null-terminate
            strcpy(current_status, read_buffer);      // Save raw message
            LOG("Received cover status: " + String(current_status));

            // Parse status – expected format: WandererCoverV4Afield1Afield2A...Afield8
            char temp[256];
            strcpy(temp, current_status);

            char* token = strtok(temp, "A");
            if (token && strcmp(token, "WandererCoverV4") == 0) {
                // Field 1: Firmware version (YYYYMMDD)
                token = strtok(NULL, "A");
                if (token) strncpy(parsed_status.firmware, token, sizeof(parsed_status.firmware) - 1);

                // Field 2: Close position (°)
                token = strtok(NULL, "A");
                if (token) parsed_status.close_position = atof(token);

                // Field 3: Open position (°)
                token = strtok(NULL, "A");
                if (token) parsed_status.open_position = atof(token);

                // Field 4: Current position (°)
                token = strtok(NULL, "A");
                if (token) parsed_status.current_position = atof(token);

                // Field 5: Input voltage (V)
                token = strtok(NULL, "A");
                if (token) parsed_status.input_voltage = atof(token);

                // Field 6: Flat panel brightness (0-255)
                token = strtok(NULL, "A");
                if (token) parsed_status.brightness = (int)atoi(token);

                // Field 7: Dew heater power (0/50/100/150)
                token = strtok(NULL, "A");
                if (token) parsed_status.dew_heater = (int)atoi(token);

                // Field 8: ASIAIR control enabled (0/1)
                token = strtok(NULL, "A");
                if (token) parsed_status.asiair_enabled = (int)atoi(token);

                parsed_status.connection_status = device_connected;
            }

            buf_idx = 0;  // Reset for next message
        }
    }
}

/**
 * Helper: send numeric command as ASCII string
 */
static void send_command(uint32_t value) {
    char cmd[16];
    sprintf(cmd, "%u", value);
    usbSerial.write((const uint8_t*)cmd, strlen(cmd));
    LOG("Sent command: " + String(cmd));
}

/**
 * Open cover
 */
void usb_manager_open_cover() {
    send_command(1001);
}

/**
 * Close cover
 */
void usb_manager_close_cover() {
    send_command(1000);
}

/**
 * Set flat panel brightness (1–255)
 */
void usb_manager_set_brightness(int level) {
    if (level < 1) level = 1;
    if (level > 255) level = 255;
    send_command(level);
}

/**
 * Turn off flat light
 */
void usb_manager_turn_off_light() {
    send_command(9999);
}

/**
 * Get raw status string
 */
const char* usb_manager_get_status() {
    return current_status;
}

/**
 * Get parsed status struct
 */
const WandererStatus* usb_manager_get_parsed_status() {
    return &parsed_status;
}