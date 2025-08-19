/*
 * VitalCare Rural - Communication & Storage ESP32 Module
 * 
 * This ESP32 module handles data storage and cellular communication for the 
 * VitalCare Rural system. It manages offline data storage on MicroSD card
 * and synchronizes data to remote servers when cellular connectivity is available.
 * 
 * Features:
 * - MicroSD card data logging
 * - SIM800L cellular communication
 * - Data synchronization with remote servers
 * - Offline data buffering
 * - Emergency alert system
 * 
 * Hardware: ESP32-WROOM-32 #3 (Communication Controller)
 * Author: VitalCare Rural Team  
 * Date: August 2025
 * Educational Purpose Only - Not for Medical Use
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <TinyGsmClient.h>
#include <StreamDebugger.h>

// Pin Definitions
#define SD_CS_PIN         5   // MicroSD card CS pin
#define SIM800L_TX        17  // SIM800L TX pin (connect to ESP32 RX)
#define SIM800L_RX        16  // SIM800L RX pin (connect to ESP32 TX)
#define SIM800L_RESET     4   // SIM800L reset pin
#define STATUS_LED        2   // Status LED for communication

// Network Configuration
const char* WIFI_SSID = "VitalCare-Rural";
const char* WIFI_PASSWORD = "VitalCare2025";
const char* MAIN_CONTROLLER_IP = "192.168.4.1";

// Cellular Configuration
const char* APN = "internet";  // Change according to your carrier
const char* GPRS_USER = "";    // Usually empty
const char* GPRS_PASS = "";    // Usually empty

// Remote Server Configuration (for demonstration)
const char* REMOTE_SERVER = "http://your-server.com/api";
const char* BACKUP_SERVER = "http://backup-server.com/api";

// GSM Setup
HardwareSerial sim800l(1);
StreamDebugger debugger(sim800l, Serial);
TinyGsm modem(debugger);
TinyGsmClient client(modem);

// Data Structures
struct PatientRecord {
  String patientId;
  String name;
  int age;
  String gender;
  unsigned long sessionStart;
  bool uploaded;
};

struct VitalRecord {
  String patientId;
  float heartRate;
  float systolicBP;
  float diastolicBP;
  float spO2;
  float temperature;
  unsigned long timestamp;
  bool uploaded;
  bool emergency;
};

// Global Variables
bool sdCardAvailable = false;
bool cellularConnected = false;
bool wifiConnected = false;
PatientRecord currentPatient;
unsigned long lastSyncAttempt = 0;
unsigned long lastHeartbeat = 0;
const unsigned long SYNC_INTERVAL = 30000;      // Sync every 30 seconds
const unsigned long HEARTBEAT_INTERVAL = 5000;  // Status update every 5 seconds

// Emergency thresholds
const float HEART_RATE_MIN = 50;
const float HEART_RATE_MAX = 120;
const float SYSTOLIC_MIN = 90;
const float SYSTOLIC_MAX = 160;
const float SPO2_MIN = 90;
const float TEMP_MIN = 96.0;
const float TEMP_MAX = 102.0;

// Function Prototypes
void setupSDCard();
void setupCellular();
void setupWiFi();
void savePatientRecord(const PatientRecord& patient);
void saveVitalRecord(const VitalRecord& vital);
void syncDataToRemote();
void sendEmergencyAlert(const VitalRecord& vital);
void handleIncomingData();
void sendStatusUpdate();
bool connectCellular();
void blinkStatusLED(int pattern);
String formatDateTime(unsigned long timestamp);
bool isEmergency(const VitalRecord& vital);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("==========================================");
  Serial.println("📡 VitalCare Rural - Communication Module");
  Serial.println("==========================================");
  
  // Initialize pins
  pinMode(STATUS_LED, OUTPUT);
  pinMode(SIM800L_RESET, OUTPUT);
  
  // Reset SIM800L module
  digitalWrite(SIM800L_RESET, LOW);
  delay(100);
  digitalWrite(SIM800L_RESET, HIGH);
  delay(2000);
  
  // Setup SD card for local storage
  setupSDCard();
  
  // Setup WiFi connection
  setupWiFi();
  
  // Setup cellular communication
  setupCellular();
  
  Serial.println("✅ Communication Module Ready!");
  Serial.println("💾 Local storage: " + String(sdCardAvailable ? "Available" : "Unavailable"));
  Serial.println("📶 WiFi: " + String(wifiConnected ? "Connected" : "Disconnected"));
  Serial.println("📱 Cellular: " + String(cellularConnected ? "Connected" : "Disconnected"));
  Serial.println("==========================================\n");
}

void loop() {
  // Handle incoming data from main controller
  handleIncomingData();
  
  // Attempt to sync data periodically
  if (millis() - lastSyncAttempt >= SYNC_INTERVAL) {
    syncDataToRemote();
    lastSyncAttempt = millis();
  }
  
  // Send status updates
  if (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    sendStatusUpdate();
    lastHeartbeat = millis();
  }
  
  // Maintain cellular connection
  if (cellularConnected) {
    if (!modem.isNetworkConnected()) {
      Serial.println("📱 Cellular network lost, reconnecting...");
      cellularConnected = connectCellular();
    }
  }
  
  // Visual status indication
  if (wifiConnected && cellularConnected) {
    blinkStatusLED(1); // Fast blink - all systems operational
  } else if (wifiConnected || cellularConnected) {
    blinkStatusLED(2); // Slow blink - partial connectivity
  } else {
    blinkStatusLED(3); // Solid - no connectivity
  }
  
  delay(100);
}

void setupSDCard() {
  Serial.println("🔧 Initializing MicroSD card...");
  
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("❌ MicroSD card initialization failed");
    sdCardAvailable = false;
    return;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("❌ No SD card attached");
    sdCardAvailable = false;
    return;
  }
  
  sdCardAvailable = true;
  Serial.println("✅ MicroSD card initialized");
  
  // Create directory structure if it doesn't exist
  if (!SD.exists("/patients")) {
    SD.mkdir("/patients");
  }
  if (!SD.exists("/vitals")) {
    SD.mkdir("/vitals");
  }
  if (!SD.exists("/logs")) {
    SD.mkdir("/logs");
  }
  
  // Log startup
  File logFile = SD.open("/logs/system.txt", FILE_APPEND);
  if (logFile) {
    logFile.println(formatDateTime(millis()) + " - System started");
    logFile.close();
  }
  
  Serial.println("📁 Directory structure created");
}

void setupWiFi() {
  Serial.println("🔧 Connecting to WiFi...");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 15) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n✅ WiFi Connected!");
    Serial.println("📡 IP Address: " + WiFi.localIP().toString());
  } else {
    wifiConnected = false;
    Serial.println("\n⚠️ WiFi connection failed");
  }
}

void setupCellular() {
  Serial.println("🔧 Initializing SIM800L module...");
  
  sim800l.begin(9600, SERIAL_8N1, SIM800L_RX, SIM800L_TX);
  delay(3000);
  
  Serial.println("📱 Initializing modem...");
  if (!modem.restart()) {
    Serial.println("❌ Failed to restart modem");
    cellularConnected = false;
    return;
  }
  
  String modemInfo = modem.getModemInfo();
  Serial.println("📱 Modem Info: " + modemInfo);
  
  // Connect to cellular network
  cellularConnected = connectCellular();
}

bool connectCellular() {
  Serial.println("📱 Connecting to cellular network...");
  
  if (!modem.waitForNetwork()) {
    Serial.println("❌ Failed to connect to network");
    return false;
  }
  
  if (modem.isNetworkConnected()) {
    Serial.println("✅ Network connected");
  }
  
  Serial.println("📱 Connecting to APN: " + String(APN));
  if (!modem.gprsConnect(APN, GPRS_USER, GPRS_PASS)) {
    Serial.println("❌ GPRS connection failed");
    return false;
  }
  
  Serial.println("✅ Cellular connection established");
  return true;
}

void savePatientRecord(const PatientRecord& patient) {
  if (!sdCardAvailable) return;
  
  String filename = "/patients/" + patient.patientId + ".json";
  File file = SD.open(filename, FILE_WRITE);
  
  if (file) {
    DynamicJsonDocument doc(1024);
    doc["patientId"] = patient.patientId;
    doc["name"] = patient.name;
    doc["age"] = patient.age;
    doc["gender"] = patient.gender;
    doc["sessionStart"] = patient.sessionStart;
    doc["uploaded"] = patient.uploaded;
    
    serializeJson(doc, file);
    file.close();
    
    Serial.println("💾 Patient record saved: " + patient.name);
  } else {
    Serial.println("❌ Failed to save patient record");
  }
}

void saveVitalRecord(const VitalRecord& vital) {
  if (!sdCardAvailable) return;
  
  String filename = "/vitals/" + String(millis()) + ".json";
  File file = SD.open(filename, FILE_WRITE);
  
  if (file) {
    DynamicJsonDocument doc(512);
    doc["patientId"] = vital.patientId;
    doc["heartRate"] = vital.heartRate;
    doc["systolicBP"] = vital.systolicBP;
    doc["diastolicBP"] = vital.diastolicBP;
    doc["spO2"] = vital.spO2;
    doc["temperature"] = vital.temperature;
    doc["timestamp"] = vital.timestamp;
    doc["uploaded"] = vital.uploaded;
    doc["emergency"] = vital.emergency;
    
    serializeJson(doc, file);
    file.close();
    
    if (vital.emergency) {
      Serial.println("🚨 Emergency vital record saved!");
    }
  }
}

void syncDataToRemote() {
  if (!wifiConnected && !cellularConnected) {
    return; // No connectivity available
  }
  
  Serial.println("🔄 Syncing data to remote server...");
  
  // TODO: Implement actual server synchronization
  // This would involve:
  // 1. Reading unsynced records from SD card
  // 2. Sending them to remote server via HTTP POST
  // 3. Marking records as uploaded upon successful transmission
  // 4. Handling failed transmissions with retry logic
  
  // For demonstration, we'll just log the attempt
  if (sdCardAvailable) {
    File logFile = SD.open("/logs/sync.txt", FILE_APPEND);
    if (logFile) {
      logFile.println(formatDateTime(millis()) + " - Sync attempted");
      logFile.close();
    }
  }
}

void sendEmergencyAlert(const VitalRecord& vital) {
  Serial.println("🚨 EMERGENCY ALERT TRIGGERED!");
  Serial.println("Patient ID: " + vital.patientId);
  Serial.println("Heart Rate: " + String(vital.heartRate));
  Serial.println("Blood Pressure: " + String(vital.systolicBP) + "/" + String(vital.diastolicBP));
  Serial.println("SpO2: " + String(vital.spO2) + "%");
  Serial.println("Temperature: " + String(vital.temperature) + "°F");
  
  // Save emergency record
  saveVitalRecord(vital);
  
  // TODO: Implement actual emergency alert system
  // This could include:
  // 1. SMS to emergency contacts
  // 2. HTTP POST to emergency services API
  // 3. Local alarm activation
  
  // Blink emergency pattern
  for (int i = 0; i < 10; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(100);
    digitalWrite(STATUS_LED, LOW);
    delay(100);
  }
}

void handleIncomingData() {
  // TODO: Implement communication with main controller
  // This would receive patient data and vital signs for storage
  // For now, we'll simulate receiving data periodically
  
  static unsigned long lastDataReceived = 0;
  if (millis() - lastDataReceived > 10000) { // Every 10 seconds
    // Simulate receiving vital signs data
    VitalRecord simulatedVital = {
      "VCR" + String(random(1000, 9999)),
      72.0 + random(-10, 11),
      120.0 + random(-20, 21),
      80.0 + random(-10, 11),
      98.0 + random(-3, 4),
      98.6 + random(-20, 21) / 10.0,
      millis(),
      false,
      false
    };
    
    // Check if it's an emergency
    simulatedVital.emergency = isEmergency(simulatedVital);
    
    if (simulatedVital.emergency) {
      sendEmergencyAlert(simulatedVital);
    } else {
      saveVitalRecord(simulatedVital);
    }
    
    lastDataReceived = millis();
  }
}

void sendStatusUpdate() {
  // Send status to main controller
  DynamicJsonDocument status(256);
  status["module"] = "communication";
  status["sdCard"] = sdCardAvailable;
  status["wifi"] = wifiConnected;
  status["cellular"] = cellularConnected;
  status["timestamp"] = millis();
  
  String statusString;
  serializeJson(status, statusString);
  
  // TODO: Send this to main controller via HTTP POST or serial
  
  Serial.print("📊 Status - SD: " + String(sdCardAvailable ? "OK" : "FAIL"));
  Serial.print(" | WiFi: " + String(wifiConnected ? "OK" : "FAIL"));
  Serial.println(" | Cellular: " + String(cellularConnected ? "OK" : "FAIL"));
}

void blinkStatusLED(int pattern) {
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  unsigned long interval;
  
  switch (pattern) {
    case 1: interval = 200; break;  // Fast blink
    case 2: interval = 1000; break; // Slow blink
    case 3: digitalWrite(STATUS_LED, HIGH); return; // Solid on
    default: interval = 500; break;
  }
  
  if (millis() - lastBlink > interval) {
    ledState = !ledState;
    digitalWrite(STATUS_LED, ledState);
    lastBlink = millis();
  }
}

String formatDateTime(unsigned long timestamp) {
  // Simple timestamp formatting
  unsigned long seconds = timestamp / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  return String(hours % 24) + ":" + 
         (minutes % 60 < 10 ? "0" : "") + String(minutes % 60) + ":" +
         (seconds % 60 < 10 ? "0" : "") + String(seconds % 60);
}

bool isEmergency(const VitalRecord& vital) {
  return (vital.heartRate < HEART_RATE_MIN || vital.heartRate > HEART_RATE_MAX ||
          vital.systolicBP < SYSTOLIC_MIN || vital.systolicBP > SYSTOLIC_MAX ||
          vital.spO2 < SPO2_MIN ||
          vital.temperature < TEMP_MIN || vital.temperature > TEMP_MAX);
}
