/**
 * @file main.cpp
 * @brief Central setup function and cyclic loop
 *
 * Responsible for:
 * - initialize all subsystems
 * - init order: Pins → SD → Config → WiFi → OTA
 * - cyclic call of non-blocking handler fcunctions

 */

#include <Arduino.h>
#include "pins.h"
#include "sdcard.h"
#include "config_manager.h"
#include "wifi_config.h"
#include "ota.h"
#include "led_manager.h"
#include "button_manager.h"
#include "power_control.h"
#include "webserver.h"
#include "time_manager.h"
#include "web_log.h"
#include "bme280_manager.h"
#include "dew_controller.h"
#include "oled_display.h"
#include "usb_manager.h"

void setup() {

    //Serial.begin(115200);
    //delay(200);

    LOG("+-- Telescope Cover Controller Starting --+");

    initPins();     // GPIO-init
    LOG("Pins initialized");
    initLeds();     // LED initialize
    LOG("LEDs initialized");
    power_init();
    power_setPanel(true); // Power ON flat panel by default
    LOG("Power Outputs initialized");

    initButtons();
    LOG("Buttons initialized");

    // load config from SD card
    if (initSD()) {
        loadConfigFromSD();
    }

    // set LED brightness
    setGlobalLedBrightness(ledBrightnessNormal);

    // Wifi Mode intiialized - NO CONNECTION
    initWiFi();
    delay(100);
    LOG("WiFi initialized");

    // Time Manager init (NTP)
    initTimeManager();
    LOG("Time Manager initialized");

	//Start Webserver
	initWebServer();
    LOG("Webserver started");

    // OTA init after wifi mode initialized
    initOTA();
    LOG("OTA initialized");
    

    // Initialize USB Cover Manager
    //usb_manager_init(); // Initialize USB Host for CH341
    LOG("USB Host initialized");

    // BME280 init
    bme_init();
    dew_init();

    // OLED Display init
    oled_init();

    LOG("End of setup reached");
    LOG("*****************************************************");
}

void loop() {
    handleWiFi();   			// maintain Wifi connections
    handleOTA();    			// OTA-Handler
    updateLeds();   			// LED-Fading
	checkScheduledActions();	// Check for scheduled actions
    updatePoti();               // Update Potentiometer
    updateButtons();            // Update Buttons
    bme_loop();                 // Update BME280 sensor
    dew_update();               // Update Dew Controller
    oled_update();              // Update OLED Display
    usb_manager_update();               // USB Host CH341 task

    //process button events and poti changes
    if (buttonClicked(BTN_OPEN)) { 
        usb_manager_open_cover(); 
        LOG("Button OPEN clicked");
    }
    if (buttonClicked(BTN_CLOSE)) { 
        usb_manager_close_cover(); 
        LOG("Button CLOSE clicked");
    }
    if (buttonClicked(BTN_LIGHT_ON)) { 
        usb_manager_set_brightness(getPotiBrightness()); 
        LOG("Button LIGHT ON clicked");
    }
    if (buttonClicked(BTN_LIGHT_OFF)) { 
        usb_manager_turn_off_light();
        LOG("Button LIGHT OFF clic‚‚ked");
    }
    //handlePotiBrightness();
    
    delay(10);      			// CPU entlasten
}
