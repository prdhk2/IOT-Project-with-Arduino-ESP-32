#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <Wire.h>


#define RS_RO  3
#define RS_DI  2
#define RS_DE_RE  8

LiquidCrystal_I2C lcd(0x27, 20, 4);

ModbusMaster node;
SoftwareSerial RS_Slave (RS_RO, RS_DI);

void preTransmission()
{
  digitalWrite(RS_DE_RE, HIGH);
}

void postTransmission()
{
  digitalWrite(RS_DE_RE, LOW);
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RS_Slave.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(RS_DE_RE, OUTPUT);
  digitalWrite(RS_DE_RE, LOW);

  Serial.print("Slave IS Ready maseee");

  // Modbus slave ID 1
  node.begin(3, RS_Slave); 

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

bool state = true;

void loop() {
  if (RS_Slave.available())
    Serial.write(RS_Slave.read());

  if (Serial.available()) {
    digitalWrite(RS_DE_RE, HIGH);
    delay(2); // Tambahkan delay untuk memastikan bahwa DE telah aktif sebelum data dikirim
    RS_Slave.write(Serial.read());
    digitalWrite(RS_DE_RE, LOW);
  }

  uint8_t gas1;
  uint8_t gas2;
  uint8_t gas3;
  uint8_t gas4;
  uint8_t gas5;
  uint8_t gas6;
  uint8_t gas7;
  uint8_t gas8;
  uint8_t gas9;
  uint8_t gas10;
  uint16_t data[10];

  // gas = node.readHoldingRegisters(0x40001, 10);
  // gas1 = node.readHoldingRegisters(0x40001, 10);
  gas2 = node.readHoldingRegisters(0x40011, 10);
  // gas3 = node.readHoldingRegisters(0x40003, 10);
  // gas4 = node.readHoldingRegisters(0x40004, 10);
  // gas5 = node.readHoldingRegisters(0x40005, 10);
  // gas6 = node.readHoldingRegisters(0x40006, 10);
  // gas7 = node.readHoldingRegisters(0x40007, 10);
  // gas8 = node.readHoldingRegisters(0x40008, 10);
  // gas9 = node.readHoldingRegisters(0x40009, 10);
  // gas10 = node.readHoldingRegisters(0x40010, 10);

  if (gas2== node.ku8MBSuccess)
  {
    // int gas = 0;
    // int gas1 = 0;
    int gas2 = 0;
    // int gas3 = 0;
    // int gas4 = 0;
    // int gas5 = 0;
    // int gas6 = 0;
    // int gas7 = 0;
    // int gas8 = 0;
    // int gas9 = 0;
    // int gas10 = 0;
    {
      Serial.print("Value ");
      Serial.print(gas2);
      Serial.print(": ");
      Serial.print(node.getResponseBuffer(gas2));
      Serial.println();

      delay(1000);
      // lcd.setCursor(0, 2);
      // lcd.print("GF1:");
      // lcd.print((gas1));
      // lcd.print("|");
      // lcd.print("GF2:");
      // lcd.print(node.getResponseBuffer(gas1));
    }
  }
}

//   byte byte1 = gas & 0xFF;
//   byte byte2 = (gas >> 8) & 0xFF;
//   byte byte3 = (gas >> 16) & 0xFF;
//   byte byte4 = (gas >> 24) & 0xFF;
//   byte byte5 = (gas >> 32) & 0xFF;
//   byte byte6 = (gas >> 40) & 0xFF;

//   // Kirim ke ESP32 (alamat 0x75)
//   Wire.beginTransmission(0x75); // Alamat ESP32
//   Wire.write(byte1); // Kirim byte pertama (LSB)
//   Wire.write(byte2); // Kirim byte kedua
//   Wire.write(byte3); // Kirim byte ketiga
//   Wire.write(byte4); // Kirim byte keempat (MSB)
//   Wire.endTransmission();

//   Serial.print("Mengirim angka: ");
//   Serial.print(gas1);

//   delay(1000);
//   // delay(1000);
// }
