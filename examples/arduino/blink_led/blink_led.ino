/*
  Blink LED Example for Arduino
  
  This example demonstrates the basic Arduino structure and GPIO control.
  The built-in LED (pin 13 on most Arduino boards) will blink every second.
  
  Board: Arduino Uno/Nano/Mega
  Author: HTH 2025 Team
  License: MIT
*/

// Pin definition
const int ledPin = LED_BUILTIN;  // Built-in LED pin (usually pin 13)

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Arduino Blink LED Example");
  
  // Initialize the LED pin as output
  pinMode(ledPin, OUTPUT);
  
  Serial.println("Setup complete. LED will start blinking...");
}

void loop() {
  // Turn the LED on
  digitalWrite(ledPin, HIGH);
  Serial.println("LED ON");
  delay(1000);  // Wait for 1 second
  
  // Turn the LED off
  digitalWrite(ledPin, LOW);
  Serial.println("LED OFF");
  delay(1000);  // Wait for 1 second
}