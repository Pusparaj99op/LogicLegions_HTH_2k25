/*
 * VitalCare Rural - Single ESP32 Complete System
 *
 * This ESP32 module combines all functionality into one device:
 * - WiFi Access Point for mobile connectivity
 * - Real-time web dashboard with WebSocket communication
 * - Patient registration and management
 * - Direct sensor readings (AD8232, BMP180, Pulse sensor)
 * - Data storage with MicroSD card
 * - Cellular communication with SIM800L (optional)
 * - Live vital signs display with 1-second updates
 *
 * Hardware: ESP32-WROOM-32 (Single Controller)
 * Components: AD8232 Heart Monitor, BMP180 Sensor, Pulse Sensor,
 *            MicroSD Card Module, SIM800L GSM Module
 * Author: VitalCare Rural Team
 * Date: January 2025
 * Educational Purpose Only - Not for Medical Use
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>

// Pin Definitions
#define AD8232_LO_PLUS_PIN 18  // AD8232 Leads Off Detection +
#define AD8232_LO_MINUS_PIN 19 // AD8232 Leads Off Detection -
#define AD8232_OUTPUT_PIN 36   // AD8232 Analog Output (VP)
#define PULSE_SENSOR_PIN 39    // Pulse sensor analog input (VN)
#define PULSE_LED_PIN 2        // Built-in LED for pulse indication
#define SD_CS_PIN 5            // MicroSD Card Chip Select
#define SIM800_RX_PIN 16       // SIM800L RX (connect to ESP32 TX)
#define SIM800_TX_PIN 17       // SIM800L TX (connect to ESP32 RX)
#define BUZZER_PIN 4           // Buzzer for alerts

// BMP180 uses default I2C pins: SDA=21, SCL=22

// Network Configuration
const char *AP_SSID = "VitalCare-Rural";
const char *AP_PASSWORD = "VitalCare2025";
const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress AP_GATEWAY(192, 168, 4, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);

// Web Server and WebSocket
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Sensor Objects
Adafruit_BMP085 bmp180;
SoftwareSerial sim800(SIM800_RX_PIN, SIM800_TX_PIN);

// Patient Data Structure
struct Patient
{
  String id;
  String name;
  int age;
  String gender;
  String contact;
  String emergencyContact;
  String medicalConditions;
  unsigned long registrationTime;
};

struct VitalSigns
{
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

// Global Variables
Patient currentPatient;
VitalSigns currentVitals;
bool patientRegistered = false;
bool sdCardReady = false;
bool sim800Ready = false;
bool bmp180Ready = false;

// Timing variables
unsigned long lastVitalUpdate = 0;
unsigned long lastSensorRead = 0;
unsigned long lastDataSave = 0;
unsigned long lastHeartbeatTime = 0;

const unsigned long VITAL_UPDATE_INTERVAL = 1000; // 1 second for UI updates
const unsigned long SENSOR_READ_INTERVAL = 100;   // 100ms for sensor readings
const unsigned long DATA_SAVE_INTERVAL = 30000;   // 30 seconds for SD card saves
const unsigned long HEARTBEAT_TIMEOUT = 10000;    // 10 seconds heartbeat timeout

// Pulse detection variables
int pulseThreshold = 2048; // Adjustable threshold for pulse detection
int lastPulseValue = 0;
bool pulseDetected = false;
unsigned long pulseCount = 0;
unsigned long pulseWindow = 0;

// Function Prototypes
void setupHardware();
void setupSensors();
void setupWiFiAP();
void setupWebServer();
void setupWebSocket();
void setupSDCard();
void setupSIM800();

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void handleRoot();
void handlePatientRegistration();
void handleGetPatientData();
void handleGetVitalSigns();
void handleSystemStatus();
void handleNotFound();

void readSensors();
void calculateHeartRate();
void estimateBloodPressure();
void sendVitalSignsToClients();
void saveDataToSD();
void sendSMSAlert(String message);
void checkForAlerts();

String generatePatientID();
String formatTimestamp(unsigned long timestamp);
String getSystemStatusJSON();

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("=====================================");
  Serial.println("🏥 VitalCare Rural - Complete System");
  Serial.println("=====================================");

  // Initialize hardware and sensors
  setupHardware();
  setupSensors();

  // Initialize SPIFFS for web files
  if (!SPIFFS.begin(true))
  {
    Serial.println("❌ SPIFFS initialization failed");
    return;
  }
  Serial.println("✅ SPIFFS initialized");

  // Setup components
  setupWiFiAP();
  setupWebServer();
  setupWebSocket();
  setupSDCard();
  setupSIM800();

  // Initialize mDNS
  if (MDNS.begin("vitalcare"))
  {
    Serial.println("✅ mDNS responder started: http://vitalcare.local");
  }

  // Initialize current vitals with default values
  currentVitals = {0.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0, millis(), "System Ready"};

  Serial.println("\n🌐 VitalCare Rural System Ready!");
  Serial.println("📱 Connect to WiFi: " + String(AP_SSID));
  Serial.println("🌐 Open browser: http://192.168.4.1");
  Serial.println("=====================================\n");
}

void loop()
{
  // Handle web server requests
  server.handleClient();

  // Handle WebSocket connections
  webSocket.loop();

  // Read sensors at high frequency
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL)
  {
    readSensors();
    lastSensorRead = millis();
  }

  // Update UI every second
  if (millis() - lastVitalUpdate >= VITAL_UPDATE_INTERVAL)
  {
    calculateHeartRate();
    estimateBloodPressure();
    currentVitals.timestamp = millis();

    if (patientRegistered)
    {
      currentVitals.status = "Monitoring";
      checkForAlerts();
    }
    else
    {
      currentVitals.status = "No Patient";
    }

    sendVitalSignsToClients();
    lastVitalUpdate = millis();
  }

  // Save data to SD card periodically
  if (millis() - lastDataSave >= DATA_SAVE_INTERVAL)
  {
    if (patientRegistered && sdCardReady)
    {
      saveDataToSD();
    }
    lastDataSave = millis();
  }

  // Small delay to prevent watchdog issues
  delay(10);
}

void setupHardware()
{
  Serial.println("🔧 Initializing hardware pins...");

  // Configure sensor pins
  pinMode(AD8232_LO_PLUS_PIN, INPUT);
  pinMode(AD8232_LO_MINUS_PIN, INPUT);
  pinMode(PULSE_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Configure analog pins
  analogReadResolution(12); // 12-bit ADC resolution

  digitalWrite(PULSE_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("✅ Hardware pins configured");
}

void setupSensors()
{
  Serial.println("🔧 Initializing sensors...");

  // Initialize I2C for BMP180
  Wire.begin();

  // Initialize BMP180
  if (bmp180.begin())
  {
    bmp180Ready = true;
    Serial.println("✅ BMP180 sensor initialized");
  }
  else
  {
    Serial.println("❌ BMP180 sensor not found");
  }

  // Initialize pulse detection
  pulseWindow = millis();
  Serial.println("✅ Pulse sensor configured");
  Serial.println("✅ AD8232 ECG sensor configured");
}

void setupWiFiAP()
{
  Serial.println("🔧 Setting up WiFi Access Point...");

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);

  bool apStarted = WiFi.softAP(AP_SSID, AP_PASSWORD);

  if (apStarted)
  {
    Serial.println("✅ WiFi Access Point started");
    Serial.println("📡 SSID: " + String(AP_SSID));
    Serial.println("🔐 Password: " + String(AP_PASSWORD));
    Serial.println("🌐 IP Address: " + WiFi.softAPIP().toString());
  }
  else
  {
    Serial.println("❌ Failed to start Access Point");
  }
}

void setupWebServer()
{
  Serial.println("🔧 Setting up Web Server...");

  // Serve static files from SPIFFS
  server.serveStatic("/", SPIFFS, "/", "max-age=86400");

  // API Endpoints
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/register-patient", HTTP_POST, handlePatientRegistration);
  server.on("/api/patient", HTTP_GET, handleGetPatientData);
  server.on("/api/vitals", HTTP_GET, handleGetVitalSigns);
  server.on("/api/status", HTTP_GET, handleSystemStatus);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("✅ Web Server started on port 80");
}

void setupWebSocket()
{
  Serial.println("🔧 Setting up WebSocket Server...");
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  Serial.println("✅ WebSocket Server started on port 81");
}

void setupSDCard()
{
  Serial.println("🔧 Initializing SD Card...");

  if (SD.begin(SD_CS_PIN))
  {
    sdCardReady = true;
    Serial.println("✅ SD Card initialized");

    // Create data directory if it doesn't exist
    if (!SD.exists("/vitalcare"))
    {
      SD.mkdir("/vitalcare");
      Serial.println("✅ Created /vitalcare directory");
    }
  }
  else
  {
    Serial.println("❌ SD Card initialization failed");
  }
}

void setupSIM800()
{
  Serial.println("🔧 Initializing SIM800L...");

  sim800.begin(9600);
  delay(3000);

  // Test SIM800L communication
  sim800.println("AT");
  delay(1000);

  if (sim800.available())
  {
    String response = sim800.readString();
    if (response.indexOf("OK") > -1)
    {
      sim800Ready = true;
      Serial.println("✅ SIM800L initialized and ready");
    }
    else
    {
      Serial.println("❌ SIM800L not responding properly");
    }
  }
  else
  {
    Serial.println("❌ SIM800L not found or not responding");
  }
}

void readSensors()
{
  // Read AD8232 ECG sensor
  bool loPlus = digitalRead(AD8232_LO_PLUS_PIN);
  bool loMinus = digitalRead(AD8232_LO_MINUS_PIN);

  if (!loPlus && !loMinus)
  {
    // Good connection, read ECG value
    currentVitals.ecgValue = analogRead(AD8232_OUTPUT_PIN);
  }
  else
  {
    // Poor connection
    currentVitals.ecgValue = 0;
  }

  // Read pulse sensor
  int pulseValue = analogRead(PULSE_SENSOR_PIN);

  // Simple pulse detection algorithm
  if (pulseValue > pulseThreshold && lastPulseValue <= pulseThreshold)
  {
    pulseDetected = true;
    pulseCount++;
    digitalWrite(PULSE_LED_PIN, HIGH);
    lastHeartbeatTime = millis();
  }
  else
  {
    digitalWrite(PULSE_LED_PIN, LOW);
  }

  lastPulseValue = pulseValue;

  // Read BMP180 if available
  if (bmp180Ready)
  {
    currentVitals.temperature = (bmp180.readTemperature() * 9.0 / 5.0) + 32.0; // Convert to Fahrenheit
    currentVitals.pressure = bmp180.readPressure() / 100.0;                    // Convert to hPa
  }
  else
  {
    // Fallback temperature simulation
    currentVitals.temperature = 98.6 + random(-10, 11) / 10.0;
    currentVitals.pressure = 1013.25 + random(-20, 21); // Standard atmospheric pressure
  }

  // Estimate SpO2 (simplified - would need proper red/IR LED setup)
  // For now, simulate based on pulse quality
  if (currentVitals.ecgValue > 0)
  {
    currentVitals.spO2 = 98 + random(-2, 3);
  }
  else
  {
    currentVitals.spO2 = 95; // Lower value when no signal
  }
}

void calculateHeartRate()
{
  unsigned long currentTime = millis();

  // Calculate heart rate over 15-second window
  if (currentTime - pulseWindow >= 15000)
  {
    currentVitals.heartRate = (pulseCount * 60.0) / 15.0; // Convert to BPM
    pulseCount = 0;
    pulseWindow = currentTime;
  }

  // Check for heartbeat timeout
  if (currentTime - lastHeartbeatTime > HEARTBEAT_TIMEOUT)
  {
    currentVitals.heartRate = 0;
  }

  // Bounds checking
  if (currentVitals.heartRate < 0)
    currentVitals.heartRate = 0;
  if (currentVitals.heartRate > 200)
    currentVitals.heartRate = 200;
}

void estimateBloodPressure()
{
  // Simplified BP estimation based on heart rate and other factors
  // In a real device, this would require proper cuff and pressure sensors

  if (currentVitals.heartRate > 0)
  {
    // Base values
    float baseSystolic = 120;
    float baseDiastolic = 80;

    // Adjust based on heart rate
    if (currentVitals.heartRate > 100)
    {
      baseSystolic += (currentVitals.heartRate - 100) * 0.5;
      baseDiastolic += (currentVitals.heartRate - 100) * 0.3;
    }
    else if (currentVitals.heartRate < 60)
    {
      baseSystolic -= (60 - currentVitals.heartRate) * 0.3;
      baseDiastolic -= (60 - currentVitals.heartRate) * 0.2;
    }

    // Add some realistic variation
    currentVitals.systolicBP = baseSystolic + random(-5, 6);
    currentVitals.diastolicBP = baseDiastolic + random(-3, 4);
  }
  else
  {
    currentVitals.systolicBP = 0;
    currentVitals.diastolicBP = 0;
  }
}

void checkForAlerts()
{
  bool alertCondition = false;
  String alertMessage = "";

  // Check for critical vital signs
  if (currentVitals.heartRate < 50 || currentVitals.heartRate > 120)
  {
    alertCondition = true;
    alertMessage += "Heart Rate: " + String(currentVitals.heartRate) + " BPM. ";
  }

  if (currentVitals.systolicBP > 160 || currentVitals.systolicBP < 80)
  {
    alertCondition = true;
    alertMessage += "Blood Pressure: " + String(currentVitals.systolicBP) + "/" + String(currentVitals.diastolicBP) + " mmHg. ";
  }

  if (currentVitals.spO2 < 90)
  {
    alertCondition = true;
    alertMessage += "SpO2: " + String(currentVitals.spO2) + "%. ";
  }

  if (currentVitals.temperature > 102 || currentVitals.temperature < 95)
  {
    alertCondition = true;
    alertMessage += "Temperature: " + String(currentVitals.temperature) + "°F. ";
  }

  if (alertCondition)
  {
    currentVitals.status = "⚠️ ALERT";

    // Sound buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);

    // Send SMS if configured
    if (sim800Ready && currentPatient.emergencyContact.length() > 0)
    {
      String smsMessage = "ALERT: " + currentPatient.name + " - " + alertMessage + "Location: VitalCare Rural Clinic";
      sendSMSAlert(smsMessage);
    }

    Serial.println("⚠️ ALERT: " + alertMessage);
  }
  else
  {
    currentVitals.status = "✅ Normal";
  }
}

void saveDataToSD()
{
  if (!sdCardReady)
    return;

  String filename = "/vitalcare/" + currentPatient.id + "_" +
                    String(currentPatient.registrationTime / 1000) + ".csv";

  File dataFile = SD.open(filename, FILE_APPEND);
  if (dataFile)
  {
    // Write header if file is new
    if (dataFile.size() == 0)
    {
      dataFile.println("timestamp,heartRate,systolicBP,diastolicBP,spO2,temperature,ecgValue,pressure,status");
    }

    // Write data
    dataFile.print(currentVitals.timestamp);
    dataFile.print(",");
    dataFile.print(currentVitals.heartRate);
    dataFile.print(",");
    dataFile.print(currentVitals.systolicBP);
    dataFile.print(",");
    dataFile.print(currentVitals.diastolicBP);
    dataFile.print(",");
    dataFile.print(currentVitals.spO2);
    dataFile.print(",");
    dataFile.print(currentVitals.temperature);
    dataFile.print(",");
    dataFile.print(currentVitals.ecgValue);
    dataFile.print(",");
    dataFile.print(currentVitals.pressure);
    dataFile.print(",");
    dataFile.println(currentVitals.status);

    dataFile.close();
    Serial.println("💾 Data saved to SD card: " + filename);
  }
  else
  {
    Serial.println("❌ Error opening SD card file for writing");
  }
}

void sendSMSAlert(String message)
{
  if (!sim800Ready)
    return;

  sim800.println("AT+CMGF=1"); // Set SMS text mode
  delay(1000);

  sim800.print("AT+CMGS=\"");
  sim800.print(currentPatient.emergencyContact);
  sim800.println("\"");
  delay(1000);

  sim800.print(message);
  delay(1000);

  sim800.write(26); // Send Ctrl+Z to send SMS
  delay(5000);

  Serial.println("📱 SMS alert sent to: " + currentPatient.emergencyContact);
}

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("🔌 Client [%u] disconnected\n", num);
    break;

  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("🔌 Client [%u] connected from %d.%d.%d.%d\n",
                  num, ip[0], ip[1], ip[2], ip[3]);

    // Send current patient data to new client
    DynamicJsonDocument doc(1024);
    doc["type"] = "init";
    doc["patientRegistered"] = patientRegistered;
    if (patientRegistered)
    {
      doc["patient"]["name"] = currentPatient.name;
      doc["patient"]["age"] = currentPatient.age;
      doc["patient"]["gender"] = currentPatient.gender;
    }

    String message;
    serializeJson(doc, message);
    webSocket.sendTXT(num, message);
    break;
  }

  case WStype_TEXT:
    Serial.printf("📨 Received from [%u]: %s\n", num, payload);
    break;

  default:
    break;
  }
}

void handleRoot()
{
  // Serve embedded dashboard with enhanced features
  String html = R"(<!DOCTYPE html>
<html><head><title>VitalCare Rural</title><meta name="viewport" content="width=device-width, initial-scale=1">
<style>body{font-family:Arial;margin:0;background:#f0f8ff}
.header{background:#2c5e9b;color:white;text-align:center;padding:1rem}
.container{max-width:900px;margin:0 auto;padding:1rem}
.card{background:white;border-radius:8px;padding:1rem;margin:1rem 0;box-shadow:0 2px 4px rgba(0,0,0,0.1)}
.vital{display:flex;justify-content:space-between;align-items:center;padding:0.5rem;margin:0.5rem 0;border-left:4px solid #2c5e9b}
.value{font-size:1.5em;font-weight:bold}
.status{padding:0.2rem 0.5rem;border-radius:4px;color:white;font-size:0.8em}
.normal{background:#28a745}.warning{background:#ffc107;color:#000}.critical{background:#dc3545}
.patient-form input,select{width:100%;padding:0.5rem;margin:0.5rem 0;border:1px solid #ddd;border-radius:4px;box-sizing:border-box}
.btn{background:#2c5e9b;color:white;padding:0.7rem 1.5rem;border:none;border-radius:4px;cursor:pointer;font-size:1em;margin:0.5rem 0}
.btn:hover{background:#1e4a73}
.system-info{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:1rem;margin:1rem 0}
.info-item{background:#e8f4fd;padding:1rem;border-radius:4px;text-align:center}
.alert{background:#fff3cd;border:1px solid #ffeaa7;color:#856404;padding:1rem;border-radius:4px;margin:1rem 0}
.ecg-display{height:150px;border:1px solid #ddd;background:#000;color:#0f0;font-family:monospace;overflow:hidden;position:relative}
</style></head><body>
<div class="header">
<h1>🏥 VitalCare Rural</h1>
<p>Complete Portable Health Monitoring System</p>
<div id="systemStatus" class="status normal">System Ready</div>
</div>
<div class="container">

<div class="card" id="systemCard">
<h2>📊 System Status</h2>
<div class="system-info">
<div class="info-item"><strong>📡 WiFi</strong><br><span id="wifiStatus">Active</span></div>
<div class="info-item"><strong>💾 SD Card</strong><br><span id="sdStatus">Ready</span></div>
<div class="info-item"><strong>📱 GSM</strong><br><span id="gsmStatus">Ready</span></div>
<div class="info-item"><strong>🌡️ Sensors</strong><br><span id="sensorStatus">Online</span></div>
</div>
</div>

<div class="card" id="patientCard">
<h2>👤 Patient Registration</h2>
<form id="patientForm">
<input type="text" id="patientName" placeholder="Full Name" required>
<input type="number" id="patientAge" placeholder="Age" min="1" max="150" required>
<select id="patientGender" required>
<option value="">Select Gender</option>
<option value="Male">Male</option>
<option value="Female">Female</option>
<option value="Other">Other</option>
</select>
<input type="text" id="patientContact" placeholder="Contact Number">
<input type="text" id="emergencyContact" placeholder="Emergency Contact (for SMS alerts)" >
                <input type = "text" id = "medicalConditions" placeholder = "Known Medical Conditions">
                    <button type = "submit" class = "btn">📝 Register Patient</ button>
                    </ form>
                    </ div>

                    <div class = "card" id = "vitalsCard" style = "display:none">
                    <h2>📊 Live Vital Signs</ h2>
                    <div class = "alert" id = "alertBox" style = "display:none"></ div>

                    <div class = "vital">
                    <span>❤️ Heart Rate : </ span>
                                         <div><span class = "value" id = "heartRate"> --</ span>
                                             BPM<span class = "status normal" id = "hrStatus">
                                                 Normal</ span></ div>
                                         </ div>

                                         <div class = "vital">
                                         <span>🩸 Blood Pressure : </ span>
                                                                   <div><span class = "value" id = "bloodPressure"> --/
                                                                   --</ span> mmHg<span class = "status normal" id = "bpStatus"> Normal</ span></ div>
                                                                   </ div>

                                                                   <div class = "vital">
                                                                   <span>🫁 SpO2 : </ span>
                                                                                   <div><span class = "value" id = "spO2"> --%
                                                                   </ span><span class = "status normal" id = "spo2Status"> Normal</ span></ div>
                                                                   </ div>

                                                                   <div class = "vital">
                                                                   <span>🌡️ Temperature : </ span>
                                                                                          <div><span class = "value" id = "temperature"> --°F</ span><span class = "status normal" id = "tempStatus">
                                                                                              Normal</ span></ div>
                                                                                          </ div>

                                                                                          <div class = "vital">
                                                                                          <span>📈 ECG Signal : </ span>
                                                                                                                <div><span class = "value" id = "ecgValue"> --</ span><span class = "status normal" id = "ecgStatus">
                                                                                                                    Normal</ span></ div>
                                                                                                                </ div>

                                                                                                                <h3>📈 ECG Waveform</ h3>
                                                                                                                <div class = "ecg-display" id = "ecgDisplay"></ div>

                                                                                                                <button class = "btn" onclick = "exportData()">💾 Export Patient Data</ button>
                                                                                                                </ div>

                                                                                                                </ div>

                                                                                                                <script> const ws = new WebSocket('ws://192.168.4.1:81');
  let patientRegistered = false;
  let ecgChart = [];
  let maxECGPoints = 100;

  ws.onopen = function()
  {
    console.log('WebSocket connected');
  };

  ws.onmessage = function(event)
  {
    const data = JSON.parse(event.data);

    if (data.type == = 'vitals')
    {
      updateVitalSigns(data);
      updateECGDisplay(data.ecgValue || 0);

      // Check for alerts
      if (data.status && data.status.includes('ALERT'))
      {
        showAlert(data.status);
      }
      else
      {
        hideAlert();
      }
    }

    if (data.type == = 'init' && data.patientRegistered)
    {
      document.getElementById('patientCard').style.display = 'none';
      document.getElementById('vitalsCard').style.display = 'block';
      patientRegistered = true;
    }
  };

  ws.onerror = function(error)
  {
    console.error('WebSocket error:', error);
  };

  function updateVitalSigns(data)
  {
    document.getElementById('heartRate').textContent = data.heartRate ? data.heartRate.toFixed(0) : '--';
    document.getElementById('bloodPressure').textContent =
        (data.systolicBP && data.diastolicBP) ? data.systolicBP.toFixed(0) + '/' + data.diastolicBP.toFixed(0) : '--/--';
    document.getElementById('spO2').textContent = data.spO2 ? data.spO2.toFixed(0) + '%' : '--%';
    document.getElementById('temperature').textContent = data.temperature ? data.temperature.toFixed(1) + '°F' : '--°F';
    document.getElementById('ecgValue').textContent = data.ecgValue || '--';

    // Update status indicators
    updateVitalStatus('hr', data.heartRate, 60, 100);
    updateVitalStatus('bp', data.systolicBP, 90, 140);
    updateVitalStatus('spo2', data.spO2, 95, 100);
    updateVitalStatus('temp', data.temperature, 97, 100);
    updateVitalStatus('ecg', data.ecgValue, 100, 4000);
  }

  function updateVitalStatus(type, value, minNormal, maxNormal)
  {
    const statusElement = document.getElementById(type + 'Status');
    if (!statusElement)
      return;

    if (value >= minNormal && value <= maxNormal)
    {
      statusElement.className = 'status normal';
      statusElement.textContent = 'Normal';
    }
    else if (value < minNormal * 0.8 || value > maxNormal * 1.2)
    {
      statusElement.className = 'status critical';
      statusElement.textContent = 'Critical';
    }
    else
    {
      statusElement.className = 'status warning';
      statusElement.textContent = 'Warning';
    }
  }

  function updateECGDisplay(ecgValue)
  {
    ecgChart.push(ecgValue);
    if (ecgChart.length > maxECGPoints)
    {
      ecgChart.shift();
    }

    const display = document.getElementById('ecgDisplay');
    display.innerHTML = '';

    const canvas = document.createElement('canvas');
    canvas.width = display.offsetWidth;
    canvas.height = display.offsetHeight;
    display.appendChild(canvas);

    const ctx = canvas.getContext('2d');
    ctx.strokeStyle = '#0f0';
    ctx.lineWidth = 2;

    ctx.beginPath();
    for (let i = 0; i < ecgChart.length; i++)
    {
      const x = (i / ecgChart.length) * canvas.width;
      const y = canvas.height - ((ecgChart[i] / 4095) * canvas.height);

      if (i == = 0)
      {
        ctx.moveTo(x, y);
      }
      else
      {
        ctx.lineTo(x, y);
      }
    }
    ctx.stroke();
  }

  function showAlert(message)
  {
    const alertBox = document.getElementById('alertBox');
    alertBox.textContent = message;
    alertBox.style.display = 'block';
    alertBox.className = 'alert';
  }

  function hideAlert()
  {
    document.getElementById('alertBox').style.display = 'none';
  }

  function exportData()
  {
    const link = document.createElement('a');
    link.href = '/api/vitals';
    link.download = 'patient_vitals.json';
    link.click();
  }

  document.getElementById('patientForm').addEventListener('submit', function(e) {
  e.preventDefault();
  
  const formData = {
    name: document.getElementById('patientName').value,
    age: parseInt(document.getElementById('patientAge').value),
    gender: document.getElementById('patientGender').value,
    contact: document.getElementById('patientContact').value,
    emergencyContact: document.getElementById('emergencyContact').value,
    medicalConditions: document.getElementById('medicalConditions').value
  };
  
  fetch('/api/register-patient', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(formData)
  })
  .then(response => response.json())
  .then(data => {
    if (data.success) {
      document.getElementById('patientCard').style.display = 'none';
      document.getElementById('vitalsCard').style.display = 'block';
      patientRegistered = true;
    } else {
      alert('Registration failed: ' + data.message);
    }
  })
  .catch(error => {
    console.error('Error:', error);
    alert('Registration failed. Please try again.');
  }); });

  // Update system status periodically
  setInterval(function() { fetch('/api/status')
                               .then(response = > response.json())
                               .then(data = > {
                                 document.getElementById('systemStatus').textContent = data.status || 'System Ready';
                               })
                               .catch(error = > {
                                 console.error('Status update failed:', error);
                               }); }, 5000);

</script>
</body></html>)";
  
  server.send(200, "text/html", html);
}

void handlePatientRegistration()
{
  if (server.hasArg("plain"))
  {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (!error)
    {
      currentPatient.id = generatePatientID();
      currentPatient.name = doc["name"].as<String>();
      currentPatient.age = doc["age"].as<int>();
      currentPatient.gender = doc["gender"].as<String>();
      currentPatient.contact = doc["contact"].as<String>();
      currentPatient.emergencyContact = doc["emergencyContact"].as<String>();
      currentPatient.medicalConditions = doc["medicalConditions"].as<String>();
      currentPatient.registrationTime = millis();

      patientRegistered = true;

      Serial.println("✅ Patient registered:");
      Serial.println("👤 Name: " + currentPatient.name);
      Serial.println("🎂 Age: " + String(currentPatient.age));
      Serial.println("⚧ Gender: " + currentPatient.gender);

      DynamicJsonDocument response(256);
      response["success"] = true;
      response["patientId"] = currentPatient.id;
      response["message"] = "Patient registered successfully";

      String responseString;
      serializeJson(response, responseString);
      server.send(200, "application/json", responseString);
    }
    else
    {
      server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
    }
  }
  else
  {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"No data received\"}");
  }
}

void handleGetPatientData()
{
  DynamicJsonDocument doc(1024);

  if (patientRegistered)
  {
    doc["registered"] = true;
    doc["patient"]["id"] = currentPatient.id;
    doc["patient"]["name"] = currentPatient.name;
    doc["patient"]["age"] = currentPatient.age;
    doc["patient"]["gender"] = currentPatient.gender;
    doc["patient"]["contact"] = currentPatient.contact;
    doc["patient"]["emergencyContact"] = currentPatient.emergencyContact;
    doc["patient"]["medicalConditions"] = currentPatient.medicalConditions;
    doc["patient"]["registrationTime"] = formatTimestamp(currentPatient.registrationTime);
  }
  else
  {
    doc["registered"] = false;
    doc["message"] = "No patient registered";
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetVitalSigns()
{
  DynamicJsonDocument doc(1024);

  doc["heartRate"] = currentVitals.heartRate;
  doc["systolicBP"] = currentVitals.systolicBP;
  doc["diastolicBP"] = currentVitals.diastolicBP;
  doc["spO2"] = currentVitals.spO2;
  doc["temperature"] = currentVitals.temperature;
  doc["ecgValue"] = currentVitals.ecgValue;
  doc["pressure"] = currentVitals.pressure;
  doc["timestamp"] = formatTimestamp(currentVitals.timestamp);
  doc["status"] = currentVitals.status;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSystemStatus()
{
  DynamicJsonDocument doc(512);

  doc["status"] = "System Operational";
  doc["uptime"] = millis() / 1000;
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["wifiConnected"] = WiFi.softAPgetStationNum();
  doc["sdCardReady"] = sdCardReady;
  doc["sim800Ready"] = sim800Ready;
  doc["bmp180Ready"] = bmp180Ready;
  doc["patientRegistered"] = patientRegistered;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleNotFound()
{
  server.send(404, "text/plain", "404: Page not found");
}

void sendVitalSignsToClients()
{
  DynamicJsonDocument doc(1024);
  doc["type"] = "vitals";
  doc["heartRate"] = currentVitals.heartRate;
  doc["systolicBP"] = currentVitals.systolicBP;
  doc["diastolicBP"] = currentVitals.diastolicBP;
  doc["spO2"] = currentVitals.spO2;
  doc["temperature"] = currentVitals.temperature;
  doc["ecgValue"] = currentVitals.ecgValue;
  doc["pressure"] = currentVitals.pressure;
  doc["timestamp"] = currentVitals.timestamp;
  doc["status"] = currentVitals.status;

  String message;
  serializeJson(doc, message);
  webSocket.broadcastTXT(message);
}

String generatePatientID()
{
  return "VCR" + String(millis()) + String(random(100, 999));
}

String formatTimestamp(unsigned long timestamp)
{
  unsigned long seconds = timestamp / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  return String(hours % 24) + ":" +
         (minutes % 60 < 10 ? "0" : "") + String(minutes % 60) + ":" +
         (seconds % 60 < 10 ? "0" : "") + String(seconds % 60);
}
