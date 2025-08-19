@echo off
echo ========================================
echo HTH-2025 IoT Development Setup
echo ========================================
echo.

echo Checking Python installation...
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Python not found! Please install Python 3.7+ from https://python.org
    pause
    exit /b 1
)
echo Python found!

echo.
echo Checking Git installation...
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Git not found! Please install Git from https://git-scm.com
    pause
    exit /b 1
)
echo Git found!

echo.
echo Installing PlatformIO Core...
pip install platformio
if %errorlevel% neq 0 (
    echo Failed to install PlatformIO. Please check your Python/pip installation.
    pause
    exit /b 1
)

echo.
echo Setting up PlatformIO...
pio upgrade
pio platform install espressif32
pio platform install atmelavr

echo.
echo Installing common libraries...
pio lib -g install "DHT sensor library"
pio lib -g install "ArduinoJson"
pio lib -g install "PubSubClient"
pio lib -g install "Adafruit SSD1306"

echo.
echo ========================================
echo Setup Complete!
echo ========================================
echo.
echo Your VS Code workspace is ready for IoT development with:
echo - Arduino Framework
echo - ESP32-WROOM-32 Support  
echo - PlatformIO IDE
echo - ESP-IDF Framework
echo - Serial Monitor
echo - Wokwi Simulator
echo.
echo To start developing:
echo 1. Open the HTH-2025.code-workspace file in VS Code
echo 2. Create a new PlatformIO project (Ctrl+Shift+P)
echo 3. Choose ESP32 Dev Module as your board
echo 4. Happy coding!
echo.
pause
