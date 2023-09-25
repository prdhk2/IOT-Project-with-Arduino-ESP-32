#include <ModbusMaster.h>
#include <SoftwareSerial.h>

ModbusMaster node;
SoftwareSerial Serial2(8, 9);

#define ID_meter  4  // Alamat perangkat Modbus Anda
#define Reg_E     0x000E  // Alamat register yang akan dibaca

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  
  node.begin(ID_meter, Serial2);
}

void loop() {
  uint8_t result;
  uint16_t data[2]; // Ubah menjadi array karena Anda membaca 2 register
  
  result = node.readHoldingRegisters(Reg_E, 2); // Baca 2 register dimulai dari Reg_E
  
  if (result == node.ku8MBSuccess) {
    data[0] = node.getResponseBuffer(0); // Ambil data dari buffer respon (register pertama)
    data[1] = node.getResponseBuffer(1); // Ambil data dari buffer respon (register kedua)
    Serial.print("Data Register 0 = ");
    Serial.println(data[0]);
    Serial.print("Data Register 1 = ");
    Serial.println(data[1]);
  } else {
    Serial.println("Failed to read data");
  }
  
  delay(1000); // Tunggu sejenak sebelum membaca lagi
}
