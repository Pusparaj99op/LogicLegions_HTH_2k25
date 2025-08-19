# 🔧 Hardware Assembly Guide - VitalCare Rural (Single ESP32 System)

## Overview
This guide provides step-by-step instructions for assembling the VitalCare Rural portable health monitoring system using a **single ESP32-WROOM-32** controller with all sensors integrated directly.

---

## 📋 Component List

### Required Components (Single ESP32 Configuration)
- **1x ESP32-WROOM-32** - Main and only controller
- **1x AD8232 Heart Rate Monitor** - ECG sensor module
- **1x BMP180 Sensor** - Temperature and atmospheric pressure
- **1x Pulse Sensor** - Heart rate detection sensor
- **1x SIM800L Module** - Cellular communication (SMS alerts)
- **1x MicroSD Card Adapter** - Local data storage
- **1x 3.7V 2000mAh Li-ion Battery** - Primary power supply
- **1x Buzzer (Active)** - Audio alert system
- **1x Breadboard or Custom PCB** - Component mounting
- **Jumper wires and connectors** - Electrical connections
- **Electrode pads** - For ECG measurements

### Available but Not Used (For Future Expansion)
- **4x Arduino Mega** - Available for distributed processing
- **2x Arduino Nano** - Available for auxiliary sensors
- **1x Arduino Uno** - Available for system monitoring
- **1x W1209 Digital Temperature Controller** - Environmental control
- **4x Relay Modules** - System switching
- **2x Vibrator Motors** - Tactile feedback
- **1x Accelerometer** - Motion detection

---

## 🔌 Complete Wiring Diagram - Single ESP32

### ESP32-WROOM-32 Pin Assignments
```
ESP32 Complete System Connections:
├── Power Supply
│   ├── VIN → 3.7V Li-ion Battery (+)
│   ├── GND → Battery (-) & Common Ground
│   ├── 3.3V → Sensor power distribution
│   └── EN → 10kΩ Pull-up to 3.3V
│
├── AD8232 ECG Sensor
│   ├── 3.3V → ESP32 3.3V
│   ├── GND → ESP32 GND
│   ├── OUTPUT → GPIO36 (VP - Analog Input)
│   ├── LO+ → GPIO18 (Leads Off Detection +)
│   ├── LO- → GPIO19 (Leads Off Detection -)
│   └── SDN → 3.3V (Shutdown - Always On)
│
├── BMP180 Temperature/Pressure Sensor (I2C)
│   ├── VCC → ESP32 3.3V
│   ├── GND → ESP32 GND
│   ├── SDA → GPIO21 (I2C Data)
│   └── SCL → GPIO22 (I2C Clock)
│
├── Pulse Sensor (Analog)
│   ├── RED → ESP32 3.3V
│   ├── BLACK → ESP32 GND
│   └── PURPLE → GPIO39 (VN - Analog Input)
│
├── MicroSD Card Module (SPI)
│   ├── VCC → ESP32 3.3V
│   ├── GND → ESP32 GND
│   ├── MISO → GPIO19 (Master In Slave Out)
│   ├── MOSI → GPIO23 (Master Out Slave In)
│   ├── SCK → GPIO18 (Serial Clock)
│   └── CS → GPIO5 (Chip Select)
│
├── SIM800L GSM Module
│   ├── VCC → 3.7V Battery (+) [DIRECT CONNECTION]
│   ├── GND → ESP32 GND
│   ├── TXD → GPIO16 (ESP32 RX2)
│   ├── RXD → GPIO17 (ESP32 TX2)
│   └── RST → GPIO4 (Reset Control)
│
├── Status and Alert System
│   ├── GPIO2 → Built-in LED (Status Indicator)
│   ├── GPIO4 → Buzzer + 100Ω Resistor → GND
│   └── GPIO15 → External Status LED + 330Ω → GND
│
└── Reserved for Future Expansion
    ├── GPIO12 → Available Digital I/O
    ├── GPIO13 → Available Digital I/O
    ├── GPIO14 → Available Digital I/O
    └── GPIO27 → Available Analog Input
```

---

## 📐 Breadboard Layout

### Component Placement Map
```
Breadboard Layout (Top View):
┌─────────────────────────────────────────────────┐
│ [+] Power Rail                           [-] GND │
│  │                                          │    │
│  ├── ESP32-WROOM-32 ──────────────────────── │  │
│  │   (Center mounted)                       │    │
│  │                                          │    │
│  ├── AD8232 ECG ────── BMP180 ─────────────── │  │
│  │   (Left side)      (I2C Sensors)         │    │
│  │                                          │    │
│  ├── Pulse Sensor ──── MicroSD Module ────── │  │
│  │   (Bottom left)    (SPI Interface)       │    │
│  │                                          │    │
│  ├── SIM800L Module ─────────────────────── │    │
│  │   (Bottom right - Direct battery power)  │    │
│  │                                          │    │
│  └── Buzzer + Status LEDs ──────────────────── │  │
│      (Top right corner)                     │    │
│                                                  │
│ [Battery Compartment Below]                      │
└─────────────────────────────────────────────────┘
```

---

## 🔧 Step-by-Step Assembly

### Step 1: Prepare the Workspace
1. **Gather Tools:**
   - Soldering iron and solder
   - Wire strippers
   - Multimeter
   - Small screwdrivers
   - Heat shrink tubing

2. **Safety Preparation:**
   - ESD wrist strap (recommended)
   - Well-lit workspace
   - Clean work surface

### Step 2: Power System Setup
1. **Battery Preparation:**
   ```
   Battery Connection:
   Battery (+) ─── VIN (ESP32)
        │
        └───── VCC (SIM800L) [Direct connection]
   
   Battery (-) ─── GND (ESP32) ─── Common Ground Rail
   ```

2. **Power Distribution:**
   - Connect 3.3V rail to all sensor modules
   - Ensure SIM800L gets direct battery voltage (3.7V)
   - Add capacitors: 100µF near SIM800L, 10µF near ESP32

### Step 3: Sensor Integration

#### AD8232 ECG Sensor
1. **Mount AD8232 module on breadboard**
2. **Wire connections carefully:**
   ```
   AD8232 → ESP32
   OUTPUT → GPIO36 (VP)
   LO+    → GPIO18
   LO-    → GPIO19
   3.3V   → 3.3V rail
   GND    → GND rail
   SDN    → 3.3V (keep enabled)
   ```
3. **Add electrode connections** with shielded wires

#### BMP180 Temperature/Pressure Sensor
1. **Mount BMP180 module**
2. **I2C Connection:**
   ```
   BMP180 → ESP32
   SDA    → GPIO21
   SCL    → GPIO22
   VCC    → 3.3V
   GND    → GND
   ```
3. **Add pull-up resistors** (4.7kΩ) on SDA and SCL lines

#### Pulse Sensor
1. **Mount pulse sensor module**
2. **Simple analog connection:**
   ```
   Pulse Sensor → ESP32
   PURPLE (Signal) → GPIO39 (VN)
   RED (Power)     → 3.3V
   BLACK (Ground)  → GND
   ```

### Step 4: Communication Modules

#### MicroSD Card Module
1. **Mount SD card module**
2. **SPI Connections:**
   ```
   SD Module → ESP32
   MISO      → GPIO19
   MOSI      → GPIO23
   SCK       → GPIO18
   CS        → GPIO5
   VCC       → 3.3V
   GND       → GND
   ```
3. **Insert formatted MicroSD card** (FAT32, 32GB max recommended)

#### SIM800L GSM Module
1. **⚠️ Critical: SIM800L Power Requirements**
   - Requires stable 3.7V - 4.2V supply
   - High current draw (up to 2A during transmission)
   - Connect directly to battery, NOT 3.3V rail

2. **SIM800L Connections:**
   ```
   SIM800L → ESP32/Battery
   VCC     → Battery (+) [Direct connection]
   GND     → ESP32 GND
   TXD     → GPIO16 (ESP32 RX2)
   RXD     → GPIO17 (ESP32 TX2)
   RST     → GPIO4
   ```

3. **Power Stabilization:**
   - Add 100µF capacitor between VCC and GND
   - Add 10µF ceramic capacitor in parallel
   - Use thick wires for power connections

### Step 5: Alert and Status System
1. **Buzzer Connection:**
   ```
   Buzzer (+) → GPIO4 → 100Ω resistor → Buzzer (-)
   Buzzer (-) → GND
   ```

2. **Status LEDs:**
   ```
   Built-in LED: GPIO2 (No external connection needed)
   External LED: GPIO15 → 330Ω resistor → LED (+) → LED (-) → GND
   ```

### Step 6: Final Assembly
1. **Secure all connections with solder**
2. **Add heat shrink tubing** to protect connections
3. **Test each component individually** before final integration
4. **Mount in protective enclosure**

---

## 📦 Enclosure Design

### Recommended Specifications
- **Material:** ABS plastic case or 3D printed enclosure
- **Internal Dimensions:** 180mm x 120mm x 60mm
- **Features Required:**
  - Ventilation holes for heat dissipation
  - Access port for electrode cables
  - Transparent window for status LEDs
  - Charging port access
  - Power switch mounting

### Internal Layout
```
Case Layout (Top View):
┌───────────────────────────────────────┐
│ [ECG Electrodes Port]    [Status LEDs]│
│                                       │
│ ┌─────────────────────────────────┐   │
│ │                                 │   │
│ │        ESP32 + Sensors          │   │
│ │        (Main Board)             │   │
│ │                                 │   │
│ └─────────────────────────────────┘   │
│                                       │
│ [SIM800L]              [Battery]      │
│ [+ Antenna]            [Compartment]  │
│                                       │
│ [Power Switch] [Charging Port]        │
└───────────────────────────────────────┘
```

---

## 🧪 Testing and Calibration

### Power System Test
1. **Battery Voltage:** 3.7V - 4.2V (full charge)
2. **System Current Draw:**
   - Idle: ~200mA
   - Active WiFi: ~350mA
   - With GSM active: ~500mA
   - During GSM transmission: Up to 2A peak
3. **Voltage Stability:** Monitor during GSM transmission

### Sensor Calibration

#### AD8232 ECG Testing
1. **Electrode Placement Test:**
   - Right arm to LA (Left Arm input)
   - Left arm to RA (Right Arm input) 
   - Right leg to RL (Right Leg/Ground)
2. **Signal Quality Check:**
   - Clean heartbeat waveform visible
   - Lead-off detection working
   - Minimal 50/60Hz interference

#### BMP180 Calibration
1. **Temperature Accuracy:**
   - Compare with calibrated thermometer
   - Should be within ±0.5°C
2. **Pressure Readings:**
   - Compare with local weather station
   - Verify sea-level compensation

#### Pulse Sensor Verification
1. **Heart Rate Accuracy:**
   - Compare with manual pulse count
   - Test with different finger pressures
   - Verify signal strength indicators

### System Integration Test
1. **WiFi Access Point:**
   - Verify "VitalCare-Rural" network creation
   - Test web interface access
   - Check real-time data updates

2. **Data Storage:**
   - Verify SD card write operations
   - Test data file creation
   - Check storage capacity monitoring

3. **SMS Alert System:**
   - Test SIM card recognition
   - Verify network registration
   - Send test SMS message

---

## ⚠️ Safety and Compliance

### Electrical Safety
- **Patient Safety:** This is an educational device only
- **Isolation:** AD8232 provides basic isolation but is not medical-grade
- **Ground Safety:** Single-point grounding to prevent ground loops
- **Overcurrent Protection:** Built into ESP32 and modules

### Medical Disclaimer
- **⚠️ NOT FDA APPROVED**
- **Educational Purpose Only**
- **Not for Medical Diagnosis**
- **Requires Qualified Supervision**
- **Emergency Procedures:** Include written emergency response plan

### Battery Safety
- Use protected Li-ion batteries only
- Monitor temperature during operation
- Include charging circuit with overcharge protection
- Never leave charging unattended

---

## 🔧 Troubleshooting Guide

### Common Issues and Solutions

#### ESP32 Won't Start
```
Symptoms: No WiFi network, no status LED
Solutions:
1. Check battery voltage (must be >3.2V)
2. Verify EN pin pull-up resistor
3. Check for short circuits
4. Press RESET button while powering on
```

#### Sensors Not Reading
```
Symptoms: Zero values or error messages
Solutions:
1. Verify all power connections
2. Check I2C pull-up resistors (BMP180)
3. Test each sensor individually
4. Check for loose connections
```

#### SIM800L Problems
```
Symptoms: No network registration, SMS fails
Solutions:
1. Verify direct battery connection (not 3.3V)
2. Check SIM card activation and plan
3. Ensure antenna is properly connected
4. Add larger capacitors for power stability
5. Check for overheating
```

#### Web Interface Issues
```
Symptoms: Can't connect to dashboard
Solutions:
1. Verify WiFi network "VitalCare-Rural" is visible
2. Try connecting to IP: 192.168.4.1 directly
3. Check ESP32 serial output for errors
4. Restart ESP32 and try again
```

### Debug Tools and Techniques
- **Serial Monitor:** Use 115200 baud for debugging output
- **Multimeter:** Essential for voltage/current measurements
- **WiFi Scanner:** Verify network is being broadcast
- **Test Equipment:** Simple pulse oximeter for comparison

---

## 📊 Performance Specifications

### System Capabilities
- **Heart Rate Range:** 40-200 BPM
- **Temperature Range:** 95°F - 104°F (35°C - 40°C)
- **Pressure Range:** 950-1050 hPa
- **ECG Resolution:** 12-bit ADC (4096 levels)
- **Update Rate:** 1 Hz (1 second intervals)
- **WiFi Range:** ~30 meters (open space)
- **Battery Life:** 8-12 hours continuous operation

### Data Storage
- **SD Card Support:** Up to 32GB FAT32
- **Data Format:** CSV files with timestamps
- **Storage Rate:** ~1KB per minute per patient
- **Retention:** Limited only by SD card capacity

---

## 🔄 Maintenance Schedule

### Daily Checks (If Used Regularly)
- Battery charge level
- Sensor cleanliness
- LED status indicators
- Basic functionality test

### Weekly Maintenance
- Clean electrode pads
- Check all connections
- Verify data storage
- Test SMS alert function

### Monthly Maintenance
- Calibrate sensors with references
- Update firmware if available
- Check SIM card data usage
- Inspect for physical damage

### Annual Service
- Replace electrode pads
- Professional sensor calibration
- Battery capacity test
- Complete system documentation review

---

## 📝 Documentation and Records

### Required Documentation
- Assembly photos and notes
- Calibration certificates
- User manual and safety instructions
- Emergency contact procedures
- Maintenance log

### Recommended Testing Log
```
Date: ___________
Tester: _________________
Battery Voltage: _________V
WiFi Signal: ____________
Temperature Cal: ________°F
Heart Rate Test: ________BPM
SMS Test: ______________
Notes: _________________
```

---

*Assembly Time: 6-8 hours for complete system*  
*Skill Level: Intermediate electronics experience required*  
*Cost: Approximately $150-200 for components*

**🎓 Educational Project - SDCE Nagpur HTH 2025**
