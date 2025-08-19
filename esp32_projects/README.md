# ESP32 Projects

This folder contains ESP32 sketches and projects for ESP32 development boards including:
- ESP32-WROOM-32
- ESP32-DevKitC
- ESP32-CAM
- Other ESP32 variants

## Structure
Each project should be in its own folder with the main `.ino` file having the same name as the folder.

Example:
```
esp32_projects/
├── wifi_scanner/
│   └── wifi_scanner.ino
├── ble_beacon/
│   └── ble_beacon.ino
├── web_server/
│   └── web_server.ino
└── iot_sensor/
    ├── iot_sensor.ino
    ├── config.h
    └── platformio.ini
```

## Features
ESP32 projects can utilize:
- WiFi connectivity
- Bluetooth/BLE
- Multiple GPIO pins
- ADC/DAC
- Touch sensors
- Real-time clock
- Deep sleep modes

## Getting Started
1. Create a new folder for your project
2. Add your `.ino` file with the same name as the folder
3. Configure board settings for ESP32
4. Use VSCode's Arduino extension or PlatformIO for development