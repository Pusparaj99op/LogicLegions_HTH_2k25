# PlatformIO ESP32 Project Template

This directory contains PlatformIO project templates for ESP32 development.

## Quick Start

1. Open Terminal in VS Code
2. Navigate to this directory:
   ```cmd
   cd examples\platformio
   ```
3. Create a new PlatformIO project:
   ```cmd
   pio project init --board esp32dev --project-option "framework=arduino"
   ```
4. Or use VS Code Command Palette:
   - Press `Ctrl+Shift+P`
   - Type "PlatformIO: New Project"
   - Select ESP32 Dev Module
   - Choose Arduino framework

## platformio.ini Template

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_port = COM3
monitor_port = COM3

; Library dependencies
lib_deps = 
    WiFi
    WebServer
    ArduinoJson
    PubSubClient
    DHT sensor library

; Build flags
build_flags = 
    -DCORE_DEBUG_LEVEL=3

; Upload protocol
upload_protocol = esptool

; Monitor filters
monitor_filters = 
    esp32_exception_decoder
```

## Common Libraries for IoT

- **WiFi**: Built-in WiFi connectivity
- **WebServer**: Create web servers
- **ArduinoJson**: JSON parsing and generation
- **PubSubClient**: MQTT client
- **DHT sensor library**: Temperature and humidity sensors
- **Adafruit Sensor**: Sensor abstraction layer
- **ESP32Servo**: Servo motor control
- **OneWire**: One-wire protocol communication
