/*
 * VitalCare Rural - Sensor ESP32 Module
 *
 * This ESP32 module handles all sensor data collection for the VitalCare Rural system.
 * It interfaces with medical sensors to collect vital signs and transmits the data
 * to the main ESP32 controller.
 *
 * Sensors Supported:
 * - AD8232 Heart Rate Monitor (ECG)
 * - Pulse Sensor for Heart Rate detection
 * - BMP180 for Temperature and Pressure
 * - Future: SpO2 sensor integration
 *
 * Hardware: ESP32-WROOM-32 #2 (Sensor Controller)
 * Author: VitalCare Rural Team
 * Date: August 2025
 * Educational Purpose Only - Not for Medical Use
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Pin Definitions
#define AD8232_LO_PLUS_PIN 18  // AD8232 Leads-off detection +
#define AD8232_LO_MINUS_PIN 19 // AD8232 Leads-off detection -
#define AD8232_OUTPUT_PIN A0   // AD8232 analog output
#define PULSE_SENSOR_PIN A3    // Pulse sensor analog input
#define PULSE_BLINK_PIN 2      // LED for pulse indication
#define PULSE_FADE_PIN 5       // LED PWM for fade effect

// Network Configuration
const char *MAIN_CONTROLLER_IP = "192.168.4.1";
const int MAIN_CONTROLLER_PORT = 80;
const char *WIFI_SSID = "VitalCare-Rural";
const char *WIFI_PASSWORD = "VitalCare2025";

// Sensor Objects
Adafruit_BMP085 bmp180;

// Sensor Data Structure
struct SensorData
{
  float heartRateECG;    // From AD8232
  float heartRatePulse;  // From Pulse Sensor
  float temperature;     // From BMP180
  float pressure;        // From BMP180 (for future BP calculation)
  float spO2;            // Future implementation
  bool sensorsConnected; // Connection status
  unsigned long timestamp;
};

// Global Variables
SensorData currentSensorData;
unsigned long lastSensorRead = 0;
unsigned long lastDataTransmission = 0;
const unsigned long SENSOR_READ_INTERVAL = 500; // Read sensors every 500ms
const unsigned long DATA_SEND_INTERVAL = 1000;  // Send data every 1 second

// Pulse Detection Variables
int pulseSignal;
int threshold = 2048; // Adjust based on your pulse sensor
int BPM;
bool pulseDetected = false;
unsigned long lastBeat = 0;
int beatCount = 0;
unsigned long beatStartTime = 0;

// ECG Processing Variables
int ecgSignal;
bool leadsConnected = true;
float ecgHeartRate = 0;
int ecgBeatCount = 0;
unsigned long ecgBeatTime = 0;
int ecgThreshold = 2000; // Adjust based on AD8232 output

// Function Prototypes
void setupWiFi();
void setupSensors();
void readAD8232();
void readPulseSensor();
void readBMP180();
void calculateHeartRates();
void sendSensorData();
void blinkHeartbeat();
bool connectToMainController();

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("======================================");
  Serial.println("🔬 VitalCare Rural - Sensor Module");
  Serial.println("======================================");

  // Initialize pins
  pinMode(AD8232_LO_PLUS_PIN, INPUT);
  pinMode(AD8232_LO_MINUS_PIN, INPUT);
  pinMode(PULSE_BLINK_PIN, OUTPUT);
  pinMode(PULSE_FADE_PIN, OUTPUT);

  // Setup sensors
  setupSensors();

  // Setup WiFi connection
  setupWiFi();

  // Initialize sensor data
  currentSensorData = {0.0, 0.0, 0.0, 0.0, 0.0, false, millis()};
  beatStartTime = millis();
  ecgBeatTime = millis();

  Serial.println("✅ Sensor Module Ready!");
  Serial.println("🔬 Monitoring vital signs...");
  Serial.println("======================================\n");
}

void loop()
{
  // Read sensors at specified interval
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL)
  {
    readAD8232();
    readPulseSensor();
    readBMP180();
    calculateHeartRates();
    lastSensorRead = millis();
  }

  // Send data to main controller
  if (millis() - lastDataTransmission >= DATA_SEND_INTERVAL)
  {
    sendSensorData();
    lastDataTransmission = millis();
  }

  // Visual feedback for heartbeat
  blinkHeartbeat();

  // Small delay to prevent watchdog issues
  delay(10);
}

void setupWiFi()
{
  Serial.println("🔧 Connecting to WiFi Access Point...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30)
  {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\n✅ WiFi Connected!");
    Serial.println("📡 IP Address: " + WiFi.localIP().toString());
    Serial.println("📶 Signal Strength: " + String(WiFi.RSSI()) + " dBm");
  }
  else
  {
    Serial.println("\n❌ WiFi Connection Failed!");
    Serial.println("⚠️ Operating in offline mode");
  }
}

void setupSensors()
{
  Serial.println("🔧 Initializing sensors...");

  // Initialize BMP180
  if (bmp180.begin())
  {
    Serial.println("✅ BMP180 sensor initialized");
    currentSensorData.sensorsConnected = true;
  }
  else
  {
    Serial.println("❌ BMP180 sensor not found");
    currentSensorData.sensorsConnected = false;
  }

  // Initialize AD8232 (no initialization needed, just analog read)
  Serial.println("✅ AD8232 ECG monitor ready");

  // Initialize Pulse Sensor (no initialization needed, just analog read)
  Serial.println("✅ Pulse sensor ready");

  Serial.println("🔬 All sensors initialized");
}

void readAD8232()
{
  // Check if leads are connected
  if ((digitalRead(AD8232_LO_PLUS_PIN) == 1) || (digitalRead(AD8232_LO_MINUS_PIN) == 1))
  {
    leadsConnected = false;
    ecgSignal = 0;
  }
  else
  {
    leadsConnected = true;
    ecgSignal = analogRead(AD8232_OUTPUT_PIN);

    // Simple peak detection for heart rate calculation
    if (ecgSignal > ecgThreshold && (millis() - ecgBeatTime) > 300)
    {
      ecgBeatCount++;
      ecgBeatTime = millis();

      // Calculate heart rate every 10 seconds
      if (ecgBeatCount >= 10)
      {
        unsigned long timeDiff = millis() - (ecgBeatTime - (10 * 60000 / ecgHeartRate)); // Approximate
        if (timeDiff > 0)
        {
          ecgHeartRate = (ecgBeatCount * 60000.0) / timeDiff;
          currentSensorData.heartRateECG = ecgHeartRate;
        }
        ecgBeatCount = 0;
      }
    }
  }
}

void readPulseSensor()
{
  pulseSignal = analogRead(PULSE_SENSOR_PIN);

  // Simple pulse detection algorithm
  if (pulseSignal > threshold && !pulseDetected && (millis() - lastBeat) > 300)
  {
    pulseDetected = true;
    lastBeat = millis();
    beatCount++;

    // Calculate BPM every 10 beats or 10 seconds
    if (beatCount >= 10 || (millis() - beatStartTime) > 10000)
    {
      unsigned long timeDiff = millis() - beatStartTime;
      if (timeDiff > 0 && beatCount > 0)
      {
        BPM = (beatCount * 60000) / timeDiff;
        currentSensorData.heartRatePulse = BPM;
      }
      beatCount = 0;
      beatStartTime = millis();
    }
  }

  // Reset pulse detection when signal drops
  if (pulseSignal < (threshold - 100))
  {
    pulseDetected = false;
  }
}

void readBMP180()
{
  if (currentSensorData.sensorsConnected)
  {
    // Read temperature in Celsius
    float tempC = bmp180.readTemperature();
    // Convert to Fahrenheit
    currentSensorData.temperature = (tempC * 9.0 / 5.0) + 32.0;

    // Read pressure in Pascals
    currentSensorData.pressure = bmp180.readPressure() / 100.0; // Convert to mbar

    // For future: Use pressure changes to estimate blood pressure
    // This requires calibration and additional sensors
  }
}

void calculateHeartRates()
{
  // Use the most reliable heart rate source
  float finalHeartRate = 0;

  if (leadsConnected && currentSensorData.heartRateECG > 40 && currentSensorData.heartRateECG < 200)
  {
    finalHeartRate = currentSensorData.heartRateECG;
  }
  else if (currentSensorData.heartRatePulse > 40 && currentSensorData.heartRatePulse < 200)
  {
    finalHeartRate = currentSensorData.heartRatePulse;
  }

  // Apply simple filtering to reduce noise
  static float lastHeartRate = 0;
  if (lastHeartRate > 0)
  {
    finalHeartRate = (finalHeartRate + lastHeartRate) / 2.0;
  }
  lastHeartRate = finalHeartRate;

  // Update both readings with filtered value for consistency
  currentSensorData.heartRateECG = finalHeartRate;
  currentSensorData.heartRatePulse = finalHeartRate;
}

void sendSensorData()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin("http://" + String(MAIN_CONTROLLER_IP) + "/api/sensor-data");
    http.addHeader("Content-Type", "application/json");

    // Prepare JSON data
    DynamicJsonDocument doc(512);
    doc["heartRateECG"] = currentSensorData.heartRateECG;
    doc["heartRatePulse"] = currentSensorData.heartRatePulse;
    doc["temperature"] = currentSensorData.temperature;
    doc["pressure"] = currentSensorData.pressure;
    doc["spO2"] = currentSensorData.spO2; // Placeholder for future SpO2 sensor
    doc["leadsConnected"] = leadsConnected;
    doc["sensorsConnected"] = currentSensorData.sensorsConnected;
    doc["timestamp"] = millis();

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      if (httpResponseCode != 200)
      {
        Serial.println("⚠️ Data transmission error: " + String(httpResponseCode));
      }
    }
    else
    {
      Serial.println("❌ HTTP Error: " + String(httpResponseCode));
    }

    http.end();
  }
  else
  {
    Serial.println("📡 WiFi disconnected - buffering data");
    // TODO: Store data locally for later transmission
  }

  // Print current readings to serial for debugging
  Serial.print("❤️ HR(ECG): " + String(currentSensorData.heartRateECG, 1));
  Serial.print(" | HR(Pulse): " + String(currentSensorData.heartRatePulse, 1));
  Serial.print(" | 🌡️ Temp: " + String(currentSensorData.temperature, 1) + "°F");
  Serial.print(" | 📊 Pressure: " + String(currentSensorData.pressure, 1) + " mbar");
  Serial.println(" | 🔗 Leads: " + String(leadsConnected ? "OK" : "DISCONNECTED"));
}

void blinkHeartbeat()
{
  // Visual feedback for detected heartbeats
  if (currentSensorData.heartRatePulse > 0 || currentSensorData.heartRateECG > 0)
  {
    // Calculate blink interval based on heart rate
    float avgHeartRate = (currentSensorData.heartRatePulse + currentSensorData.heartRateECG) / 2.0;
    unsigned long blinkInterval = 60000 / avgHeartRate; // ms per beat

    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > blinkInterval)
    {
      digitalWrite(PULSE_BLINK_PIN, HIGH);
      delay(100);
      digitalWrite(PULSE_BLINK_PIN, LOW);
      lastBlink = millis();
    }
  }
}
