#include <SoftwareSerial.h>

SoftwareSerial sim800lSerial(0, 1);  // RX, TX pins for SIM800L

void setup() {
  Serial.begin(9600);  // Initialize serial monitor
  sim800lSerial.begin(9600);  // Initialize serial communication with SIM800L
}

void loop() {
  // Read input from serial monitor
  if (Serial.available()) {
    char data = Serial.read();
    // Send data to SIM800L module
    sim800lSerial.write(data);
  }
  
  // Read response from SIM800L module
  if (sim800lSerial.available()) {
    char data = sim800lSerial.read();
    // Display response in serial monitor
    Serial.write(data);
  }
}
