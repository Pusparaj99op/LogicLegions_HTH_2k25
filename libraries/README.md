# Libraries

This folder is for custom libraries that you create or download for your Arduino and ESP32 projects.

## Folder Structure

```
libraries/
├── CustomSensor/
│   ├── CustomSensor.h
│   ├── CustomSensor.cpp
│   ├── examples/
│   │   └── basic_usage/
│   │       └── basic_usage.ino
│   └── README.md
├── WiFiManager/
│   └── ... (downloaded library files)
└── README.md (this file)
```

## How to Add Libraries

### Method 1: Arduino IDE Library Manager
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for the library you want
4. Click Install

### Method 2: Manual Installation
1. Download the library (usually a .zip file)
2. Extract to this `libraries/` folder
3. Restart Arduino IDE/VSCode

### Method 3: Git Submodules (Advanced)
```bash
# Add a library as git submodule
git submodule add https://github.com/author/library-name.git libraries/library-name

# Update all submodules
git submodule update --init --recursive
```

## Popular Libraries for IoT Development

### ESP32 Libraries:
- **WiFiManager** - Easy WiFi configuration
- **ArduinoJson** - JSON parsing and creation
- **PubSubClient** - MQTT client
- **AsyncWebServer** - Asynchronous web server
- **WebSocketsServer** - WebSocket communication
- **ESP32Servo** - Servo control for ESP32
- **OneWire** - One-wire protocol (DS18B20 temperature sensors)

### Sensor Libraries:
- **Adafruit Unified Sensor** - Base for many Adafruit sensors
- **DHT sensor library** - Temperature and humidity sensors
- **Adafruit BME280** - Environmental sensor
- **Adafruit SSD1306** - OLED display
- **LiquidCrystal** - LCD displays
- **NewPing** - Ultrasonic sensors

### Communication Libraries:
- **SoftwareSerial** - Additional serial ports
- **Wire** - I2C communication
- **SPI** - SPI communication
- **ESP32 BLE Arduino** - Bluetooth Low Energy

## Creating Custom Libraries

### Basic Library Structure:
1. Create a folder with your library name
2. Add `.h` (header) and `.cpp` (implementation) files
3. Include examples and documentation

### Example Header File (MyLibrary.h):
```cpp
#ifndef MyLibrary_h
#define MyLibrary_h

#include "Arduino.h"

class MyLibrary {
  public:
    MyLibrary(int pin);
    void begin();
    int readValue();
  private:
    int _pin;
};

#endif
```

### Example Implementation (MyLibrary.cpp):
```cpp
#include "Arduino.h"
#include "MyLibrary.h"

MyLibrary::MyLibrary(int pin) {
  _pin = pin;
}

void MyLibrary::begin() {
  pinMode(_pin, INPUT);
}

int MyLibrary::readValue() {
  return analogRead(_pin);
}
```

## Library Management Tips

1. **Version Control**: Keep track of library versions
2. **Documentation**: Always include examples and documentation
3. **Testing**: Test libraries with different boards
4. **Dependencies**: Document any library dependencies
5. **Licensing**: Respect library licenses and attribution

## Troubleshooting

### Common Issues:
- **Library not found**: Check folder names and file extensions
- **Compilation errors**: Verify library compatibility with your board
- **Version conflicts**: Use compatible library versions

### Solutions:
- Restart Arduino IDE/VSCode after adding libraries
- Check include paths in VSCode settings
- Verify library examples work first