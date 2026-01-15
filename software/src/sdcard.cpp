/**
 * @file sdcard.cpp
 * @brief SD card initialization and file access
 */

#include "sdcard.h"
#include "pins.h"
#include <SPI.h>
#include "web_log.h"

SPIClass sdSPI(FSPI);

/**
 * @brief Initializes the SD card.
 *
 * Sets up the SPI interface and attempts to mount the SD card.
 * Prints status messages to the serial monitor.
 * Also runs a test and lists the root directory if initialization succeeds.
 *
 * @return true if SD card was successfully initialized, false otherwise
 */
bool initSD() {
    sdSPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

    if (!SD.begin(PIN_SD_CS, sdSPI)) {
        LOG("SD init failed");
        return false;
    }

    LOG("SD initialized");
    testSDCard();
    listRootDir();
    return true;
}

/**
 * @brief Opens the configuration file (config.txt) from the SD card.
 *
 * Checks if the file exists and opens it for reading.
 *
 * @return File object for config.txt if it exists, otherwise an empty File object
 */
File openConfigFile() {
    if (!SD.exists("/config.txt")) {
        LOG("config.txt missing");
        return File();
    }
    return SD.open("/config.txt", FILE_READ);
}

/**
 * @brief Tests the SD card and prints information about it.
 *
 * Prints card type and size to the serial monitor.
 */
void testSDCard() {
    LOG("---- SD CARD TEST ----");

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        LOG("No SD card attached");
        return;
    }

    LOG("Card Type: ");
    if (cardType == CARD_MMC) LOG("MMC");
    else if (cardType == CARD_SD) LOG("SDSC");
    else if (cardType == CARD_SDHC) LOG("SDHC");
    else LOG("UNKNOWN");

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    LOG("Card Size: " + String(cardSize) + " MB");

    LOG("----------------------");
}

/**
 * @brief Lists all files in the root directory of the SD card.
 *
 * Prints file names and sizes to the serial monitor.
 */
void listRootDir() {
    LOG("---- SD ROOT DIR ----");

    File root = SD.open("/");
    if (!root) {
        LOG("Failed to open root directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        LOG("FILE: " + String(file.name()) + "  SIZE: " + String(file.size()));
        file = root.openNextFile();
    }

    LOG("---------------------");
}
