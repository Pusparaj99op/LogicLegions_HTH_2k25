# 🏥 VitalCare Rural - Portable Health Monitoring System

<div align="center">

![VitalCare Rural](https://img.shields.io/badge/VitalCare-Rural%20Health%20Monitoring-green?style=for-the-badge)
![ESP32](https://img.shields.io/badge/ESP32-WROOM--32-red?style=for-the-badge)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Development-yellow?style=for-the-badge)

*Bridging the healthcare gap in rural communities through innovative IoT technology*

</div>

## 🌟 Project Overview

**VitalCare Rural** is a comprehensive, portable health monitoring system designed specifically for rural clinics and underserved communities. This educational project addresses the critical shortage of diagnostic tools in remote areas by providing essential vital sign monitoring capabilities through affordable, locally-manufactured components.

### 🎯 Mission Statement
*"To democratize healthcare by making vital sign monitoring accessible, affordable, and reliable for rural communities worldwide."*

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

## 🛠️ Hardware Architecture

### 🎛️ **Component Distribution**

#### **Main Processing Unit - ESP32 #1**
```
📡 WiFi Access Point & Web Server
├── 🌐 Local Web Dashboard
├── 📱 Mobile Interface
├── 🔄 Real-time Data Broadcasting
└── 📊 Patient Data Management
```

#### **Sensor Hub - ESP32 #2**
```
🔬 Vital Signs Collection
├── ❤️ AD8232 Heart Monitor (ECG)
├── 🫁 Pulse Sensor (Heart Rate)
├── 🌡️ BMP180 (Temperature/Pressure)
└── 📡 Wireless Data Transmission
```

#### **Communication & Storage - ESP32 #3**
```
📡 Data Management & Sync
├── 💾 MicroSD Card Interface
├── 📱 SIM800L Cellular Module
├── ☁️ Cloud Synchronization
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
│  👤 Patient: John Doe     Age: 45  M    │
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
pio run --target upload --environment esp32-main
pio run --target upload --environment esp32-sensors
pio run --target upload --environment esp32-comm
```

### 🌐 **Network Setup**
1. Power on the VitalCare Rural system
2. Connect your mobile device to WiFi network: `VitalCare-Rural`
3. Open web browser and navigate to: `http://192.168.4.1`
4. Begin patient monitoring!

---

## 📁 Project Structure

```
VitalCare-Rural/
├── 📂 firmware/
│   ├── 🔧 esp32-main/           # Web server & interface
│   ├── 🔬 esp32-sensors/        # Sensor data collection
│   ├── 📡 esp32-communication/  # Data sync & storage
│   └── 🛠️ libraries/           # Custom libraries
├── 🌐 web-interface/
│   ├── 📄 index.html           # Main dashboard
│   ├── 🎨 styles.css           # UI styling
│   ├── ⚡ script.js            # Real-time updates
│   └── 📱 mobile.css           # Mobile optimization
├── 📊 data-schemas/
│   ├── 👤 patient.json         # Patient data structure
│   ├── 📈 vitals.json          # Vital signs format
│   └── 🗂️ session.json        # Session management
├── 📚 documentation/
│   ├── 🔧 hardware-guide.md    # Assembly instructions
│   ├── 💻 software-guide.md    # Programming guide
│   ├── 🏥 user-manual.md       # Clinical usage
│   └── 🔍 troubleshooting.md   # Common issues
└── 🧪 testing/
    ├── ⚡ unit-tests/          # Component testing
    ├── 🔗 integration-tests/   # System testing
    └── 🏥 field-tests/         # Real-world validation
```

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

### 📋 **Contribution Guidelines**
1. Fork the repository
2. Create a feature branch
3. Make your improvements
4. Test thoroughly
5. Submit a pull request

---

## 📞 Contact & Support

### 👥 **Development Team**
- 🏫 **Institution**: SDCE Nagpur - Hack The Hardware 2k25
- 📖 ** team Members**: Pranay Gajbhiye, Vineet mandhankar, abhijit Dohe, Soham Pise 
- 📧 **Contact**: [Pranaygajbhiye2020@gmail.com]
- 💬 **Discussion**: We Will Soon Create Discussion

<!-- ### 🆘 **Getting Help**
- 📖 Check the [Documentation](./documentation/)
- 🐛 Report issues on [GitHub Issues](https://github.com/Pusparaj99op/LogicLegions_HTH_2k25)
- 💭 Join discussions on [GitHub Discussions](https://github.com/Pusparaj99op/LogicLegions_HTH_2k25)
- 📹 Watch My Vlog Of HTH 2k25 At SDCE, Nagpur -->

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<div align="center">

### 🌟 Star this project if it helps bridge healthcare gaps! 🌟

Made with ❤️ for rural communities worldwide

**#HealthcareForAll #RuralInnovation #IoTForGood #HTH2025**

</div>

---

*Last Updated: August 19, 2025 | Version: 1.3.0*
