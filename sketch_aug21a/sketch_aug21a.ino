#include <SoftwareSerial.h>

SoftwareSerial RS485Serial(2, 3); // RX, TX pins on Arduino

#define DE_PIN 4 // Define the Data Enable (DE) pin
#define RE_PIN 5 // Define the Receiver Enable (RE) pin

void setup() {
  Serial.begin(9600);
  RS485Serial.begin(9600); // Set the baud rate to match your communication
}

void loop() {
  // Construct Modbus request (Tx)
  byte request[] = {0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0x98}; // Adjust this request based on your previous communication

  // Set the DE (Data Enable) and RE (Receiver Enable) pins to transmit mode
  digitalWrite(DE_PIN, HIGH);
  digitalWrite(RE_PIN, HIGH);

  RS485Serial.write(request, sizeof(request)); // Send the request

  // Delay for a sufficient time to allow the response to arrive (adjust as needed)
  delay(10);

  // Set the DE (Data Enable) and RE (Receiver Enable) pins to receive mode
  digitalWrite(DE_PIN, LOW);
  digitalWrite(RE_PIN, LOW);

  // Check if there is data available to read
  if (RS485Serial.available() > 0) {
    // Read the response (Rx)
    while (RS485Serial.available()) {
      byte data = RS485Serial.read();
      // Process the response data here
      Serial.print(data, HEX); // Print the response data in hexadecimal format
      Serial.print(" ");
    }
    Serial.println(); // Print a newline after the response
  }

  // Delay between requests (adjust as needed)
  delay(1000);
}
