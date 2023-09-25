#include <SoftwareSerial.h>

SoftwareSerial gasMeter(0, 1);  // RX, TX pins of the TTL to RS485 converter module
byte slaveID = 0x04;  // Slave ID for the gas meter

void setup() {
  Serial.begin(4800); // Change baud rate to match the documentation
  gasMeter.begin(4800);  // Change baud rate to match the documentation
}

void loop() {

  readHistoricalFlowHourly();
  readHistoricalFlowDaily();
  readHistoricalFlowMonthly();
  delay(10000);
}

void readHistoricalFlowHourly() {

  byte command[] = {slaveID, 0x03, 0x00, 0x0E, 0x00, 0x03, 0x64, 0x08};
  
  gasMeter.write(command, sizeof(command));
  delay(100);

  while (gasMeter.available() > 0) {
    int byteRead = gasMeter.read();
    Serial.print(byteRead, HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void readHistoricalFlowDaily() {

  byte command[] = {slaveID, 0x03, 0x00, 0x10, 0x00, 0x03, 0x64, 0x08};
  
  gasMeter.write(command, sizeof(command));
  delay(100);

  while (gasMeter.available() > 0) {
    int byteRead = gasMeter.read();
    Serial.print(byteRead, HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void readHistoricalFlowMonthly() {

  byte command[] = {slaveID, 0x03, 0x00, 0x12, 0x00, 0x03, 0x64, 0x08}; // Example command
  
  gasMeter.write(command, sizeof(command));
  delay(100);  // Wait for response

  while (gasMeter.available() > 0) {
    int byteRead = gasMeter.read();
    Serial.print(byteRead, HEX); // Display byte value in hexadecimal
    Serial.print(" ");
  }
  Serial.println();
}
