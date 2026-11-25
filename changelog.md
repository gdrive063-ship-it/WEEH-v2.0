# Changelog

## 1.0.0 - 2026-11-25
- Added `weeh_firmware.ino` for ESP8266 (NodeMCU 1.0 / ESP-12E, core 3.1.x) with open WiFi hotspot, captive portal, non-blocking RF scanning, optional A0 ADC monitoring, device priority/rate-limiting, EEPROM persistence, and LED health/template modes.
- Added self-contained `index.html` dashboard and embedded it into `index_html.h` (`PROGMEM`) for offline serving from the ESP8266.
