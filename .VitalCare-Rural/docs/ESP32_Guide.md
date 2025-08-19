# ESP32-WROOM-32 Development Guide

## Hardware Specifications

### ESP32-WROOM-32 Features
- **CPU**: Dual-core Xtensa 32-bit LX6 microprocessor (up to 240 MHz)
- **Memory**: 520 KB SRAM, 4 MB Flash
- **WiFi**: 802.11 b/g/n (2.4 GHz)
- **Bluetooth**: v4.2 BR/EDR and BLE
- **GPIO**: 34 programmable GPIO pins
- **ADC**: 18 channels, 12-bit SAR ADC
- **DAC**: 2 channels, 8-bit DAC
- **PWM**: 16 channels
- **Interfaces**: SPI, I2C, I2S, UART, CAN 2.0

### Pin Configuration
```
                     ESP32-WROOM-32 Pinout
                    ┌─────────────────────┐
               3V3  │ 1                39 │ GND
               EN   │ 2                38 │ D27
          SVP  D36  │ 3                37 │ D25
          SVN  D39  │ 4                36 │ D32
               D34  │ 5                35 │ D33
               D35  │ 6                34 │ D26
               D32  │ 7                33 │ D14
               D33  │ 8                32 │ D12
               D25  │ 9                31 │ D13
               D26  │10                30 │ GND
               D27  │11                29 │ VIN
               D14  │12                28 │ D23
               D12  │13                27 │ D22
              GND   │14                26 │ D21
               D13  │15                25 │ D19
               D9   │16                24 │ D18
               D10  │17                23 │ D5
               D11  │18                22 │ D17
              VIN   │19                21 │ D16
                    └─────────────────────┘
```

## Development Frameworks

### 1. Arduino Framework
- Easy to use, Arduino-compatible
- Large library ecosystem
- Good for beginners and rapid prototyping

### 2. ESP-IDF Framework
- Official Espressif development framework
- More advanced features and control
- Better for professional development
- FreeRTOS integration

## Common Sensors and Modules

### Temperature & Humidity
- **DHT22**: Digital temperature and humidity sensor
- **DS18B20**: Digital temperature sensor (1-Wire)
- **BME280**: Pressure, temperature, and humidity sensor

### Motion & Proximity
- **PIR**: Passive infrared motion sensor
- **HC-SR04**: Ultrasonic distance sensor
- **MPU6050**: 6-axis gyroscope and accelerometer

### Communication Modules
- **ESP32 Built-in WiFi**: 2.4GHz 802.11 b/g/n
- **ESP32 Built-in Bluetooth**: Classic and BLE
- **LoRa**: Long-range communication
- **GSM/GPRS**: Cellular communication

### Displays
- **OLED (SSD1306)**: 128x64 monochrome display
- **LCD (16x2)**: Character display
- **TFT**: Color display modules

## Wiring Examples

### Basic LED Circuit
```
ESP32 Pin D2 → 330Ω Resistor → LED Anode (+)
LED Cathode (-) → GND
```

### DHT22 Sensor
```
DHT22 VCC → ESP32 3.3V
DHT22 GND → ESP32 GND
DHT22 Data → ESP32 D4 (with 10kΩ pull-up resistor to 3.3V)
```

### I2C OLED Display
```
OLED VCC → ESP32 3.3V
OLED GND → ESP32 GND
OLED SDA → ESP32 D21
OLED SCL → ESP32 D22
```

## Programming Tips

### 1. Power Management
```cpp
// Deep sleep for power saving
esp_deep_sleep(10 * 1000000); // Sleep for 10 seconds
```

### 2. WiFi Connection
```cpp
#include <WiFi.h>

WiFi.begin("SSID", "PASSWORD");
while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
}
```

### 3. Serial Communication
```cpp
Serial.begin(115200);
Serial.println("Hello ESP32!");
```

### 4. Analog Reading
```cpp
int sensorValue = analogRead(A0);
float voltage = sensorValue * (3.3 / 4095.0);
```

## Troubleshooting

### Common Issues

1. **Upload Failed**
   - Hold BOOT button while uploading
   - Check COM port selection
   - Ensure correct baud rate (115200)

2. **Brown-out Detector**
   - Power supply insufficient
   - Use quality USB cable
   - External power source may be needed

3. **WiFi Connection Issues**
   - Check SSID and password
   - Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
   - Signal strength and range

4. **Memory Issues**
   - Use SPIFFS or LittleFS for file storage
   - Optimize code and reduce memory usage
   - Use external memory if needed

### Debugging Tips
- Use Serial.println() for debugging
- Monitor serial output at 115200 baud
- Use ESP32 exception decoder
- Check power supply voltage (should be 3.3V)

## Resources

- [ESP32 Official Documentation](https://docs.espressif.com/projects/esp-idf/)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [Community Forums](https://www.esp32.com/)
