/*
 * ESP32 WiFi Connection Example
 * Hardware: ESP32-WROOM-32
 * Framework: Arduino
 */

#include <WiFi.h>

const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 WiFi Connection Example");
  Serial.println("==============================");

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected successfully!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void loop()
{
  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Status: Connected");
  }
  else
  {
    Serial.println("WiFi Status: Disconnected");
    // Attempt to reconnect
    WiFi.begin(ssid, password);
  }

  delay(10000); // Check every 10 seconds
}
