/*
 * VitalCare Rural - Main ESP32 Web Server
 * 
 * This ESP32 module serves as the main web interface for the VitalCare Rural system.
 * It creates a WiFi Access Point and hosts a real-time web dashboard for monitoring
 * patient vital signs.
 * 
 * Features:
 * - WiFi Access Point for mobile device connectivity
 * - Real-time web dashboard with WebSocket communication
 * - Patient registration and management
 * - Live vital signs display with 1-second updates
 * - Mobile-responsive interface
 * 
 * Hardware: ESP32-WROOM-32 #1 (Main Controller)
 * Author: VitalCare Rural Team
 * Date: August 2025
 * Educational Purpose Only - Not for Medical Use
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiAP.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>

// Network Configuration
const char* AP_SSID = "VitalCare-Rural";
const char* AP_PASSWORD = "VitalCare2025";
const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress AP_GATEWAY(192, 168, 4, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);

// Web Server and WebSocket
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Patient Data Structure
struct Patient {
  String id;
  String name;
  int age;
  String gender;
  String contact;
  String emergencyContact;
  String medicalConditions;
  unsigned long registrationTime;
};

struct VitalSigns {
  float heartRate;
  float systolicBP;
  float diastolicBP;
  float spO2;
  float temperature;
  unsigned long timestamp;
  String status;
};

// Global Variables
Patient currentPatient;
VitalSigns currentVitals;
bool patientRegistered = false;
unsigned long lastVitalUpdate = 0;
const unsigned long VITAL_UPDATE_INTERVAL = 1000; // 1 second

// Function Prototypes
void setupWiFiAP();
void setupWebServer();
void setupWebSocket();
void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void handleRoot();
void handlePatientRegistration();
void handleGetPatientData();
void handleGetVitalSigns();
void handleNotFound();
void sendVitalSignsToClients();
void simulateVitalSigns(); // For testing without actual sensors
String generatePatientID();
String formatTimestamp(unsigned long timestamp);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=====================================");
  Serial.println("🏥 VitalCare Rural - Main Controller");
  Serial.println("=====================================");
  
  // Initialize SPIFFS for web files
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS initialization failed");
    return;
  }
  Serial.println("✅ SPIFFS initialized");
  
  // Setup WiFi Access Point
  setupWiFiAP();
  
  // Setup Web Server
  setupWebServer();
  
  // Setup WebSocket for real-time communication
  setupWebSocket();
  
  // Initialize mDNS
  if (MDNS.begin("vitalcare")) {
    Serial.println("✅ mDNS responder started: http://vitalcare.local");
  }
  
  // Initialize current vitals with default values
  currentVitals = {0.0, 0.0, 0.0, 0.0, 0.0, millis(), "No Patient"};
  
  Serial.println("\n🌐 VitalCare Rural System Ready!");
  Serial.println("📱 Connect to WiFi: " + String(AP_SSID));
  Serial.println("🌐 Open browser: http://192.168.4.1");
  Serial.println("=====================================\n");
}

void loop() {
  // Handle web server requests
  server.handleClient();
  
  // Handle WebSocket connections
  webSocket.loop();
  
  // Update vital signs every second
  if (millis() - lastVitalUpdate >= VITAL_UPDATE_INTERVAL) {
    // TODO: Receive actual sensor data from ESP32 #2
    // For now, simulate data for testing
    simulateVitalSigns();
    sendVitalSignsToClients();
    lastVitalUpdate = millis();
  }
  
  // Small delay to prevent watchdog issues
  delay(10);
}

void setupWiFiAP() {
  Serial.println("🔧 Setting up WiFi Access Point...");
  
  // Configure Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
  
  bool apStarted = WiFi.softAP(AP_SSID, AP_PASSWORD);
  
  if (apStarted) {
    Serial.println("✅ WiFi Access Point started");
    Serial.println("📡 SSID: " + String(AP_SSID));
    Serial.println("🔐 Password: " + String(AP_PASSWORD));
    Serial.println("🌐 IP Address: " + WiFi.softAPIP().toString());
    Serial.println("👥 Max Clients: " + String(WiFi.softAPgetStationNum()) + "/4");
  } else {
    Serial.println("❌ Failed to start Access Point");
  }
}

void setupWebServer() {
  Serial.println("🔧 Setting up Web Server...");
  
  // Serve static files from SPIFFS
  server.serveStatic("/", SPIFFS, "/", "max-age=86400");
  
  // API Endpoints
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/register-patient", HTTP_POST, handlePatientRegistration);
  server.on("/api/patient", HTTP_GET, handleGetPatientData);
  server.on("/api/vitals", HTTP_GET, handleGetVitalSigns);
  
  // Handle 404 errors
  server.onNotFound(handleNotFound);
  
  // Start server
  server.begin();
  Serial.println("✅ Web Server started on port 80");
}

void setupWebSocket() {
  Serial.println("🔧 Setting up WebSocket Server...");
  
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  
  Serial.println("✅ WebSocket Server started on port 81");
}

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("🔌 Client [%u] disconnected\n", num);
      break;
      
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("🔌 Client [%u] connected from %d.%d.%d.%d\n", 
                    num, ip[0], ip[1], ip[2], ip[3]);
      
      // Send current patient data to new client
      DynamicJsonDocument doc(1024);
      doc["type"] = "init";
      doc["patientRegistered"] = patientRegistered;
      if (patientRegistered) {
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

void handleRoot() {
  // Check if custom dashboard exists, otherwise serve default
  if (SPIFFS.exists("/dashboard.html")) {
    File file = SPIFFS.open("/dashboard.html", "r");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    // Serve embedded dashboard
    String html = R"(<!DOCTYPE html>
<html><head><title>VitalCare Rural</title><meta name="viewport" content="width=device-width, initial-scale=1">
<style>body{font-family:Arial;margin:0;background:#f0f8ff}
.header{background:#2c5e9b;color:white;text-align:center;padding:1rem}
.container{max-width:800px;margin:0 auto;padding:1rem}
.card{background:white;border-radius:8px;padding:1rem;margin:1rem 0;box-shadow:0 2px 4px rgba(0,0,0,0.1)}
.vital{display:flex;justify-content:space-between;align-items:center;padding:0.5rem;margin:0.5rem 0;border-left:4px solid #2c5e9b}
.value{font-size:1.5em;font-weight:bold}
.status{padding:0.2rem 0.5rem;border-radius:4px;color:white}
.normal{background:#28a745}.warning{background:#ffc107;color:#000}.critical{background:#dc3545}
.patient-form input,select{width:100%;padding:0.5rem;margin:0.5rem 0;border:1px solid #ddd;border-radius:4px}
.btn{background:#2c5e9b;color:white;padding:0.7rem 1.5rem;border:none;border-radius:4px;cursor:pointer;font-size:1em}
.btn:hover{background:#1e4a73}
</style></head><body>
<div class="header"><h1>🏥 VitalCare Rural</h1><p>Portable Health Monitoring System</p></div>
<div class="container">
<div class="card" id="patientCard"><h2>👤 Patient Registration</h2>
<form id="patientForm">
<input type="text" id="patientName" placeholder="Full Name" required>
<input type="number" id="patientAge" placeholder="Age" min="1" max="150" required>
<select id="patientGender" required><option value="">Select Gender</option><option value="Male">Male</option><option value="Female">Female</option><option value="Other">Other</option></select>
<input type="text" id="patientContact" placeholder="Contact Number">
<input type="text" id="emergencyContact" placeholder="Emergency Contact">
<input type="text" id="medicalConditions" placeholder="Known Medical Conditions">
<button type="submit" class="btn">📝 Register Patient</button>
</form></div>
<div class="card" id="vitalsCard" style="display:none"><h2>📊 Live Vital Signs</h2>
<div class="vital"><span>❤️ Heart Rate:</span><div><span class="value" id="heartRate">--</span> BPM <span class="status normal" id="hrStatus">Normal</span></div></div>
<div class="vital"><span>🩸 Blood Pressure:</span><div><span class="value" id="bloodPressure">--/--</span> mmHg <span class="status normal" id="bpStatus">Normal</span></div></div>
<div class="vital"><span>🫁 SpO2:</span><div><span class="value" id="spO2">--%</span> <span class="status normal" id="spo2Status">Normal</span></div></div>
<div class="vital"><span>🌡️ Temperature:</span><div><span class="value" id="temperature">--°F</span> <span class="status normal" id="tempStatus">Normal</span></div></div>
</div></div>
<script>
const ws = new WebSocket('ws://192.168.4.1:81');
let patientRegistered = false;

ws.onmessage = function(event) {
  const data = JSON.parse(event.data);
  if (data.type === 'vitals') {
    document.getElementById('heartRate').textContent = data.heartRate.toFixed(0);
    document.getElementById('bloodPressure').textContent = data.systolicBP.toFixed(0) + '/' + data.diastolicBP.toFixed(0);
    document.getElementById('spO2').textContent = data.spO2.toFixed(0) + '%';
    document.getElementById('temperature').textContent = data.temperature.toFixed(1) + '°F';
  }
};

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
  }).then(response => response.json()).then(data => {
    if (data.success) {
      document.getElementById('patientCard').style.display = 'none';
      document.getElementById('vitalsCard').style.display = 'block';
      patientRegistered = true;
    }
  });
});
</script></body></html>)";
    server.send(200, "text/html", html);
  }
}

void handlePatientRegistration() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (!error) {
      // Store patient data
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
      
      // Send success response
      DynamicJsonDocument response(256);
      response["success"] = true;
      response["patientId"] = currentPatient.id;
      response["message"] = "Patient registered successfully";
      
      String responseString;
      serializeJson(response, responseString);
      server.send(200, "application/json", responseString);
    } else {
      server.send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
    }
  } else {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"No data received\"}");
  }
}

void handleGetPatientData() {
  DynamicJsonDocument doc(1024);
  
  if (patientRegistered) {
    doc["registered"] = true;
    doc["patient"]["id"] = currentPatient.id;
    doc["patient"]["name"] = currentPatient.name;
    doc["patient"]["age"] = currentPatient.age;
    doc["patient"]["gender"] = currentPatient.gender;
    doc["patient"]["contact"] = currentPatient.contact;
    doc["patient"]["emergencyContact"] = currentPatient.emergencyContact;
    doc["patient"]["medicalConditions"] = currentPatient.medicalConditions;
    doc["patient"]["registrationTime"] = formatTimestamp(currentPatient.registrationTime);
  } else {
    doc["registered"] = false;
    doc["message"] = "No patient registered";
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleGetVitalSigns() {
  DynamicJsonDocument doc(512);
  
  doc["heartRate"] = currentVitals.heartRate;
  doc["systolicBP"] = currentVitals.systolicBP;
  doc["diastolicBP"] = currentVitals.diastolicBP;
  doc["spO2"] = currentVitals.spO2;
  doc["temperature"] = currentVitals.temperature;
  doc["timestamp"] = formatTimestamp(currentVitals.timestamp);
  doc["status"] = currentVitals.status;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Page not found");
}

void sendVitalSignsToClients() {
  DynamicJsonDocument doc(512);
  doc["type"] = "vitals";
  doc["heartRate"] = currentVitals.heartRate;
  doc["systolicBP"] = currentVitals.systolicBP;
  doc["diastolicBP"] = currentVitals.diastolicBP;
  doc["spO2"] = currentVitals.spO2;
  doc["temperature"] = currentVitals.temperature;
  doc["timestamp"] = currentVitals.timestamp;
  doc["status"] = currentVitals.status;
  
  String message;
  serializeJson(doc, message);
  webSocket.broadcastTXT(message);
}

// Simulate vital signs for testing (replace with actual sensor data)
void simulateVitalSigns() {
  if (patientRegistered) {
    // Simulate realistic vital signs with small variations
    currentVitals.heartRate = 72 + random(-5, 6);
    currentVitals.systolicBP = 120 + random(-10, 11);
    currentVitals.diastolicBP = 80 + random(-5, 6);
    currentVitals.spO2 = 98 + random(-2, 3);
    currentVitals.temperature = 98.6 + random(-10, 11) / 10.0;
    currentVitals.timestamp = millis();
    currentVitals.status = "Monitoring";
    
    // Simple status determination
    if (currentVitals.heartRate < 60 || currentVitals.heartRate > 100 ||
        currentVitals.systolicBP < 90 || currentVitals.systolicBP > 140 ||
        currentVitals.spO2 < 95 || currentVitals.temperature < 97 || currentVitals.temperature > 100) {
      currentVitals.status = "Alert";
    }
  } else {
    currentVitals.status = "No Patient";
  }
}

String generatePatientID() {
  return "VCR" + String(millis()) + String(random(100, 999));
}

String formatTimestamp(unsigned long timestamp) {
  unsigned long seconds = timestamp / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  return String(hours % 24) + ":" + 
         (minutes % 60 < 10 ? "0" : "") + String(minutes % 60) + ":" +
         (seconds % 60 < 10 ? "0" : "") + String(seconds % 60);
}
