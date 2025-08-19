# 🔧 Hardware Assembly Guide - VitalCare Rural

## Overview
This guide provides step-by-step instructions for assembling the VitalCare Rural portable health monitoring system using the available components.

## 📋 Component List

### Primary Components (Used)
- **3x ESP32-WROOM-32** - Main controllers
- **1x AD8232 Heart Rate Monitor** - ECG sensor
- **1x BMP180 Sensor** - Temperature and pressure
- **1x Pulse Sensor** - Heart rate detection
- **1x SIM800L Module** - Cellular communication
- **1x MicroSD Card Adapter** - Data storage
- **3x 3.7V 1000mAh Li-ion Batteries** - Power supply
- **1x Buzzer** - Audio alerts

### Secondary Components (Available for enhancements)
- **4x Arduino Mega** - Additional processing
- **2x Arduino Nano** - Auxiliary sensors
- **1x Arduino Uno** - System monitoring
- **1x W1209 Digital Temperature Controller** - Environmental control
- **4x Relay Modules** - System switching
- **2x Vibrator Motors** - Tactile feedback
- **1x Accelerometer** - Motion detection

---

## 🔌 Wiring Diagrams

### ESP32 #1 - Main Controller (Web Server)
```
ESP32 #1 Connections:
├── Power
│   ├── VIN → 3.7V Battery (+)
│   ├── GND → Battery (-) & Common Ground
│   └── EN → 10kΩ Pull-up to 3.3V
├── Status LEDs
│   ├── GPIO2 → Status LED (Built-in)
│   └── GPIO4 → External Status LED + 330Ω Resistor → GND
└── Communication
    ├── GPIO21 (SDA) → I2C Bus (Future expansion)
    ├── GPIO22 (SCL) → I2C Bus (Future expansion)
    └── Serial pins for debugging
```

### ESP32 #2 - Sensor Controller
```
ESP32 #2 Sensor Connections:
├── Power
│   ├── VIN → 3.7V Battery (+)
│   └── GND → Battery (-) & Common Ground
├── AD8232 ECG Sensor
│   ├── 3.3V → ESP32 3.3V
│   ├── GND → ESP32 GND
│   ├── OUTPUT → GPIO36 (A0)
│   ├── LO+ → GPIO18
│   ├── LO- → GPIO19
│   └── SDN → 3.3V (Always on)
├── BMP180 Sensor (I2C)
│   ├── VCC → ESP32 3.3V
│   ├── GND → ESP32 GND
│   ├── SDA → GPIO21
│   └── SCL → GPIO22
├── Pulse Sensor
│   ├── RED → ESP32 3.3V
│   ├── BLACK → ESP32 GND
│   └── PURPLE → GPIO39 (A3)
└── Visual Feedback
    ├── GPIO2 → Pulse LED + 330Ω → GND
    └── GPIO5 → Fade LED (PWM)
```

### ESP32 #3 - Communication Controller
```
ESP32 #3 Communication Connections:
├── Power
│   ├── VIN → 3.7V Battery (+)
│   └── GND → Battery (-) & Common Ground
├── MicroSD Card Module (SPI)
│   ├── VCC → ESP32 3.3V
│   ├── GND → ESP32 GND
│   ├── MISO → GPIO19
│   ├── MOSI → GPIO23
│   ├── SCK → GPIO18
│   └── CS → GPIO5
├── SIM800L GSM Module
│   ├── VCC → 3.7V Battery (+) [Direct connection]
│   ├── GND → ESP32 GND
│   ├── TXD → GPIO16 (ESP32 RX)
│   ├── RXD → GPIO17 (ESP32 TX)
│   └── RST → GPIO4
└── Status Indicators
    ├── GPIO2 → Status LED + 330Ω → GND
    └── Buzzer → GPIO13 + Transistor circuit
```

---

## 🔧 Assembly Steps

### Step 1: Power Distribution Setup
1. **Create Power Rails**
   ```
   Battery 1 → ESP32 #1 (Main Controller)
   Battery 2 → ESP32 #2 (Sensors) + AD8232 + BMP180 + Pulse Sensor
   Battery 3 → ESP32 #3 (Communication) + SIM800L
   ```

2. **Common Ground Connection**
   - Connect all GND pins together
   - Use breadboard or custom PCB for clean connections

### Step 2: ESP32 #1 Setup (Main Controller)
1. Mount ESP32 on breadboard/PCB
2. Connect power supply (3.7V battery)
3. Add status LED on GPIO2
4. Test power-on and WiFi functionality

### Step 3: ESP32 #2 Setup (Sensor Controller)
1. Mount ESP32 and sensors on breadboard
2. **AD8232 Connections:**
   - Carefully connect all pins as per diagram
   - Ensure proper shielding for clean ECG signals
   - Test with electrode pads
3. **BMP180 Connections:**
   - Standard I2C connection
   - Add pull-up resistors (4.7kΩ) on SDA/SCL if needed
4. **Pulse Sensor:**
   - Simple analog connection
   - Position sensor for proper finger placement

### Step 4: ESP32 #3 Setup (Communication)
1. **MicroSD Module:**
   - Use SPI connection
   - Format SD card as FAT32
   - Test read/write operations
2. **SIM800L Module:**
   - ⚠️ **Important:** SIM800L requires stable 3.7-4.2V supply
   - Connect directly to battery, not ESP32 3.3V rail
   - Add capacitors (100µF, 10µF) for power stability
   - Insert activated SIM card
   - Test network registration

### Step 5: Interconnection
1. **WiFi Communication Setup:**
   - ESP32 #1 creates Access Point "VitalCare-Rural"
   - ESP32 #2 and #3 connect as clients
   - Test HTTP communication between modules

2. **Data Flow Verification:**
   ```
   Sensors (ESP32 #2) → WiFi → Main Controller (ESP32 #1) → Web Interface
                                    ↓
                            Communication (ESP32 #3) → Local Storage + Cellular
   ```

---

## 📦 Enclosure Design

### Recommended Enclosure Specs
- **Material:** ABS plastic or 3D printed case
- **Dimensions:** 200mm x 150mm x 80mm
- **Features:**
  - Transparent top for status LEDs
  - Ventilation holes for heat dissipation
  - Access ports for charging and electrodes
  - Mounting points for sensors

### Layout Inside Enclosure
```
┌─────────────────────────────────┐
│  [Battery 1] [Battery 2] [Bat 3]│
│                                 │
│  [ESP32#1]   [ESP32#2]          │
│              + Sensors          │
│                                 │
│  [ESP32#3] + [SIM800L]          │
│  [SD Card]                      │
│                                 │
│  [Status LEDs] [Charging Port]  │
└─────────────────────────────────┘
```

---

## 🔍 Testing Procedures

### Power System Test
1. **Battery Voltage Check:**
   - Each battery should read 3.7V - 4.2V
   - Test under load conditions
   - Monitor for voltage drops during operation

2. **Current Consumption:**
   - ESP32 #1: ~150mA (WiFi AP mode)
   - ESP32 #2: ~120mA (sensor reading)
   - ESP32 #3: ~200mA (with SIM800L active)
   - Total: ~470mA typical

### Sensor Calibration
1. **AD8232 ECG:**
   - Test with proper electrode placement
   - Check for 50/60Hz noise filtering
   - Verify lead-off detection

2. **BMP180:**
   - Compare temperature with reference thermometer
   - Test altitude/pressure readings

3. **Pulse Sensor:**
   - Test with different finger pressures
   - Compare with reference pulse oximeter

### Communication Test
1. **WiFi Network:**
   - Verify Access Point creation
   - Test client connections
   - Check range and stability

2. **Cellular Module:**
   - Test SIM card recognition
   - Verify network registration
   - Test HTTP POST to test server

---

## ⚠️ Safety Considerations

### Electrical Safety
- **Patient Isolation:** AD8232 provides basic isolation, but add additional optocouplers for medical safety
- **Ground Loops:** Ensure single-point grounding
- **EMI Shielding:** Use shielded cables for ECG signals

### Battery Safety
- Use protection circuits for Li-ion batteries
- Monitor temperature during charging
- Include overcurrent protection

### Medical Safety
- **Not FDA Approved:** Clearly label as educational/demonstration device
- **Supervision Required:** Use only under qualified medical supervision
- **Emergency Procedures:** Include clear instructions for emergency situations

---

## 🛠️ Troubleshooting

### Common Issues
1. **ESP32 Won't Start:**
   - Check power supply voltage (3.0V - 3.6V)
   - Verify EN pin pull-up
   - Check for short circuits

2. **WiFi Connection Problems:**
   - Verify antenna connections
   - Check for interference
   - Monitor power supply stability

3. **Sensor Reading Issues:**
   - Verify wiring connections
   - Check power supply to sensors
   - Test sensor modules individually

4. **SIM800L Problems:**
   - Ensure stable 3.7V+ power supply
   - Check SIM card activation
   - Verify antenna connection
   - Monitor for overheating

### Debug Tools
- Use serial monitor for debugging
- Multimeter for voltage/current testing
- Oscilloscope for signal analysis (optional)
- Logic analyzer for I2C/SPI debugging (optional)

---

## 📈 Performance Optimization

### Power Optimization
- Use deep sleep modes when inactive
- Optimize sensor reading intervals
- Implement dynamic frequency scaling

### Signal Quality
- Use proper grounding techniques
- Add filtering capacitors where needed
- Shield sensitive analog inputs

### Reliability
- Implement watchdog timers
- Add error recovery mechanisms
- Use redundant sensors where possible

---

## 🔄 Maintenance

### Regular Checks
- Battery voltage and capacity
- Sensor calibration
- Connection integrity
- Software updates

### Component Replacement
- Battery life: ~2-3 years typical
- SD card: Monitor for wear
- Sensors: Calibrate annually
- SIM card: Check data plan status

---

*Assembly time: 4-6 hours for experienced builders*  
*Recommended for intermediate to advanced electronics enthusiasts*
