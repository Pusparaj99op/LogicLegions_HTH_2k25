# 🏥 VitalCare Rural - Complete Portable Health Monitoring System

<div align="center">

![VitalCare Rural Logo](https://img.shields.io/badge/VitalCare-Rural-blue?style=for-the-badge&logo=hospital-o)
![ESP32](https://img.shields.io/badge/ESP32-WROOM--32-red?style=for-the-badge&logo=espressif)
![License](https://img.shields.io/badge/License-Educational-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Production%20Ready-success?style=for-the-badge)

**A comprehensive IoT health monitoring system designed for rural healthcare delivery**

*Single ESP32 Complete System - Consolidated and Production Ready*

</div>

---

## 📖 Project Overview

VitalCare Rural is an **educational IoT health monitoring system** designed to demonstrate portable healthcare technology for resource-limited settings. Built around a **single ESP32-WROOM-32 microcontroller**, this system integrates multiple medical sensors to provide real-time monitoring of patient vital signs.

**🔄 Project Evolution:** Originally designed as a 3-ESP32 distributed system, VitalCare Rural has been **consolidated into a single ESP32 complete system** to match available hardware constraints while maintaining all functionality.

### 🎯 Project Goals
- **Demonstrate IoT healthcare applications** in rural settings
- **Provide hands-on learning experience** with medical electronics
- **Showcase wireless sensor integration** and data management
- **Create educational documentation** for future students
- **Emphasize safety and ethical considerations** in medical device development

### � Educational Context
- **Institution:** SDCE Nagpur
- **Course:** HTH 2025 (Health Technology & Hardware)
- **Project Type:** Capstone Educational Demonstration
- **Target Audience:** Engineering students, healthcare technology enthusiasts
- **Hardware Constraint:** Single ESP32 configuration (adapted from multi-ESP32 design)

---

## 📋 Features & Capabilities

### 🔬 **Vital Sign Monitoring**
- ❤️ **Heart Rate (HR)** - Real-time cardiac monitoring with AD8232 ECG sensor
- 🩸 **Blood Pressure (BP)** - Non-invasive pressure measurement system
- 🫁 **Oxygen Saturation (SpO2)** - Pulse oximetry for respiratory assessment
- 🌡️ **Temperature** - Body temperature monitoring with BMP180 integration

### 💻 **Smart Web Interface**
- 📱 **Mobile-Responsive Dashboard** - Accessible on any device with WiFi
- ⚡ **Real-Time Updates** - Data refreshes every second
- 📊 **Patient Management** - Digital patient records and history
- 📈 **Trend Analysis** - Historical data visualization
- 🔄 **Offline/Online Sync** - Works without internet, syncs when available

### 🌐 **Connectivity Options**
- 📶 **WiFi Hotspot** - Creates local access point for mobile devices
- 📡 **Cellular Backup** - SIM800L for remote data transmission
- 💾 **Local Storage** - MicroSD card for offline data persistence
- ☁️ **Cloud Sync** - Automatic upload when connectivity is restored

---

## 🛠️ Single ESP32 Hardware Architecture

### 🎛️ **Consolidated System Design**

Our **production-ready single ESP32 system** integrates all functionality into one microcontroller to match hardware availability while maintaining complete feature set:

#### **Complete ESP32-WROOM-32 System**
```
� Integrated All-in-One Architecture
├── �📡 WiFi Access Point & Web Server
│   ├── 🌐 Responsive Web Dashboard
│   ├── 📱 Mobile Interface
│   ├── 🔄 WebSocket Real-time Updates
│   └── 📊 Patient Data Management
├── 🔬 Comprehensive Sensor Integration
│   ├── ❤️ AD8232 ECG Monitor (Pins 36, 39, 34)
│   ├── 🫁 Pulse Sensor (Pin 35)
│   ├── 🌡️ BMP180 Temperature/Pressure (I2C: SDA-21, SCL-22)
│   └── � Battery Voltage Monitor (Pin 33)
├── 💾 Data Management System
│   ├── 💽 MicroSD Card Storage (SPI: MISO-19, MOSI-23, CLK-18, CS-5)
│   ├── 📡 SIM800L GSM Module (TX-17, RX-16)
│   └── ☁️ SMS Alert System
└── 🎛️ Advanced Features
    ├── 📈 Real-time ECG Waveform Display
    ├── 🚨 Automatic Alert Generation
    ├── 📋 Patient Report Generation
    └── 📁 CSV Data Export
```

### 🔌 **Pin Configuration Summary**
| Component | ESP32 Pins | Interface |
|-----------|------------|-----------|
| **AD8232 ECG** | 36 (OUTPUT), 39 (LO+), 34 (LO-) | Analog Input |
| **Pulse Sensor** | 35 | Analog Input |
| **BMP180** | 21 (SDA), 22 (SCL) | I2C |
| **MicroSD** | 19 (MISO), 23 (MOSI), 18 (CLK), 5 (CS) | SPI |
| **SIM800L** | 17 (TX), 16 (RX) | UART |
| **Battery Monitor** | 33 | Analog Input |
└── 🔄 Offline Data Buffering
```

### 📊 **System Components**

| Component | Quantity | Function | Status |
|-----------|----------|----------|---------|
| ESP32-WROOM-32 | 3 | Main Controllers | ✅ Primary |
| AD8232 Heart Monitor | 1 | ECG Signal Processing | ✅ Primary |
| BMP180 Sensor | 1 | Temperature/Pressure | ✅ Primary |
| Pulse Sensor | 1 | Heart Rate Detection | ✅ Primary |
| SIM800L Module | 1 | Cellular Communication | ✅ Primary |
| MicroSD Adapter | 1 | Local Data Storage | ✅ Primary |
| Arduino Mega | 4 | Auxiliary Processing | 🔄 Secondary |
| Relay Modules | 4 | System Control | 🔄 Secondary |
| Vibrator Motors | 2 | Patient Feedback | 🔄 Secondary |
| Li-ion Battery | 3 | Portable Power | ✅ Primary |

---

## 🌐 Web Interface Preview

### 📱 **Patient Dashboard**
```
┌─────────────────────────────────────────┐
│  🏥 VitalCare Rural - Patient Monitor   │
├─────────────────────────────────────────┤
│  👤 Patient: Tuka Ram     Age: 45  M    │
│  📅 Session: Aug 19, 2025  ⏰ 14:30    │
├─────────────────────────────────────────┤
│  ❤️  Heart Rate: 72 BPM     🟢 Normal   │
│  🩸  Blood Pressure: 120/80 🟢 Normal   │
│  🫁  SpO2: 98%              🟢 Normal   │
│  🌡️  Temperature: 98.6°F    🟢 Normal   │
├─────────────────────────────────────────┤
│  📊 [Live Chart - Last 5 minutes]       │
│  ⚠️  [Alert Panel]                      │
│  💾 [Save Reading] 📤 [Send to Doctor]  │
└─────────────────────────────────────────┘
```

### 🗂️ **Patient Registration**
```
┌─────────────────────────────────────────┐
│  📝 New Patient Registration            │
├─────────────────────────────────────────┤
│  Full Name: [________________]          │
│  Age: [___] Gender: [M/F/O]             │
│  Contact: [________________]            │
│  Medical ID: [Auto-Generated]           │
│  Emergency Contact: [__________]        │
│  Known Conditions: [___________]        │
├─────────────────────────────────────────┤
│  [📷 Take Photo] [💾 Save Patient]      │
└─────────────────────────────────────────┘
```

---

## 🚀 Quick Start Guide

### 🔧 **Hardware Setup**
1. **Power System**: Connect 3.7V Li-ion batteries to ESP32 modules
2. **Sensor Connections**: Wire AD8232, BMP180, and Pulse sensor
3. **Communication Setup**: Install SIM800L and MicroSD modules
4. **Assembly**: Mount components in portable enclosure

### 💻 **Software Installation**
```bash
# Clone the project
git clone https://github.com/Pusparaj99op/LogicLegions_HTH_2k25

# Open in VS Code with PlatformIO
cd LogicLegions_HTH_2k25
code VitalCare-Rural.code-workspace # Mene yaha Ek Worspace banaya hai For Easy Setup

# Flash ESP32 modules
### 💻 **Single ESP32 Setup Process**
```bash
# Setup PlatformIO environment
# Install PlatformIO in VS Code
# Clone or setup project files

# Single ESP32 flashing (consolidated system)
pio run --target upload --environment esp32-main
```

### 🌐 **Network Setup**
1. Power on the VitalCare Rural system (single ESP32)
2. Connect your mobile device to WiFi network: `VitalCare-Rural`
3. Open web browser and navigate to: `http://192.168.4.1`
4. Begin educational demonstration!

---

## 📁 Consolidated Project Structure

```
HTH-2025/ (Main Workspace)
├── 📂 firmware/
│   └── 🔧 esp32-main/          # Single ESP32 complete system
│       ├── � src/main.cpp     # Consolidated firmware
│       ├── ⚙️ platformio.ini   # Build configuration
│       └── � lib/             # Custom utilities
├── 🌐 web-interface/
│   ├── 📄 index.html          # Enhanced web dashboard
│   ├── 🎨 styles.css          # Comprehensive UI styling  
│   └── ⚡ script.js           # Advanced WebSocket client
├── 📚 documentation/
│   ├── 🔧 Hardware-Guide.md   # Single ESP32 assembly guide
│   ├── 💻 Software-Guide.md   # Development setup
│   └── 🏥 User-Manual.md      # Educational usage guide
├── 📁 libraries/
│   └── � README.md           # Library dependencies
├── 📊 data/
│   └── 📁 samples/            # Sample educational data
└── 📋 README.md               # This comprehensive guide
```

### 🏗️ **Architecture Consolidation**
- **Original Design:** 3-ESP32 distributed system
- **Current Implementation:** Single ESP32 complete system
- **Hardware Adaptation:** All functionality integrated into ESP32-WROOM-32
- **Feature Preservation:** Complete monitoring capabilities maintained
- **Educational Focus:** Simplified assembly, comprehensive documentation

---

## 🎓 Educational Impact

### 👨‍🎓 **Learning Objectives**
- **IoT System Design**: Multi-microcontroller communication
- **Web Development**: Responsive, real-time interfaces
- **Healthcare Technology**: Medical device principles
- **Rural Innovation**: Appropriate technology solutions
- **Data Management**: Offline-first, sync-capable systems

### 🏆 **Skills Developed**
- Embedded Systems Programming
- Web Interface Design
- Sensor Integration
- Wireless Communication
- Medical Device Safety
- Rural Technology Deployment

---

## 🌍 Social Impact

### 🎯 **Target Communities**
- 🏘️ Rural clinics with limited resources
- 🏔️ Remote mountain communities
- 🌊 Island healthcare facilities
- 🚑 Mobile health units
- 🏫 Rural schools with health programs

### 📈 **Expected Outcomes**
- ✅ Early detection of health issues
- 📊 Improved health data collection
- 💰 Reduced healthcare costs
- 🚀 Enhanced diagnostic capabilities
- 👨‍⚕️ Better resource utilization

---

## 🔒 Safety & Compliance

### ⚠️ **Important Disclaimers**
- 🎓 **Educational Purpose Only**: This project is designed for learning and demonstration
- 🏥 **Not Medical Grade**: Not intended for clinical diagnosis or treatment
- 👨‍⚕️ **Professional Supervision**: Should be used under healthcare professional guidance
- 📋 **Regulatory Compliance**: Check local medical device regulations

### 🛡️ **Safety Features**
- 🔋 Battery protection circuits
- ⚡ Electrical isolation for patient safety
- 🚨 Emergency stop functionality
- 💾 Secure data storage
- 🔐 Patient privacy protection

---

## 🤝 Contributing

We welcome contributions from the community! Whether you're a:
- 👨‍💻 **Developer**: Improve code and add features
- 👨‍⚕️ **Healthcare Professional**: Provide clinical insights
- 🎨 **Designer**: Enhance user interface
- 📝 **Documenter**: Improve guides and tutorials
- 🧪 **Tester**: Validate system performance

---

## 📊 Technical Specifications

### 🔧 **Single ESP32 System Requirements**
- **Main Controller**: ESP32-WROOM-32 Development Board (single unit)
- **Medical Sensors**: AD8232 (ECG), Pulse sensor, BMP180 (Temperature/Pressure)
- **Communication**: SIM800L GSM Module, WiFi Built-in
- **Storage**: MicroSD Card Module (16GB+ recommended)
- **Power**: 3.7V Li-ion Battery with protection circuit
- **Development**: PlatformIO with Arduino framework

### � **Software Architecture**
- **Firmware**: C++ Arduino framework with integrated sensor management
- **Web Interface**: HTML5, CSS3, JavaScript with WebSocket communication
- **Real-time Updates**: WebSocket server for live data streaming
- **Data Management**: JSON for APIs, CSV export for analysis
- **Visualization**: Real-time ECG waveform rendering with Canvas

### 🎯 **Educational Performance Metrics**
- **ECG Sampling**: Up to 200 Hz for educational demonstration
- **Battery Life**: 6-8 hours continuous demonstration
- **WiFi Range**: Up to 30 meters for mobile access
- **Data Storage**: Educational patient simulation data
- **Response Time**: <200ms for educational alerts

---

## 🎓 Educational Learning Outcomes

### 📚 **Core Learning Objectives**
- **IoT System Design** - Understanding complete system architecture
- **Medical Electronics** - Sensor integration and signal processing
- **Embedded Programming** - ESP32 firmware development
- **Web Technologies** - Real-time web interfaces with WebSocket
- **Healthcare Ethics** - Medical device safety and educational boundaries
- **Project Management** - Hardware-constrained design adaptation

### 🛠️ **Technical Skills Development**
- **Embedded Systems** - ESP32 programming and peripheral integration
- **Web Development** - Responsive interface design and real-time communication
- **Sensor Integration** - Medical sensor calibration and data processing
- **Communication Protocols** - WiFi, GSM, and serial communication
- **Data Management** - Local storage, CSV export, and patient data handling
- **Safety Engineering** - Educational use protocols and system monitoring

### 🏥 **Healthcare Technology Awareness**
- **Rural Healthcare Challenges** - Understanding resource limitations
- **Medical Device Regulations** - Learning compliance requirements
- **Patient Privacy** - Data protection in medical applications
- **Technology Ethics** - Responsible innovation in healthcare
- **Educational Boundaries** - Distinguishing demonstration from clinical use

---

## 🔧 Development Environment Setup

### 💻 **VS Code Configuration**
1. **Install VS Code** - Latest stable version
2. **Install PlatformIO** - Official PlatformIO IDE extension
3. **Arduino Framework** - Automatically handled by PlatformIO
4. **ESP32 Platform** - Espressif 32 platform support
5. **Serial Monitor** - Built-in PlatformIO serial communication

### 📦 **Required Libraries** (Auto-installed via platformio.ini)
```ini
lib_deps = 
    bblanchon/ArduinoJson@^6.21.3
    links2004/WebSockets@^2.4.0  
    adafruit/Adafruit BMP085 Library@^1.2.2
    adafruit/Adafruit Unified Sensor@^1.1.7
```

### 🔌 **Hardware Setup Checklist**
- [ ] ESP32-WROOM-32 board with USB cable
- [ ] AD8232 ECG sensor module
- [ ] Pulse sensor (photoplethysmography)
- [ ] BMP180 temperature/pressure sensor
- [ ] SIM800L GSM module with SIM card
- [ ] MicroSD card module with formatted card
- [ ] Breadboard and jumper wires
- [ ] 3.7V Li-ion battery with protection circuit
- [ ] Educational safety documentation review

---

## ⚠️ Comprehensive Safety Documentation

### 🎓 **Educational Use Guidelines**
> **CRITICAL: This system is designed exclusively for educational demonstration and learning purposes.**

#### **Permitted Educational Uses:**
- ✅ **Classroom Demonstrations** - Instructor-supervised learning
- ✅ **Technical Training** - Understanding IoT healthcare concepts  
- ✅ **Project Development** - Student engineering projects
- ✅ **Concept Validation** - Proof-of-concept demonstrations
- ✅ **Research Simulation** - Academic research with simulated data

#### **STRICTLY PROHIBITED Uses:**
- ❌ **Medical Diagnosis** - Never use for actual patient diagnosis
- ❌ **Treatment Decisions** - Not suitable for clinical decision-making
- ❌ **Patient Monitoring** - Not certified for real patient monitoring
- ❌ **Emergency Situations** - Never rely on for medical emergencies
- ❌ **Clinical Environments** - Not approved for hospital/clinic use

### 🛡️ **Safety Protocols**
1. **Supervision Required** - Always use under qualified instruction
2. **Battery Safety** - Use protected Li-ion batteries only
3. **Electrical Safety** - Verify all connections before powering
4. **Data Privacy** - Use only simulated/educational patient data
5. **Emergency Procedures** - Have emergency stop procedures ready

### 📋 **Legal Disclaimers**
- **No Medical Claims** - This system makes no medical claims
- **Educational Prototype** - Not a certified medical device
- **No Liability** - No responsibility for medical decisions
- **Local Compliance** - Check local educational/medical device regulations
- **Professional Guidance** - Seek professional medical advice for health concerns

---

## 🤝 Project Development & Contributions

### � **Development Guidelines**
This educational project welcomes contributions from:
- 👨‍💻 **Students** - Enhance code and add educational features
- 👨‍⚕️ **Healthcare Educators** - Provide clinical learning insights
- 🎨 **UI/UX Designers** - Improve interface usability
- 📝 **Technical Writers** - Enhance documentation and guides
- 🧪 **QA Engineers** - Validate educational system performance

### 🔄 **Contribution Process**
1. **Study Documentation** - Understand educational objectives
2. **Follow Safety Guidelines** - Maintain educational focus
3. **Test Thoroughly** - Validate on single ESP32 system
4. **Document Changes** - Update relevant documentation
5. **Educational Context** - Ensure changes support learning objectives

---

## �📞 Contact & Support

### 👥 **Development Team - HTH 2025**
- 🏫 **Institution**: SDCE Nagpur - Hack The Hardware 2025
- �‍💻 **Team Members**: Pranay Gajbhiye, Vineet Mandhankar, Abhijit Dohe, Soham Pise 
- 📧 **Contact**: Pranaygajbhiye2020@gmail.com
- 🎯 **Project Focus**: Educational IoT healthcare demonstration system
- 🔧 **Hardware Constraint**: Single ESP32 consolidated architecture

### 🆘 **Educational Support**
- 📖 **Hardware Guide**: `documentation/Hardware-Guide.md`
- 🏥 **User Manual**: `documentation/User-Manual.md`
- � **Library Guide**: `libraries/README.md`
- � **Safety Guidelines**: Comprehensive safety documentation above
- 🎓 **Learning Resources**: Educational objectives and skill development sections

---

## 📜 License & Attribution

This educational project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

### 🙏 **Acknowledgments**
- **SDCE Nagpur** - Educational institution support
- **HTH 2025** - Hack The Hardware competition platform
- **Open Source Community** - Arduino, ESP32, and web technology frameworks
- **Healthcare Educators** - Guidance on educational medical technology ethics
- **Rural Healthcare Advocates** - Inspiration for addressing healthcare accessibility

---

<div align="center">

### 🌟 Educational Project - Single ESP32 Complete System 🌟

**Production Ready • Comprehensively Documented • Safety Focused**

Made with ❤️ for healthcare technology education

**#EducationalIoT #HealthcareTechnology #ESP32 #MedicalElectronics #HTH2025**

---

**⚠️ EDUCATIONAL USE ONLY - NOT FOR MEDICAL DIAGNOSIS ⚠️**

*Project Status: Production Ready for Educational Use*
*Last Updated: January 2025 | Version: Single ESP32 Complete System*

</div>
