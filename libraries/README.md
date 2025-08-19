# 📚 Libraries and Dependencies - VitalCare Rural

## Required Libraries for Single ESP32 Complete System

### Core ESP32 Libraries
```cpp
// Core ESP32 Framework
#include <WiFi.h>              // WiFi functionality
#include <WebServer.h>         // HTTP web server
#include <WiFiAP.h>           // WiFi Access Point mode
#include <SPIFFS.h>           // File system for web files
#include <ESPmDNS.h>          // mDNS responder
#include <ArduinoJson.h>      // JSON parsing and generation
#include <WebSocketsServer.h>  // Real-time communication
```

### Hardware Interface Libraries
```cpp
// I2C and SPI Communication
#include <Wire.h>             // I2C communication (BMP180)
#include <SPI.h>              // SPI communication (SD card)

// Sensor Libraries
#include <Adafruit_BMP085.h>  // BMP180 temperature/pressure sensor
#include <Adafruit_Unified_Sensor.h> // Unified sensor framework
#include <SD.h>               // SD card file operations

// Communication Libraries
#include <SoftwareSerial.h>   // Software serial for SIM800L
```

---

## PlatformIO Library Dependencies

### platformio.ini Configuration
```ini
; Library Dependencies for Complete System
lib_deps = 
    ; Core WiFi and Web Server
    WiFi
    WebServer
    WiFiAP
    SPIFFS
    ArduinoJson @ ^6.21.3
    WebSocketsServer @ ^2.3.6
    ESPmDNS
    
    ; Sensor Libraries
    Wire
    SPI
    SD
    adafruit/Adafruit BMP085 Library @ ^1.2.2
    adafruit/Adafruit Unified Sensor @ ^1.1.13
    
    ; Communication Libraries
    SoftwareSerial
    
    ; Additional Utilities
    AsyncTCP @ ^1.1.1
```

---

## Installation Instructions

### Method 1: PlatformIO (Recommended)
1. **Install PlatformIO Extension in VS Code**
2. **Open project folder**
3. **Libraries will auto-install** when you build the project
4. **No manual installation required**

### Method 2: Arduino IDE
1. **Open Arduino IDE**
2. **Install ESP32 Board Package:**
   - File → Preferences
   - Add to Additional Board Manager URLs:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager
   - Search "ESP32" and install

3. **Install Required Libraries:**
   ```
   Tools → Manage Libraries → Search and Install:
   ```
   - ArduinoJson (by Benoit Blanchon)
   - WebSocketsServer (by Markus Sattler) 
   - Adafruit BMP085 Library
   - Adafruit Unified Sensor

---

## Library Details and Usage

### 1. ArduinoJson Library
**Purpose:** Handle JSON data for patient information and vital signs
```cpp
// Example Usage:
DynamicJsonDocument doc(1024);
doc["heartRate"] = currentVitals.heartRate;
doc["temperature"] = currentVitals.temperature;
String message;
serializeJson(doc, message);
```

### 2. WebSocketsServer Library  
**Purpose:** Real-time communication between ESP32 and web interface
```cpp
// Example Usage:
WebSocketsServer webSocket = WebSocketsServer(81);
webSocket.begin();
webSocket.onEvent(handleWebSocketEvent);
webSocket.broadcastTXT(vitalSignsJSON);
```

### 3. Adafruit BMP085 Library
**Purpose:** Interface with BMP180 temperature and pressure sensor
```cpp
// Example Usage:
Adafruit_BMP085 bmp180;
bmp180.begin();
float temperature = bmp180.readTemperature();
float pressure = bmp180.readPressure();
```

### 4. SD Library
**Purpose:** Store patient data and system logs on MicroSD card
```cpp
// Example Usage:
SD.begin(SD_CS_PIN);
File dataFile = SD.open("/patient_data.csv", FILE_WRITE);
dataFile.println("timestamp,heartRate,temperature");
dataFile.close();
```

### 5. SoftwareSerial Library
**Purpose:** Communication with SIM800L GSM module
```cpp
// Example Usage:
SoftwareSerial sim800(SIM800_RX_PIN, SIM800_TX_PIN);
sim800.begin(9600);
sim800.println("AT+CMGF=1"); // Set SMS text mode
```

---

## Custom Libraries and Headers

### VitalCare Custom Functions
Create a custom header file for common functions:

**File: `libraries/VitalCareUtils/VitalCareUtils.h`**
```cpp
#ifndef VITALCARE_UTILS_H
#define VITALCARE_UTILS_H

#include <Arduino.h>

// Patient data structure
struct PatientData {
    String id;
    String name;
    int age;
    String gender;
    String contact;
    String emergencyContact;
    String medicalConditions;
    unsigned long registrationTime;
};

// Vital signs structure
struct VitalSigns {
    float heartRate;
    float systolicBP;
    float diastolicBP;
    float spO2;
    float temperature;
    int ecgValue;
    float pressure;
    unsigned long timestamp;
    String status;
};

// Alert structure
struct Alert {
    String type;        // "warning", "critical"
    String message;
    String vital;
    float value;
    unsigned long timestamp;
};

// Utility Functions
class VitalCareUtils {
public:
    static String generatePatientID();
    static String formatTimestamp(unsigned long timestamp);
    static bool isValidHeartRate(float hr);
    static bool isValidTemperature(float temp);
    static bool isValidBloodPressure(float systolic, float diastolic);
    static String getVitalStatus(String vitalType, float value);
    static bool saveToSD(String filename, String data);
    static String createCSVHeader();
    static String vitalsToCSV(const VitalSigns& vitals);
};

#endif
```

**File: `libraries/VitalCareUtils/VitalCareUtils.cpp`**
```cpp
#include "VitalCareUtils.h"
#include <SD.h>

String VitalCareUtils::generatePatientID() {
    return "VCR" + String(millis()) + String(random(100, 999));
}

String VitalCareUtils::formatTimestamp(unsigned long timestamp) {
    unsigned long seconds = timestamp / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    return String(hours % 24) + ":" +
           (minutes % 60 < 10 ? "0" : "") + String(minutes % 60) + ":" +
           (seconds % 60 < 10 ? "0" : "") + String(seconds % 60);
}

bool VitalCareUtils::isValidHeartRate(float hr) {
    return (hr >= 30 && hr <= 220);
}

bool VitalCareUtils::isValidTemperature(float temp) {
    return (temp >= 90.0 && temp <= 110.0); // Fahrenheit
}

bool VitalCareUtils::isValidBloodPressure(float systolic, float diastolic) {
    return (systolic >= 60 && systolic <= 250 && 
            diastolic >= 40 && diastolic <= 150 &&
            systolic > diastolic);
}

String VitalCareUtils::getVitalStatus(String vitalType, float value) {
    if (vitalType == "heartRate") {
        if (value < 60) return "Bradycardia";
        if (value > 100) return "Tachycardia";
        return "Normal";
    }
    
    if (vitalType == "temperature") {
        if (value > 100.4) return "Fever";
        if (value < 97.0) return "Hypothermia";
        return "Normal";
    }
    
    // Add more vital types as needed
    return "Unknown";
}

bool VitalCareUtils::saveToSD(String filename, String data) {
    File file = SD.open(filename, FILE_APPEND);
    if (file) {
        file.println(data);
        file.close();
        return true;
    }
    return false;
}

String VitalCareUtils::createCSVHeader() {
    return "timestamp,heartRate,systolicBP,diastolicBP,spO2,temperature,ecgValue,pressure,status";
}

String VitalCareUtils::vitalsToCSV(const VitalSigns& vitals) {
    return String(vitals.timestamp) + "," +
           String(vitals.heartRate) + "," +
           String(vitals.systolicBP) + "," +
           String(vitals.diastolicBP) + "," +
           String(vitals.spO2) + "," +
           String(vitals.temperature) + "," +
           String(vitals.ecgValue) + "," +
           String(vitals.pressure) + "," +
           vitals.status;
}
```

---

## Sensor-Specific Libraries

### AD8232 ECG Sensor Interface
**File: `libraries/AD8232/AD8232.h`**
```cpp
#ifndef AD8232_H
#define AD8232_H

#include <Arduino.h>

class AD8232 {
private:
    int outputPin;
    int loPlusPin;
    int loMinusPin;
    int lastValue;
    unsigned long lastHeartbeat;
    int threshold;
    
public:
    AD8232(int output, int loPlus, int loMinus);
    void begin();
    int readECG();
    bool isConnected();
    bool detectHeartbeat();
    float getHeartRate();
    void setThreshold(int threshold);
};

#endif
```

### Pulse Sensor Interface
**File: `libraries/PulseSensor/PulseSensor.h`**
```cpp
#ifndef PULSE_SENSOR_H
#define PULSE_SENSOR_H

#include <Arduino.h>

class PulseSensor {
private:
    int sensorPin;
    int ledPin;
    int threshold;
    int signal;
    unsigned long lastBeatTime;
    int beatsPerMinute;
    int beatCount;
    unsigned long windowStart;
    
public:
    PulseSensor(int sensor, int led);
    void begin();
    void update();
    int getSignal();
    int getBPM();
    bool isBeat();
    void setThreshold(int thresh);
};

#endif
```

---

## Version Management

### Library Versions (Tested and Compatible)
```ini
; Tested Library Versions
ArduinoJson = 6.21.3
WebSocketsServer = 2.3.6  
Adafruit BMP085 Library = 1.2.2
Adafruit Unified Sensor = 1.1.13
AsyncTCP = 1.1.1
```

### ESP32 Board Package
```
ESP32 Board Package: 2.0.11 or later
Arduino Core: 1.8.19 or later
```

---

## Troubleshooting Library Issues

### Common Library Problems

#### 1. Compilation Errors
```
Error: Library not found
Solution: Check library installation and include paths
```

#### 2. Version Conflicts  
```
Error: Multiple library versions
Solution: Use specific versions in platformio.ini
```

#### 3. Memory Issues
```
Error: Not enough program storage space
Solution: Use "Huge APP" partition scheme
```

### Debug Library Issues
```cpp
// Add debug output to verify library initialization
void setup() {
    Serial.begin(115200);
    
    // Test each library
    Serial.println("Testing WiFi...");
    WiFi.mode(WIFI_AP);
    Serial.println(WiFi.softAP("test") ? "WiFi OK" : "WiFi FAIL");
    
    Serial.println("Testing JSON...");
    DynamicJsonDocument doc(256);
    doc["test"] = "value";
    Serial.println("JSON OK");
    
    // Add more tests as needed
}
```

---

## Performance Optimization

### Memory Management
```cpp
// Use static allocation where possible
StaticJsonDocument<512> doc;  // Instead of DynamicJsonDocument

// Free memory periodically
void loop() {
    // Your code here
    
    // Check free heap
    if (ESP.getFreeHeap() < 10000) {
        Serial.println("Low memory warning!");
    }
}
```

### CPU Optimization
```cpp
// Use appropriate delays to prevent watchdog resets
void loop() {
    // Main processing
    handleSensors();
    handleWebServer();
    
    // Allow other tasks to run
    delay(10);
    
    // Optional: Use FreeRTOS tasks for better performance
    vTaskDelay(pdMS_TO_TICKS(10));
}
```

---

## Updates and Maintenance

### Keeping Libraries Updated
1. **Check for updates monthly**
2. **Test thoroughly before updating production systems**
3. **Keep a backup of working library versions**
4. **Document any custom modifications**

### Custom Library Maintenance
1. **Version control custom libraries**
2. **Document all custom functions**
3. **Test custom libraries independently**
4. **Provide examples for custom library usage**

---

*This libraries guide ensures all dependencies are properly managed for the VitalCare Rural single ESP32 complete system.*
