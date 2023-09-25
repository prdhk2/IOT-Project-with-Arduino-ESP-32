#include <Wire.h>

void setup() {
  Wire.begin(); // Inisialisasi komunikasi I2C
  Serial.begin(9600);
}

void loop() {
  int angka = analogRead(A1); // Angka ribuan yang akan dikirim

  // Memecah angka menjadi empat byte terpisah
  byte byte1 = angka & 0xFF;
  byte byte2 = (angka >> 8) & 0xFF;
  byte byte3 = (angka >> 16) & 0xFF;
  byte byte4 = (angka >> 24) & 0xFF;
  byte byte5 = (angka >> 32) & 0xFF;
  byte byte6 = (angka >> 40) & 0xFF;

  // Kirim ke ESP32 (alamat 0x75)
  Wire.beginTransmission(0x75); // Alamat ESP32
  Wire.write(byte1); // Kirim byte pertama (LSB)
  Wire.write(byte2); // Kirim byte kedua
  Wire.write(byte3); // Kirim byte ketiga
  Wire.write(byte4); // Kirim byte keempat (MSB)
  Wire.endTransmission();

  Serial.print("Mengirim angka: ");
  Serial.println(angka);

  delay(1000); // Tunggu 1 detik sebelum mengirim angka berikutnya
}
