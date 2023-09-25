#include <SoftwareSerial.h>

SoftwareSerial unoSerial(4, 5); // RX, TX pins for ESP32 communication with Arduino Uno

const char* phoneNumber = "+1234567890"; // Replace with your phone number

void setup() {
  Serial.begin(115200);
  unoSerial.begin(115200);

  // Initialize SIM800L, configure settings, etc.
}

void loop() {
  if (unoSerial.available()) {
    String data = unoSerial.readStringUntil('\n');
    data.trim();
    
    if (data.startsWith("pH=")) {
      // Extract pH and voltage values from data string
      float pHValue = data.substring(3, data.indexOf(',')).toFloat();
      float voltageValue = data.substring(data.indexOf("Voltage=") + 8).toFloat();

      // Now you can use the pHValue and voltageValue as needed
      // For example, send SMS using SIM800L
      sendSMSThroughSIM800L(pHValue, voltageValue);
    }
  }

  // Rest of your ESP32 code
  // ...
}

void sendSMSThroughSIM800L(float pHValue, float voltageValue) {
  // SIM800L SMS sending logic
}
