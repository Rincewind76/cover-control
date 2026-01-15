# Starting Setup
I have the following equipment:
* WandererCover V4-EC Professional Flatpanel
* Celestron EdgeHD 800 with 0.7 reducer and Off-Axis-Guide and a Dew Heater
* ASIAir Plus
* ZWO ASI585MC (main) and ZWO ASI220M (Guide)
* ZWO Autofocuser
* ZWO Filter Wheel
* Sky Watcher EQ6-R Pro

So all equipment can be conviniently controlled by the ASIAir Plus, except the Wanderer Cover...
(To be fair: The Cover can be controlled by a series of power on/off cycles via the ASIAir - but this is a more a pain, than a feature)
I have a small Windows Tablet to control it - but this is overkill, just to open/close the cover and adjust the brightness.

So target of this project is to have a small standalone unit to control everything onsite via local controls (e.g. buttons)

# Requirements Collection

- Small controller with Wi-Fi capability to host a web interface
- USB host capability to control the WandererCover via USB virtual COM port  
  - Direct USB connection from the control system to the WandererCover
- Power supply via 12V  
  - The WandererCover is powered and switched through the control system
- Two additional separate 12V outputs, PWM-controlled  
  - For external dew heaters
- Ability to measure voltage level at the 12V input port
- Integrated temperature and humidity sensor
- Separate buttons:
  - Open
  - Close
  - Light on with a selected brightness via Poti
  - Light off
- LED feedback:
  - Light on/off status
  - Wi-Fi connection
  - USB communication feedback
- Optional: OLED support for a 0.96" display  
- Wi-Fi networks configurable via settings  
  - The strongest configured network should be used

---

# Key Components

- Wemos LOLIN S3 Pro
- Brightness adjustable via potentiometer
- BME280 climate sensor via I2C
- OLED AZDelivery SSD1306 0.96" display via I2C
- MOSFET IRLZ44N
- Buck converter 1584EN module
