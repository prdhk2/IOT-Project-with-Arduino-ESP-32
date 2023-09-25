#include <SoftwareSerial.h>
#include <Wire.h>
#include <ModbusMaster.h>
#include <util/crc16.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h>

#define ONE_WIRE_BUS 4 // T

#define RS_RO 3
#define RS_DI 2
#define RS_DE_RE 8

HX711 scale;

ModbusMaster node;
SoftwareSerial RS_Slave(RS_RO, RS_DI);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int numReadings = 10;
float readings[numReadings];
int readingIndex = 0;
float total = 0;
float average = 0.0;
String lcdWeightDisplay = "";

float ph_act = 0.0;
float TempL = 0.0;
float temperatureC = 0.0;
float Voltage = 0.0;
float pH_Value = 0.0;

uint16_t gas = 65535; // Nilai awal maksimum
uint16_t gas2 = 0;

float convertToCelsius(int analogValue) {
  float voltage = analogValue * (5.0 / 1023.0);
  float resistance = (5000.0 * voltage) / (5.0 - voltage);
  float tempInKelvin = 1 / (1 / 298.15 + 1 / 3977.0 * log(resistance / 10000.0)) + 4;
  float tempInCelsius = tempInKelvin - 273.15;
  return round(tempInCelsius * 10);
}

int storage = 0;

unsigned long lcdUpdateTime = 0;
const unsigned long lcdUpdateInterval = 500;

void preTransmission() {
  digitalWrite(RS_DE_RE, HIGH);
}

void postTransmission() {
  digitalWrite(RS_DE_RE, LOW);
}

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

  Wire.beginTransmission(0x27);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.beginTransmission(0x75);
  Wire.write(0x02);
  Wire.endTransmission();

  pinMode(RS_DE_RE, OUTPUT);
  digitalWrite(RS_DE_RE, LOW);

  Serial.println("Slave IS Ready Bos!!");
  node.begin(8, RS_Slave);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);
  scale.set_scale(2.319226);
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

void updateLCD(float temperatureC, float TempL, float ph_act, float average, uint16_t gas, int storage1, int storage2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TL:");
  lcd.print(TempL, 1);
  lcd.print("'C");

  lcd.setCursor(9, 0);
  lcd.print("|");
  lcd.print("TA:");
  lcd.print(temperatureC, 1);
  lcd.print("'C");

  lcd.setCursor(0, 1);
  lcd.print("PH:");
  lcd.print(ph_act, 2);

  lcd.setCursor(9, 1);
  lcd.print("|");
  lcd.print("W:");
  lcd.print(average, 1);
  lcd.print(" Kg");

  lcd.setCursor(0, 2);
  lcd.print("GF1:");
  lcd.print(gas);

  lcd.setCursor(9, 2);
  lcd.print("|");
  lcd.print("GF2: ");
  lcd.print(gas2);

  lcd.setCursor(0, 3);
  lcd.print("St1: ");
  lcd.print(storage1);

  lcd.setCursor(9, 3);
  lcd.print("|");
  lcd.print("St2: ");
  lcd.print(storage2);
}

void sendValuesToESP32(float temperatureC, float TempL, float ph_act, float average, uint16_t gas) {
  byte gasBytes[2];

  // Mengonversi uint16_t ke dua byte dengan urutan byte yang benar
  gasBytes[0] = (byte)(gas & 0xFF);
  gasBytes[1] = (byte)((gas >> 8) & 0xFF);

  byte ph_act_bytes[sizeof(float)];
  byte tempL_bytes[sizeof(float)];
  byte tempC_bytes[sizeof(float)];
  byte average_bytes[sizeof(float)];

  memcpy(ph_act_bytes, &ph_act, sizeof(float));
  memcpy(tempL_bytes, &TempL, sizeof(float));
  memcpy(tempC_bytes, &temperatureC, sizeof(float));
  memcpy(average_bytes, &average, sizeof(float));

  Wire.beginTransmission(0x75); // Ganti dengan alamat I2C ESP32 Anda
  Wire.write(ph_act_bytes, sizeof(float));
  Wire.write(tempL_bytes, sizeof(float));
  Wire.write(tempC_bytes, sizeof(float));
  Wire.write(average_bytes, sizeof(float));
  Wire.write(gasBytes, sizeof(gasBytes));
  Wire.endTransmission();
}

void loop() {
  if (RS_Slave.available())
    Serial.write(RS_Slave.read());

  if (Serial.available()) {
    digitalWrite(RS_DE_RE, HIGH);
    delay(2);
    RS_Slave.write(Serial.read());
    digitalWrite(RS_DE_RE, LOW);
  }

  uint16_t gas;
  uint16_t data[10];

  gas = node.readHoldingRegisters(0x40001, 10);

  if (gas == node.ku8MBSuccess) {
    gas = node.getResponseBuffer(gas); // Ambil data gas yang sudah dibaca
  } else {
    gas = 0; // Atur gas ke 0 jika pembacaan gagal
  }

  //TempL
  float temperatureKelvin = analogRead(A1);
  float TempL = ((temperatureKelvin - 273.15) / 10) - (5 * 3.3);

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  // ph calibration start
  pH_Value = 0;
  for (int test_cycle = 1; test_cycle <= 10; test_cycle++) {
    pH_Value += analogRead(A0);
    delay(10);
  }

  float analog = (float)pH_Value / 10.0;
  delay(50);

  float calibration_value = 7.0 - (3.0 * (-5.0 / 0.45) + 2.5);
  float ph_act = (Voltage - 4.3) * (-5.0 / 1.4) + ((calibration_value / 3.3) + 12);

  // ph calibration end
  Voltage = analog * (12.0 / 1023.0);

  // calibration weight start
  if (scale.is_ready()) {
    float weight = scale.get_units(1) / 1000;
    total -= readings[readingIndex];
    readings[readingIndex] = weight;
    total += weight;
    readingIndex = (readingIndex + 1) % numReadings;
    average = total / numReadings;
  }

  int storage1 = 0;
  int storage2 = 0;

  // Lanjutkan dengan kode loop Anda

  updateLCD(temperatureC, TempL, ph_act, average, gas, storage1, storage2);
  sendValuesToESP32(temperatureC, TempL, ph_act, average, gas);

  // Serial print
  Serial.print("Temperature Ambien: ");
  Serial.print(temperatureC, 1);
  Serial.print("°C | Temperature Liquid: ");
  Serial.print(TempL, 1);
  Serial.print("°C | pH: ");
  Serial.print(ph_act, 2);
  Serial.print(" | Weight: ");
  Serial.print(average, 1);
  Serial.print(" Kg | Gas: ");
  Serial.println(gas);
}
