/**
 * @file sdcard.h
 * @brief SD card abstraction
 *
 * Initializes the SD card and provides access to config.txt.
 */

#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <SD.h>

/**
 * @brief Initializes the SD card.
 * @return true if initialization was successful, false otherwise.
 */
bool initSD();

/**
 * @brief Opens the configuration file (config.txt) from the SD card.
 * @return File object for config.txt if it exists, otherwise an empty File object.
 */
File openConfigFile();

/**
 * @brief Tests the SD card and prints information about it.
 */
void testSDCard();

/**
 * @brief Lists all files in the root directory of the SD card.
 */
void listRootDir();

#endif
