# PIN Belegung Lolin S3 Pro:

| PIN    | Name       | Type          | Function |
| ---    | ----       | --------      | --------- |
| GPIO1  | POT_LIGHT  | Analog in     | Potentiometer for Light Intensity (50kOhm) |
| GPIO2  | LED_STATUS | Digital Out   | Control of Status LED |
| GPIO18 | LED_WLAN | Digital Out   | Control of WiFi LED |
| GPIO8  | LED_LIGHT | Digital Out   | Control of Light LED |
| GPIO42 | BTN_OPEN   | Input Pullup  | Switch input Open Cover, needs software debounce |
| GPIO41 | BTN_CLOSE  | Input Pullup  | Switch input Close Cover, needs software debounce |
| GPIO40 | BTN_LIGHT_ON  | Input Pullup  | Switch input Light on, needs software debounce |
| GPIO39 | BTN_LIGHT_OFF  | Input Pullup  | Switch input Light off, needs software debounce |
| GPIO5  | DEW1_OUT | Digital Out   | Enable DEW1 Heater 12V Supply, PWM |
| GPIO7  | DEW2_OUT | Digital Out   | Enable DEW2 Heater 12V Supply, PWM |
| GPIO16  | PAN_OUT | Digital Out   | Enable Flat Panel 12V Supply |
| GPIO4  | VIN_12V_SENSE  | Analog in     | Supply Voltage monitor, voltage divider with 100kOhm and 22kOhm |
| GPIO13  | SD_MISO  | SPI MISO     | SPI for SD-Card, MISO |
| GPIO12 | SD_SCK  | SPI CLK     | SPI for SD-Card, CLK |
| GPIO11 | SD_MOSI  | SPI MOSI     | SPI for SD-Card, MOSI |
| GPIO46 | SD_CS  | SPI CS     | SPI for SD-Card, CS |
| GPIO19 | D-  | USB D-     | USB Host to Cover, D- |
| GPIO20 | D+  | USB D+     | USB Host to Cover, D+ |
| GPIO9 | SDA  | I2C SDA     | I2C bus, connected to BME280 Sensor (0x77) and SSD1306 0,96" OLED Display (0x3C) |
| GPIO10 | SCL  | I2C SCL    | I2C bus, connected to BME280 Sensor (0x77) and SSD1306 0,96" OLED Display (0x3C) |

