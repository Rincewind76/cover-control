#ifndef USB_MANAGER_H
#define USB_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char firmware[16];       // Firmware version (YYYYMMDD)
    float close_position;    // Close position set (°)
    float open_position;     // Open position set (°)
    float current_position;  // Current position (°)
    float input_voltage;     // Input voltage (V)
    int brightness;      // Flat panel brightness (0-255)
    int dew_heater;      // Dew heater power (0=OFF, 50=Low, 100=Mid, 150=High)
    int asiair_enabled;  // ASIAIR control enabled (0=Disabled, 1=Enabled)
    bool connection_status; // Connection status
} WandererStatus;

/**
 * Initializes the USB serial host connection for the WandererCover protocol.
 */
void usb_manager_init();

/**
 * Non-blocking update function to be called in the main loop.
 * Reads incoming data, processes status messages (updates on newline), and parses them.
 */
void usb_manager_update();

/**
 * Sends the command to open the cover.
 */
void usb_manager_open_cover();

/**
 * Sends the command to close the cover.
 */
void usb_manager_close_cover();

/**
 * Sets the flat panel brightness (1-255).
 * @param level Brightness level (clamped to 1-255).
 */
void usb_manager_set_brightness(int level);

/**
 * Turns off the flat light.
 */
void usb_manager_turn_off_light();

/**
 * Gets the last received raw status message.
 * @return Pointer to the null-terminated status string (valid until next update).
 */
const char* usb_manager_get_status();

/**
 * Gets the parsed status structure.
 * @return Pointer to the parsed WandererStatus (updated only on valid messages).
 */
const WandererStatus* usb_manager_get_parsed_status();

#ifdef __cplusplus
}
#endif

#endif // USB_MANAGER_H