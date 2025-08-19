# VSCode Setup Guide for Arduino and ESP32 Development

This guide will help you set up Visual Studio Code for Arduino and ESP32 development on your system.

## Prerequisites

### System Requirements
- Visual Studio Code (latest version)
- Git (for version control)
- USB drivers for your development boards

### Supported Platforms
- Windows 10/11
- macOS (Intel and Apple Silicon)
- Linux (Ubuntu, Debian, Fedora, etc.)

## Installation Steps

### 1. Install Visual Studio Code
Download and install VSCode from: https://code.visualstudio.com/

### 2. Install Required Extensions
Open VSCode and install these extensions (or let VSCode install them automatically when you open this workspace):

#### Essential Extensions:
- **Arduino** (by Microsoft) - Arduino development support
- **C/C++** (by Microsoft) - C/C++ IntelliSense and debugging
- **C/C++ Extension Pack** (by Microsoft) - Additional C/C++ tools

#### ESP32 Development:
- **ESP-IDF** (by Espressif) - ESP32 development framework
- **PlatformIO IDE** (by PlatformIO) - Alternative development environment

#### Additional Helpful Extensions:
- **Serial Monitor** (by Microsoft) - Serial port communication
- **Better Comments** (by Aaron Bond) - Enhanced code comments
- **GitLens** (by GitKraken) - Git integration and history
- **Hex Editor** (by Microsoft) - View binary files

### 3. Install Arduino IDE or Arduino CLI

#### Option A: Arduino IDE (Recommended for beginners)
1. Download from: https://www.arduino.cc/en/software
2. Install following the platform-specific instructions
3. Launch Arduino IDE once to initialize directories

#### Option B: Arduino CLI (For advanced users)
```bash
# Windows (using winget)
winget install ArduinoSA.ArduinoCLI

# macOS (using Homebrew)
brew install arduino-cli

# Linux (using curl)
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```

### 4. Install ESP32 Board Package

#### Using Arduino IDE:
1. Open Arduino IDE
2. Go to File → Preferences
3. Add this URL to "Additional Boards Manager URLs":
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Go to Tools → Board → Boards Manager
5. Search for "ESP32" and install "ESP32 by Espressif Systems"

#### Using Arduino CLI:
```bash
arduino-cli core update-index --additional-urls https://dl.espressif.com/dl/package_esp32_index.json
arduino-cli core install esp32:esp32 --additional-urls https://dl.espressif.com/dl/package_esp32_index.json
```

### 5. Install USB Drivers

#### ESP32:
- **Windows**: Install CP210x USB driver from Silicon Labs
- **macOS**: Usually works out of the box (macOS 10.9+)
- **Linux**: Usually works out of the box with modern kernels

#### Arduino:
- **Windows**: Install drivers from Arduino website
- **macOS/Linux**: Usually works out of the box

### 6. Configure VSCode Workspace

1. Clone or download this repository
2. Open the folder in VSCode
3. VSCode will prompt you to install recommended extensions - click "Install All"
4. The workspace is now configured with optimal settings

## Board Configuration

### ESP32-WROOM-32 Settings:
- **Board**: ESP32 Dev Module
- **CPU Frequency**: 240MHz (WiFi/BT)
- **Flash Size**: 4MB
- **Flash Mode**: QIO
- **Flash Speed**: 80MHz
- **Upload Speed**: 921600
- **Port**: Select your COM/USB port

### Arduino Uno Settings:
- **Board**: Arduino Uno
- **Port**: Select your COM/USB port
- **Programmer**: AVRISP mkII

## Quick Start

### 1. First Arduino Project:
1. Navigate to `examples/arduino/blink_led/`
2. Open `blink_led.ino`
3. Select your Arduino board and port
4. Press `Ctrl+Shift+P` and run "Arduino: Upload"

### 2. First ESP32 Project:
1. Navigate to `examples/esp32/wifi_scanner/`
2. Open `wifi_scanner.ino`
3. Select ESP32 Dev Module board and port
4. Press `Ctrl+Shift+P` and run "Arduino: Upload"

## Workspace Structure

```
HTH-2025/
├── .vscode/                    # VSCode configuration
│   ├── extensions.json         # Recommended extensions
│   ├── settings.json          # Workspace settings
│   ├── c_cpp_properties.json  # C/C++ IntelliSense config
│   └── tasks.json             # Build and upload tasks
├── arduino_projects/          # Your Arduino projects
├── esp32_projects/           # Your ESP32 projects
├── libraries/                # Custom libraries
├── examples/                 # Example projects
│   ├── arduino/             # Arduino examples
│   └── esp32/               # ESP32 examples
├── docs/                    # Documentation
└── platformio.ini.template  # PlatformIO configuration template
```

## Using PlatformIO (Alternative)

### Installation:
1. Install PlatformIO IDE extension in VSCode
2. Copy `platformio.ini.template` to your project as `platformio.ini`
3. Modify the configuration for your specific board

### Benefits of PlatformIO:
- Advanced library management
- Multiple board support in one project
- Better dependency resolution
- Professional-grade build system

## Troubleshooting

### Common Issues:

#### 1. Board not detected:
- Check USB cable (data cable, not just charging)
- Install proper USB drivers
- Try different USB ports
- Check if board is in bootloader mode (ESP32)

#### 2. Compilation errors:
- Verify board selection matches your hardware
- Check library dependencies
- Ensure proper include paths

#### 3. Upload failures:
- Close Serial Monitor before uploading
- Check port selection
- Verify board is not in use by other applications
- Try lower upload speeds

#### 4. IntelliSense not working:
- Reload VSCode window (`Ctrl+Shift+P` → "Developer: Reload Window")
- Check `c_cpp_properties.json` configuration
- Verify include paths

### Getting Help:
1. Check this repository's Issues page
2. Arduino community forums
3. ESP32 documentation: https://docs.espressif.com/
4. VSCode Arduino extension documentation

## Advanced Configuration

### Custom Include Paths:
Edit `.vscode/c_cpp_properties.json` to add custom library paths:

```json
"includePath": [
    "${workspaceFolder}/**",
    "${workspaceFolder}/libraries/**",
    "path/to/your/custom/libraries/**"
]
```

### Custom Board Definitions:
Add custom board configurations in `c_cpp_properties.json` for specialized hardware.

### Serial Monitor Configuration:
Adjust serial monitor settings in `.vscode/settings.json`:

```json
"serialMonitor.baudRate": 115200,
"serialMonitor.lineEnding": "CRLF"
```

## Next Steps

1. Explore the example projects in the `examples/` folder
2. Create your first project in `arduino_projects/` or `esp32_projects/`
3. Check out the recommended libraries for sensors and connectivity
4. Join the IoT development community

Happy coding! 🚀