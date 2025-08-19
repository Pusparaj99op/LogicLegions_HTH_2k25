# HTH-2025 - IoT Development Workspace
Hack The Hardware At SDCE Nagpur

This workspace is configured for Arduino, ESP32-WROOM-32, and general IoT development.

## Installed Extensions

### Essential Extensions
- **PlatformIO IDE** - Complete IoT development platform
- **ESP-IDF** - Official Espressif development framework
- **Serial Monitor** - Monitor serial communication
- **Wokwi Simulator** - Hardware simulation for testing
- **Better C++ Syntax** - Enhanced C++ syntax highlighting
- **Serial Port Helper** - Serial port debugging tools

### Pre-installed Extensions
- **C/C++** - Microsoft C/C++ IntelliSense and debugging
- **CMake Tools** - CMake integration
- **Code Runner** - Quick code execution
- **GitHub Copilot** - AI-powered coding assistance

## Getting Started

### 1. PlatformIO Projects
Create a new PlatformIO project:
1. Press `Ctrl+Shift+P`
2. Type "PlatformIO: New Project"
3. Choose your board (e.g., ESP32 Dev Module)
4. Select framework (Arduino or ESP-IDF)

### 2. ESP-IDF Projects
Create ESP-IDF projects:
1. Press `Ctrl+Shift+P`
2. Type "ESP-IDF: New Project"
3. Choose template and configure

### 3. Serial Monitor
Access serial monitor:
- Use the Serial Monitor extension in the status bar
- Default baud rate: 115200
- Default port: COM3 (adjust in settings)

## Project Structure

```
HTH-2025/
├── .vscode/
│   ├── settings.json      # Workspace settings
│   ├── tasks.json         # Build and upload tasks
│   └── launch.json        # Debug configurations
├── examples/
│   ├── arduino/           # Arduino examples
│   ├── esp32/            # ESP32-specific examples
│   └── platformio/       # PlatformIO examples
├── libraries/            # Custom libraries
└── docs/                # Documentation
```

## Supported Boards

- ESP32-WROOM-32
- ESP32 DevKit
- ESP8266
- Arduino Uno/Nano/Mega
- STM32 boards
- And many more through PlatformIO

## Key Features

### Development
- IntelliSense for C/C++
- Syntax highlighting for Arduino (.ino files)
- Auto-completion and error detection
- Integrated debugging

### Build & Upload
- One-click build and upload
- Multiple environment support
- Automatic dependency management
- OTA (Over-the-Air) updates

### Monitoring
- Serial port monitoring
- Real-time data visualization
- Multiple port support
- Customizable baud rates

### Simulation
- Hardware simulation with Wokwi
- Test code without physical hardware
- Debug circuits and components

## Common Commands

| Action | Shortcut | Command Palette |
|--------|----------|----------------|
| Build | `Ctrl+Alt+B` | PlatformIO: Build |
| Upload | `Ctrl+Alt+U` | PlatformIO: Upload |
| Serial Monitor | `Ctrl+Alt+S` | PlatformIO: Serial Monitor |
| Clean | `Ctrl+Alt+C` | PlatformIO: Clean |

## Configuration Files

### platformio.ini Example
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps = 
    WiFi
    WebServer
```

### ESP-IDF Configuration
- Use `idf.py menuconfig` for configuration
- Modify `sdkconfig` file for custom settings

## Troubleshooting

### Common Issues
1. **Port not found**: Check device manager, update drivers
2. **Upload failed**: Hold boot button on ESP32 during upload
3. **Library not found**: Use PlatformIO library manager

### Serial Port Issues
- Ensure correct COM port in settings
- Check baud rate compatibility
- Verify cable connections

## Resources

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/)
- [Arduino Reference](https://www.arduino.cc/reference/)
- [Wokwi Projects](https://wokwi.com/)

## Support

For issues and questions:
1. Check the documentation
2. Use VS Code's integrated terminal for debugging
3. Consult the community forums
4. Use GitHub Copilot for coding assistance
