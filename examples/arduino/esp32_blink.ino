/*
 * ESP32 Blink Example
 * Hardware: ESP32-WROOM-32
 * Framework: Arduino
 */

#define LED_PIN 2 // Built-in LED on ESP32

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("ESP32 Blink Example Started");
  Serial.println("Hardware: ESP32-WROOM-32");
}

void loop()
{
  digitalWrite(LED_PIN, HIGH); // Turn LED on
  Serial.println("LED ON");
  delay(1000); // Wait for 1 second

  digitalWrite(LED_PIN, LOW); // Turn LED off
  Serial.println("LED OFF");
  delay(1000); // Wait for 1 second
}
