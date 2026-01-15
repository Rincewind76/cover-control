#include "webserver.h"
#include <ESPAsyncWebServer.h>
#include "sdcard.h"
#include "config_manager.h"
#include <WiFi.h>
#include "web_log.h"
#include "ArduinoJSON.h"
#include "bme280_manager.h"
#include "dew_controller.h"
#include "usb_manager.h"
#include "power_control.h"
#include "button_manager.h"
#include "time_manager.h"

AsyncWebServer server(80);
AsyncEventSource logEvents("/log/events");

// Cached HTML for WiFi scan results
static String wifiScanHtml = "";

/**
 * @brief Reads the entire config.txt file from the SD card.
 *
 * @return String containing the file contents, or an error message.
 */
String loadConfigFileAsString() {
    File f = openConfigFile();
    if (!f) return "Could not open config.txt";

    String content = f.readString();
    f.close();
    return content;
}

/**
 * @brief Scans for WiFi networks and generates an HTML table.
 *
 * The result is stored in wifiScanHtml and displayed on /config.
 */
void updateWiFiScanResults() {
    int n = WiFi.scanNetworks();
    wifiScanHtml = "<table>";
    wifiScanHtml += "<tr><th>SSID</th><th>RSSI</th></tr>";

    if (n <= 0) {
        wifiScanHtml += "<tr><td colspan='2'>No networks found</td></tr>";
    } else {
        for (int i = 0; i < n; i++) {
            wifiScanHtml += "<tr>";
            wifiScanHtml += "<td>" + WiFi.SSID(i) + "</td>";
            wifiScanHtml += "<td>" + String(WiFi.RSSI(i)) + "</td>";
            wifiScanHtml += "</tr>";
        }
    }

    wifiScanHtml += "</table>";
}

/**
 * @brief Initializes the web server and registers all HTTP routes.
 */
void initWebServer() {

    // --- /config page ---
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
        String cfg = loadConfigFileAsString();

        String html = R"rawliteral(
            <html>
            <head>
                <title>Configuration</title>
                <style>
                    body {
                        font-family: Arial;
                        display: flex;
                        gap: 20px;
                        padding: 20px;
                    }
                    .left {
                        width: 60%;
                    }
                    .right {
                        width: 40%;
                    }
                    textarea {
                        width: 100%;
                        height: 400px;
                        font-family: monospace;
                    }
                    button {
                        margin-top: 10px;
                        padding: 8px 16px;
                    }
                    table {
                        width: 100%;
                        border-collapse: collapse;
                        margin-top: 10px;
                    }
                    th, td {
                        border: 1px solid #ccc;
                        padding: 6px 10px;
                        text-align: left;
                    }
                    th {
                        background: #eee;
                    }
                </style>
            </head>
            <body>
                <div class="left">
                    <h2>Configuration</h2>
                    <form action="/save_config" method="POST">
                        <textarea name="cfg">)rawliteral";

        html += cfg;

        html += R"rawliteral(</textarea><br>
                        <button type="submit">Save Config</button>
                    </form>
                    <form action="/reload_config" method="POST">
                        <button type="submit">Reload Config</button>
                    </form>
                </div>

                <div class="right">
                    <h2>WiFi Networks</h2>
        )rawliteral";

        html += wifiScanHtml;

        html += R"rawliteral(
                    <form action="/rescan_wifi" method="POST">
                        <button type="submit">Rescan WiFi Networks</button>
                    </form>
                </div>
            </body>
            </html>
        )rawliteral";

        request->send(200, "text/html", html);
    });

    // --- Save Config ---
    server.on("/save_config", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("cfg", true)) {
            request->send(400, "text/plain", "Missing cfg parameter");
            return;
        }

        String newCfg = request->getParam("cfg", true)->value();

        File f = SD.open("/config.txt", FILE_WRITE);
        if (!f) {
            request->send(500, "text/plain", "Could not write config.txt");
            return;
        }

        f.print(newCfg);
        f.close();

        loadConfigFromSD();  // reload configuration

        request->redirect("/config");
    });

    // --- Reload Config ---
    server.on("/reload_config", HTTP_POST, [](AsyncWebServerRequest *request) {
        loadConfigFromSD();
        request->redirect("/config");
    });

    // --- Rescan WiFi ---
    server.on("/rescan_wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        updateWiFiScanResults();
        request->redirect("/config");
    });

    // Initial scan
    updateWiFiScanResults();

    // --- Log Event Source ---
    server.addHandler(&logEvents);

    // --- Log Endpoint ---
    server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = R"rawliteral(
        <html>
        <head>
            <title>Live Log</title>
            <style>
                body { font-family: monospace; background:#111; color:#0f0; padding:10px; }
                #log { white-space: pre-wrap; }
            </style>
        </head>
        <body>
            <h2>Live Log</h2>
            <div id="log"></div>

            <script>
                const logDiv = document.getElementById("log");
                const evt = new EventSource("/log/events");

                evt.addEventListener("log", function(e) {
                    logDiv.innerText += e.data + "\n";
                    window.scrollTo(0, document.body.scrollHeight);
                });
            </script>
        </body>
        </html>
        )rawliteral";

        request->send(200, "text/html", html);
    });

    // Status Page with JSON
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {

    BmeStatus bme = bme_getStatus();
    DewStatus dew = dew_getStatus();
    const WandererStatus* w = usb_manager_get_parsed_status();
    uint8_t potibrightness = getPotiBrightness();
    unsigned long now = millis();

    StaticJsonDocument<768> doc;

    // --- BME and internals ---
    doc["bme"]["present"]     = bme.present;
    doc["bme"]["temperature"] = bme.temperature;
    doc["bme"]["humidity"]    = bme.humidity;
    doc["bme"]["pressure"]    = bme.pressure;
    doc["bme"]["voltage"]    = power_readSupplyVoltage();
    doc["bme"]["systime"]    = now;
    doc["bme"]["time"]    = getTimeString();

    // --- Dew ---
    doc["dew"]["temperature"] = dew.temperature;
    doc["dew"]["humidity"]    = dew.humidity;
    doc["dew"]["dewPoint"]    = dew.dewPoint;
    doc["dew"]["dew1Power"]   = power_getDew1Level();
    doc["dew"]["dew1MaxPower"]   = dew1Level;
    doc["dew"]["dew2Power"]   = power_getDew2Level();
    doc["dew"]["dew2MaxPower"]   = dew2Level;
    doc["dew"]["active"]      = dew.active;


    // --- Wanderer ---
    if (w) {
        doc["wanderer"]["firmware"]          = w->firmware;
        doc["wanderer"]["close_position"]    = w->close_position;
        doc["wanderer"]["open_position"]     = w->open_position;
        doc["wanderer"]["current_position"]  = w->current_position;
        doc["wanderer"]["input_voltage"]     = w->input_voltage;
        doc["wanderer"]["brightness"]        = w->brightness;
        doc["wanderer"]["dew_heater"]         = w->dew_heater;
        doc["wanderer"]["asiair_enabled"]     = w->asiair_enabled;
        doc["wanderer"]["connection_status"]  = w->connection_status;
        doc["wanderer"]["poti_brightness"]    = potibrightness;
    } else {
        doc["wanderer"]["connection_status"] = false;
    }

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {

    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Status</title>
<style>
    body {
        font-family: Arial, sans-serif;
        background: #111;
        color: #eee;
        margin: 0;
        padding: 20px;
    }
    h1 {
        margin-bottom: 10px;
    }
    .grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
        gap: 15px;
    }
    .card {
        background: #1e1e1e;
        border-radius: 8px;
        padding: 15px;
        box-shadow: 0 0 10px rgba(0,0,0,0.4);
    }
    .card h2 {
        margin-top: 0;
        font-size: 18px;
        border-bottom: 1px solid #333;
        padding-bottom: 6px;
    }
    .row {
        display: flex;
        justify-content: space-between;
        margin: 6px 0;
    }
    .ok { color: #4caf50; }
    .warn { color: #ff9800; }
    .err { color: #f44336; }
</style>
</head>
<body>

<h1>System Status</h1>

<div class="grid">

    <div class="card">
        <h2>Environment</h2>
        <div class="row"><span>Sensor Present</span><span id="bme_present">-</span></div>
        <div class="row"><span>Temp</span><span id="bme_temp">-</span></div>
        <div class="row"><span>Humidity</span><span id="bme_hum">-</span></div>
        <div class="row"><span>Pressure</span><span id="bme_pres">-</span></div>
        <div class="row"><span>Supply Voltage</span><span id="bme_volt">-</span></div>
        <div class="row"><span>System Time</span><span id="bme_systime">-</span></div>
        <div class="row"><span>Local Time</span><span id="bme_time">-</span></div>
    </div>

    <div class="card">
        <h2>Dew Control</h2>
        <div class="row"><span>Temp</span><span id="dew_temp">-</span></div>
        <div class="row"><span>Humidity</span><span id="dew_hum">-</span></div>
        <div class="row"><span>Dew Point</span><span id="dew_dp">-</span></div>
        <div class="row"><span>Heater 1</span><span id="dew1">-</span></div>
        <div class="row"><span>Heater 1 Max</span><span id="dew1max">-</span></div>
        <div class="row"><span>Heater 2</span><span id="dew2">-</span></div>
        <div class="row"><span>Heater 2 Max</span><span id="dew2max">-</span></div>
        <div class="row"><span>Status</span><span id="dew_active">-</span></div>
    </div>

    <div class="card">
        <h2>Wanderer Cover</h2>
        <div class="row"><span>Firmware</span><span id="fw">-</span></div>
        <div class="row"><span>Position</span><span id="pos">-</span></div>
        <div class="row"><span>Voltage</span><span id="vin">-</span></div>
        <div class="row"><span>Brightness</span><span id="bright">-</span></div>
        <div class="row"><span>ASIAIR</span><span id="asiair">-</span></div>
        <div class="row"><span>Connection</span><span id="conn">-</span></div>
        <div class="row"><span>Poti Position</span><span id="poti">-</span></div>
    </div>

</div>

<div class="grid">
    <div class="card">
        <h2>Actions</h2>
        <div class="row">
            <button onclick="openCover()">Open Cover</button>
            <button onclick="setBrightness()">Turn On Light</button>

        </div>
        <div class="row" style="margin-top:10px;">
            <button onclick="closeCover()">Close Cover</button>
            <button onclick="turnOffLight()">Turn Off Light</button>
        </div>
    </div>

</div>

<script>
async function updateStatus() {
    try {
        const r = await fetch("/status");
        const s = await r.json();

        document.getElementById("bme_present").innerText = s.bme.present ? "YES" : "NO";
        document.getElementById("bme_temp").innerText = s.bme.temperature.toFixed(1) + " °C";
        document.getElementById("bme_hum").innerText = s.bme.humidity.toFixed(1) + " %";
        document.getElementById("bme_pres").innerText = s.bme.pressure.toFixed(0) + " hPa";
        document.getElementById("bme_volt").innerText = s.bme.voltage.toFixed(2) + " V";
        document.getElementById("bme_systime").innerText = s.bme.systime + " ms";
        document.getElementById("bme_time").innerText = s.bme.time;

        document.getElementById("dew_temp").innerText = s.dew.temperature.toFixed(1) + " °C";
        document.getElementById("dew_hum").innerText = s.dew.humidity.toFixed(1) + " %";
        document.getElementById("dew_dp").innerText = s.dew.dewPoint.toFixed(1) + " °C";
        document.getElementById("dew1").innerText = s.dew.dew1Power + " %";
        document.getElementById("dew1max").innerText = s.dew.dew1MaxPower + " %";
        document.getElementById("dew2").innerText = s.dew.dew2Power + " %";
        document.getElementById("dew2max").innerText = s.dew.dew2MaxPower + " %";
        document.getElementById("dew_active").innerText = s.dew.active ? "ACTIVE" : "OFF";

        document.getElementById("fw").innerText = s.wanderer.firmware;
        document.getElementById("pos").innerText =
            s.wanderer.current_position.toFixed(1) + " °";
        document.getElementById("vin").innerText =
            s.wanderer.input_voltage.toFixed(2) + " V";
        document.getElementById("bright").innerText = s.wanderer.brightness;
        document.getElementById("asiair").innerText =
            s.wanderer.asiair_enabled ? "ENABLED" : "DISABLED";
        document.getElementById("conn").innerText =
            s.wanderer.connection_status ? "CONNECTED" : "DISCONNECTED";
        document.getElementById("poti").innerText = s.wanderer.poti_brightness;

    } catch (e) {
        console.log("Status update failed", e);
    }
}

updateStatus();
setInterval(updateStatus, 2000);

function sendCommand(url) {
    fetch(url, { method: "POST" })
        .then(r => console.log("Command sent:", url))
        .catch(e => console.error("Command failed:", url, e));
}

function openCover() {
    sendCommand("/action/open_cover");
}

function closeCover() {
    sendCommand("/action/close_cover");
}

function turnOffLight() {
    sendCommand("/action/turn_off_light");
}

function setBrightness() {
    sendCommand("/action/set_brightness");
}

</script>

</body>
</html>
)rawliteral";

    request->send(200, "text/html", html);
});

server.on("/action/open_cover", HTTP_POST, [](AsyncWebServerRequest *request){
    usb_manager_open_cover();
    request->send(200, "text/plain", "OK");
});

server.on("/action/close_cover", HTTP_POST, [](AsyncWebServerRequest *request){
    usb_manager_close_cover();
    request->send(200, "text/plain", "OK");
});

server.on("/action/turn_off_light", HTTP_POST, [](AsyncWebServerRequest *request){
    usb_manager_turn_off_light();
    request->send(200, "text/plain", "OK");
});

server.on("/action/set_brightness", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("level")) {
        int level = request->getParam("level")->value().toInt();
        level = constrain(level, 1, 255);
        usb_manager_set_brightness(level);
    } else {
        usb_manager_set_brightness(getPotiBrightness()); // set brightness to poti level
    }
    request->send(200, "text/plain", "OK");
});

    // Start server
    server.begin();
}
