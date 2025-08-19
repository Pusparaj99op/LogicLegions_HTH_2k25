# Examples

This folder contains example projects to help you learn Arduino and ESP32 development.

## 📁 Structure

```
examples/
├── arduino/          # Arduino board examples
│   ├── blink_led/    # Basic LED control
│   └── ...           # More examples coming soon
├── esp32/            # ESP32 examples  
│   ├── wifi_scanner/ # WiFi network scanning
│   └── ...           # More examples coming soon
└── README.md         # This file
```

## 🎯 Getting Started

### For Beginners:
1. Start with `arduino/blink_led/` - Learn basic Arduino structure
2. Try `esp32/wifi_scanner/` - Explore ESP32 WiFi capabilities  
3. Modify the examples to experiment and learn

### For Each Example:
1. **Read the code comments** - They explain what each part does
2. **Try the project as-is** - Get it working first
3. **Modify and experiment** - Change values, add features
4. **Create your own version** - Apply what you learned

## 🚀 Available Examples

### Arduino Examples:

#### `blink_led/`
- **Difficulty**: Beginner
- **Hardware**: Any Arduino board
- **Concepts**: Digital output, timing, serial communication
- **Description**: Classic "Hello World" - makes the built-in LED blink

**What you'll learn:**
- Basic Arduino sketch structure
- Using `pinMode()`, `digitalWrite()`, `delay()`
- Serial monitor for debugging

### ESP32 Examples:

#### `wifi_scanner/` 
- **Difficulty**: Beginner
- **Hardware**: ESP32-WROOM-32 or ESP32 DevKit
- **Concepts**: WiFi scanning, network analysis, ESP32 features
- **Description**: Scans and displays available WiFi networks

**What you'll learn:**
- ESP32 WiFi library usage
- Network scanning and analysis
- Serial output formatting
- ESP32 security type detection

## 📚 Planned Examples

### Coming Soon:

#### Arduino:
- **Button LED Control** - Digital input with pull-up resistors
- **Analog Sensor Reading** - Reading analog values from sensors  
- **Servo Motor Control** - PWM control of servo motors
- **LCD Display** - Displaying text on LCD screens
- **Temperature Sensor** - Reading DHT22 temperature/humidity

#### ESP32:
- **WiFi Connection** - Connecting to WiFi networks
- **Web Server** - Creating web interfaces
- **Bluetooth Scanner** - BLE device discovery
- **HTTP Client** - Making web requests
- **MQTT Client** - IoT communication protocol
- **Deep Sleep** - Power management

#### Advanced:
- **IoT Sensor Node** - Complete sensor-to-cloud project
- **Home Automation** - Web-controlled devices
- **Security Camera** - ESP32-CAM projects
- **Weather Station** - Multi-sensor environmental monitoring

## 🛠 How to Use Examples

### Method 1: Copy and Modify
```bash
# Copy example to your projects folder
cp -r examples/arduino/blink_led arduino_projects/my_blink_project
# Modify as needed
```

### Method 2: Direct Development
- Open example directly in VSCode
- Make changes and experiment
- Save your modifications

### Method 3: Learning Template
- Study the example code
- Understand the concepts
- Create your own implementation from scratch

## 📖 Learning Path

### Beginner Path:
1. **blink_led** → Understand Arduino basics
2. **wifi_scanner** → Learn ESP32 capabilities  
3. **button_control** → Digital input concepts
4. **sensor_reading** → Analog input and sensors

### Intermediate Path:
1. **web_server** → Network programming
2. **mqtt_client** → IoT communication
3. **multi_sensor** → Complex sensor projects
4. **home_automation** → System integration

### Advanced Path:
1. **iot_platform** → Cloud integration
2. **security_system** → Real-world applications
3. **custom_protocols** → Advanced communication
4. **optimization** → Performance and power

## 🎓 Educational Features

Each example includes:
- **Detailed comments** explaining every important line
- **Hardware requirements** list
- **Wiring diagrams** (where applicable)
- **Expected output** descriptions
- **Modification suggestions** for learning
- **Troubleshooting tips** for common issues

## 🤝 Contributing Examples

Want to add your own examples? Great! Please:

1. **Follow the structure**:
   ```
   examples/category/project_name/
   ├── project_name.ino
   ├── README.md
   ├── circuit_diagram.png (if applicable)
   └── expected_output.txt
   ```

2. **Include documentation**:
   - Clear comments in code
   - README with description and requirements
   - Expected output examples

3. **Test thoroughly**:
   - Verify on actual hardware
   - Test with different board variants
   - Include troubleshooting notes

## 🆘 Getting Help

### If an example doesn't work:
1. **Check hardware connections** - Verify wiring
2. **Verify board selection** - Ensure correct board type
3. **Check serial monitor** - Look for error messages
4. **Review requirements** - Confirm all prerequisites
5. **Ask for help** - Create an issue in this repository

### Common Issues:
- **Upload failures** - Check board and port selection
- **Compilation errors** - Verify library installations
- **No output** - Check baud rate and connections
- **Unexpected behavior** - Review power and wiring

## 🎯 Next Steps

After trying the examples:
1. **Create your own projects** in the project folders
2. **Combine concepts** from multiple examples
3. **Add new features** to existing examples
4. **Share your creations** with the community

Remember: **The best learning happens through experimentation!** 

Don't be afraid to:
- Change values and see what happens
- Break things (safely) and fix them
- Ask "what if I..." questions
- Build upon the examples

**Happy Learning! 🎓🚀**