#include <SoftwareSerial.h>
#include <Wire.h>
#include <ModbusMaster.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>

#define ONE_WIRE_BUS 4 // Temp
#define RS_RO        3
#define RS_DI        2
#define RS_DE_RE     8

HX711 scale;

ModbusMaster node;
SoftwareSerial RS_Slave(RS_RO, RS_DI);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int numReadings = 1;
float readings[numReadings];
int readingIndex = 0;
float total = 0;
float Berat = 0.0;
String lcdWeightDisplay = "";

float nilai_ph = 0.0;
float TempL = 0.0;
float TempA = 0.0;
float Voltage = 0.0;
float gas;
int storage1 = 0;
int storage2 = 0;

int mqPin = A2; // Pin analog untuk sensor MQ-2
int digitalPin = 5; // Pin digital untuk sensor MQ-2
int solenoidPin = 9; // Pin untuk mengendalikan solenoid valve
bool transferGas = false;

int kapasitasMaksimal = 1000; // Kapasitas maksimal balon gas dalam m^3
float gas1 = 900.0; // Aliran gas pertama dalam m^3 (90% dari kapasitas maksimal)
float gas2 = 0.0; // Aliran gas kedua dalam m^3

float convertToCelsius(int analogValue) {
  float voltage = analogValue * (5.0 / 1023.0);
  float resistance = (5000.0 * voltage) / (5.0 - voltage);
  float tempInKelvin = 1 / (1 / 298.15 + 1 / 3977.0 * log(resistance / 10000.0)) + 4;
  float tempInCelsius = tempInKelvin - 273.15;
  return round(tempInCelsius * 10);
}

unsigned long lcdUpdateTime = 0;
const unsigned long lcdUpdateInterval = 500;

void preTransmission() {
  digitalWrite(RS_DE_RE, HIGH);
}

void postTransmission() {
  digitalWrite(RS_DE_RE, LOW);
}

unsigned long lastWeightUpdate = 0;
const unsigned long weightUpdateInterval = 10000; // 10 detik
bool sendWeightData = false;

uint8_t dataPin = 6;
uint8_t clockPin = 7;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  RS_Slave.begin(9600);
  sensors.begin();
  lcd.init();
  lcd.backlight();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(RS_DE_RE, OUTPUT);
  digitalWrite(RS_DE_RE, LOW);

  pinMode(digitalPin, INPUT);
  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, LOW);

  Serial.println("Gas Meter OK !!");
  node.begin(3, RS_Slave);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  scale.begin(dataPin, clockPin);
  scale.set_scale(14.92440);
  scale.tare(20);

  loading_lcd();
}

bool state = true;

void loading_lcd() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("..Loading Screen..");
  lcd.setCursor(3, 1);
  lcd.print("IOT Data Sensor");
  lcd.setCursor(4, 2);
  lcd.print("BIODIGESTER");
  lcd.setCursor(6, 3);
  lcd.print("PT. OSDD");
  delay(5000);
  lcd.clear();
}

void updateLCD(float TempA, float TempL, float nilai_ph, float Berat, float gas, float gas2, int storage1, int storage2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TL:");
  lcd.print(TempL, 1);
  lcd.print("'C");

  lcd.setCursor(9, 0);
  lcd.print("|");
  lcd.print("TA:");
  lcd.print(TempA, 1);
  lcd.print("'C");

  lcd.setCursor(0, 1);
  lcd.print("PH:");
  lcd.print(nilai_ph, 2);

  lcd.setCursor(9, 1);
  lcd.print("|");
  lcd.print("W:");
  lcd.print(Berat, 1);
  lcd.print(" Kg");

  lcd.setCursor(0, 2);
  lcd.print("G1:");
  lcd.print(gas);

  lcd.setCursor(9, 2);
  lcd.print("|");
  lcd.print("G2:");
  lcd.print(gas2);

  lcd.setCursor(0, 3);
  lcd.print("S1:");
  lcd.print(storage1);

  lcd.setCursor(9, 3);
  lcd.print("|");
  lcd.print("S2:");
  lcd.print(storage2);

  delay(500);
}

void KirimDataESP32(float TempA, float TempL, float nilai_ph, float Berat, float gas, float gas2, float storage1, float storage2) {
  byte nilai_ph_bytes[sizeof(float)];
  byte TempL_bytes[sizeof(float)];
  byte TempA_bytes[sizeof(float)];
  byte Berat_bytes[sizeof(float)];
  byte gas_bytes[sizeof(float)];
  byte gas2_bytes[sizeof(float)];
  byte storage1_bytes[sizeof(float)];
  byte storage2_bytes[sizeof(float)];

  memcpy(nilai_ph_bytes, &nilai_ph, sizeof(float));
  memcpy(TempL_bytes, &TempL, sizeof(float));
  memcpy(TempA_bytes, &TempA, sizeof(float));
  memcpy(Berat_bytes, &Berat, sizeof(float));
  memcpy(gas_bytes, &gas, sizeof(float));
  memcpy(gas2_bytes, &gas2, sizeof(float));
  memcpy(storage1_bytes, &storage1, sizeof(float));
  memcpy(storage2_bytes, &storage2, sizeof(float));

  Wire.beginTransmission(0x75);
  Wire.write(nilai_ph_bytes, sizeof(float));
  Wire.write(TempL_bytes, sizeof(float));
  Wire.write(TempA_bytes, sizeof(float));
  Wire.write(Berat_bytes, sizeof(float));
  Wire.write(gas_bytes, sizeof(float));
  Wire.write(gas2_bytes, sizeof(float));
  Wire.write(storage1_bytes, sizeof(float));
  Wire.write(storage2_bytes, sizeof(float));
  Wire.endTransmission();
}

void gasRead() {
  if (RS_Slave.available()) {
    Serial.write(RS_Slave.read());
  }

  if (Serial.available()) {
    digitalWrite(RS_DE_RE, HIGH);
    delay(2);
    RS_Slave.write(Serial.read());
    digitalWrite(RS_DE_RE, LOW);
  }

  gas = node.readHoldingRegisters(0x40001, 1);

  if (gas == node.ku8MBSuccess) {
    float gasFloat = static_cast<float>(node.getResponseBuffer(0)) * 0.001;
    gas = gasFloat + 196.61;
    }else{
    gas = 0;
  }
}

void gas2Read() {
  float kapasitasSekarang = 550.0;
  gas2 = (((kapasitasMaksimal - kapasitasSekarang) * 0.001) - 0.44) + gas;
}

void storage1Read() {
  storage1 = kapasitasMaksimal - (gas + gas2 - 0.01) * 100;
}

void storage2Read() {
  storage2 = kapasitasMaksimal;
}

void TempLRead() {
  float temperatureKelvin = analogRead(A1);
  TempL = ((temperatureKelvin - 273.15) / 10) - (5 * 3.3);
}

void TempARead() {
  sensors.requestTemperatures();
  TempA = sensors.getTempCByIndex(0);
}

void nilai_phRead() {
  nilai_ph = 0;
  for (int test_cycle = 1; test_cycle <= 10; test_cycle++) {
    nilai_ph += analogRead(A0);
    delay(10);
  }

  float analog = (float)nilai_ph / 10.0;
  delay(50);

  float kalibrasi = 7.0 - (2.12 * (-5.0 / 0.45) + 2);
  nilai_ph = ((Voltage - 2.5) * (-5.0 / 1.4) + ((kalibrasi / 3.3)) + 1.76);

  Voltage = analog * (5.0 / 1023.0);
}

void BeratRead() {
  if (scale.is_ready()) {
    float weight = scale.get_units(1) / 1000;
    total -= readings[readingIndex];
    readings[readingIndex] = weight;
    total += weight;
    readingIndex = (readingIndex + 1) % numReadings;
    Berat = total / numReadings;
  }
}

void BeratReadAndSend() {
  BeratRead();
  unsigned long currentMillis = millis();

  // pengiriman data setelah 10 detik
  if (currentMillis - lastWeightUpdate >= weightUpdateInterval) {
    lastWeightUpdate = currentMillis;
    sendWeightData = true;
  }

  if (sendWeightData) {
    sendWeightData = false;
  }
}


// void Selenoid_Valve() {
//   int rawValue = analogRead(mqPin);
//   int digitalValue = digitalRead(digitalPin);

//   Serial.print("Analog Value: ");
//   Serial.print(rawValue);
//   Serial.print(" Digital Value: ");
//   Serial.println(digitalValue);

//   // Hitung kapasitas balon gas berdasarkan pembacaan sensor
//   if (digitalValue == HIGH) {
//     Serial.println("Balon penuh");
//   } else {
//     float gasConcentration = (float)rawValue / 1024.0 * 100.0;
//     Serial.print("Konsentrasi Gas: ");
//     Serial.print(gasConcentration);
//     Serial.println("%");

//     // Hitung sisa kapasitas balon gas
//     float sisaKapasitas = kapasitasMaksimal - gas - gas2;

//     // Jika sisa kapasitas cukup besar, aktifkan transfer gas
//     if (sisaKapasitas >= 30.0 && !transferGas) {
//       transferGas = true;
//       digitalWrite(solenoidPin, HIGH); // Buka solenoid valve

//       // Tunggu selama 30 detik untuk mentransfer gas
//       delay(30000);

//       digitalWrite(solenoidPin, LOW); // Tutup solenoid
//       transferGas = false;

//       // Update aliran gas kedua (gas2)
//       gas2 += 30.0;
//     }
//   }
//   delay(1000);
// }

void loop() {
  gasRead();
  gas2Read();
  TempLRead();
  TempARead();
  nilai_phRead();
  BeratRead();
  BeratReadAndSend();
  storage1Read();
  storage2Read();
  // Selenoid_Valve();

  updateLCD(TempA, TempL, nilai_ph, Berat, gas, gas2, storage1, storage2);
  KirimDataESP32(TempA, TempL, nilai_ph, Berat, gas, gas2, storage1, storage2);

  // Serial print
  Serial.print("Temperature Ambient: ");
  Serial.print(TempA, 1);
  Serial.print("°C | Temperature Liquid: ");
  Serial.print(TempL, 1);
  Serial.print("°C | pH: ");
  Serial.print(nilai_ph, 2);
  Serial.print(" | Weight: ");
  Serial.print(Berat, 1);
  Serial.print(" Kg | Gas: ");
  Serial.print(gas);
  Serial.print(" m3 | Gas: ");
  Serial.print(gas2);
  Serial.print(" m3 | Storage 1: ");
  Serial.print(storage1);
  Serial.print(" m3 | Storage 2: ");
  Serial.print(storage2);
  Serial.print(" m3");
  Serial.println();
  // end serial print
}
