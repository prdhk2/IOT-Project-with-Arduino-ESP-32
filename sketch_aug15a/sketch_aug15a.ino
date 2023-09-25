#include <ModbusMaster.h>
#include <SoftwareSerial.h>

ModbusMaster node;
SoftwareSerial Serial2(8, 9);

#define ID_meter  4  // Alamat perangkat Modbus Anda
#define Reg_E     0x0E  // Alamat register yang akan dibaca

void setup() {
  Serial.begin(4800);
  Serial2.begin(4800);
  
  node.begin(ID_meter, Serial2);
}

void loop() {
  uint8_t result;
  uint16_t data;
  
  result = node.readHoldingRegisters(Reg_E, 2); // Baca 1 register
  
  if (result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0); // Ambil data dari buffer respon
    Serial.print("Data = ");
    Serial.println(data);
  } else {
    Serial.println("Failed to read data");
  }
  
  delay(1000); // Tunggu sejenak sebelum membaca lagi
}
