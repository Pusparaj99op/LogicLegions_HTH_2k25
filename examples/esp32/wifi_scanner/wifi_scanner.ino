/*
  WiFi Scanner Example for ESP32
  
  This example demonstrates WiFi functionality on ESP32.
  It scans for available WiFi networks and displays them in the serial monitor.
  
  Board: ESP32-WROOM-32 or ESP32-DevKitC
  Author: HTH 2025 Team
  License: MIT
*/

#include "WiFi.h"

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32 WiFi Scanner Example");
  Serial.println("Hack The Hardware - SDCE Nagpur");
  
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("WiFi scanner ready!");
}

void loop() {
  Serial.println("Starting WiFi scan...");
  
  // Scan for networks
  int networkCount = WiFi.scanNetworks();
  
  if (networkCount == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print("Found ");
    Serial.print(networkCount);
    Serial.println(" networks:");
    Serial.println("------------------------------------");
    
    // Print details of each network found
    for (int i = 0; i < networkCount; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm) ");
      Serial.print("[");
      Serial.print(WiFi.BSSIDstr(i));
      Serial.print("] ");
      Serial.print("Channel: ");
      Serial.print(WiFi.channel(i));
      Serial.print(" ");
      
      // Print encryption type
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:
          Serial.print("Open");
          break;
        case WIFI_AUTH_WEP:
          Serial.print("WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          Serial.print("WPA");
          break;
        case WIFI_AUTH_WPA2_PSK:
          Serial.print("WPA2");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          Serial.print("WPA+WPA2");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          Serial.print("WPA2-EAP");
          break;
        case WIFI_AUTH_WPA3_PSK:
          Serial.print("WPA3");
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          Serial.print("WPA2+WPA3");
          break;
        case WIFI_AUTH_WAPI_PSK:
          Serial.print("WAPI");
          break;
        default:
          Serial.print("Unknown");
      }
      Serial.println();
    }
  }
  
  Serial.println("------------------------------------");
  Serial.println("Waiting 10 seconds before next scan...");
  Serial.println();
  
  // Delete scan results to free memory
  WiFi.scanDelete();
  
  // Wait 10 seconds before scanning again
  delay(10000);
}