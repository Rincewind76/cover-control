#include "oled_display.h"
#include "web_log.h"
#include "time_manager.h"

// Instantiate display
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

void oled_init() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
        LOG("OLED init failed");
        return;
    }
    LOG("OLED init failed");
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Initializing...");
    display.display();
}

void oled_update() {
    display.clearDisplay();

    int y = 0;

    // --- WLAN Status & Time ---
    bool wifi = isWiFiConnected();
    String curtime = getTimeString();
    display.setCursor(0, y);
    display.print(curtime);
    display.setCursor(70, y);
    display.setTextSize(1);
    display.print(wifi ? "WiFi: OK" : "WiFi: --");
    //drawWifiIcon(110, y, wifi); // kleines WLAN Symbol
    y += 10;

    // --- 12V Spannung ---
    float vin = power_readSupplyVoltage();
    WandererStatus cover = *usb_manager_get_parsed_status();

    display.setCursor(0, y);
    display.printf("12V: %.1fV", vin);
    display.setCursor(70, y);
    display.print(cover.connection_status ? "USB: OK": "USB: --");
    y += 10;

    // --- Cover Light Status ---
    y += 5;

    // --- Environment: Temp & Humidity ---
    BmeStatus bme = bme_getStatus();
    display.setCursor(0, y);
    if(bme.present) {
        display.printf("T: %.1fC\n", bme.temperature);
        display.setCursor(70, y);
        display.printf("H: %.0f%%", bme.humidity);
    } else {
        display.print("BME missing");
    }
    y += 10;

    // --- Dewpoint & Dew Active ---
    DewStatus dew = dew_getStatus();
    display.setCursor(0, y);
    display.printf("DP: %.1fC", dew.dewPoint);
    display.setCursor(70, y);
    display.print("Dew: ");
    display.fillRect(105, y, dew.active ? 20 : 2, 6, SSD1306_WHITE);

    y += 15;
    // --- Brightness Display ---
    uint8_t potiBrightness = getPotiBrightness();
    display.drawRect(0,y,128,6,SSD1306_WHITE);
    display.fillRect(0,y,(cover.brightness /2),6,SSD1306_WHITE);
    display.drawLine((potiBrightness /2), y-2, (potiBrightness /2), y+8, SSD1306_WHITE);

    display.display();
}

void drawSunIcon(int x, int y, bool on) {
    if (!on) {
        // ausgegraute Sonne
        display.drawCircle(x, y, 6, SSD1306_WHITE);
        display.drawLine(x-4, y-4, x+4, y+4, SSD1306_WHITE);
        return;
    }

    // Sonne an
    display.fillCircle(x, y, 4, SSD1306_WHITE);

    // Strahlen
    display.drawLine(x, y-8, x, y-12, SSD1306_WHITE); // oben
    display.drawLine(x, y+8, x, y+12, SSD1306_WHITE); // unten
    display.drawLine(x-8, y, x-12, y, SSD1306_WHITE); // links
    display.drawLine(x+8, y, x+12, y, SSD1306_WHITE); // rechts

    display.drawLine(x-6, y-6, x-9, y-9, SSD1306_WHITE); // oben links
    display.drawLine(x+6, y-6, x+9, y-9, SSD1306_WHITE); // oben rechts
    display.drawLine(x-6, y+6, x-9, y+9, SSD1306_WHITE); // unten links
    display.drawLine(x+6, y+6, x+9, y+9, SSD1306_WHITE); // unten rechts
}

void drawWifiIcon(int x, int y, bool connected) {
    if (!connected) {
        // durchgestrichenes WLAN
        display.drawLine(x+2, y+1, x+8, y+7, SSD1306_WHITE);
        display.drawLine(x+2, y+7, x+8, y+1, SSD1306_WHITE);
        // Punkt
        display.fillCircle(x+5, y+9, 1, SSD1306_WHITE);
        return;
    }

    // Punkt
    display.fillCircle(x+5, y+9, 1, SSD1306_WHITE);

    // Bögen
    display.drawLine(x+4, y+7, x+6, y+7, SSD1306_WHITE);
    display.drawLine(x+2, y+5, x+3, y+4, SSD1306_WHITE);
    display.drawLine(x+3, y+4, x+7, y+4, SSD1306_WHITE);
    display.drawLine(x+7, y+4, x+8, y+5, SSD1306_WHITE);
    display.drawLine(x+1, y+3, x+3, y+1, SSD1306_WHITE);
    display.drawLine(x+3, y+1, x+7, y+1, SSD1306_WHITE);
    display.drawLine(x+7, y+1, x+9, y+3, SSD1306_WHITE);
}
