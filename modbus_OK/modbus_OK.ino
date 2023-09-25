#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>

#define ONE_WIRE_BUS 4 // Pin untuk sensor suhu
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

unsigned int pH_Value = 0;
float Voltage = 0.0;
float TempL = 0.0;

int storage = 0;

unsigned long lcdUpdateTime = 0;
const unsigned long lcdUpdateInterval = 500;

void preTransmission()
{
  digitalWrite(RS_DE_RE, HIGH);
}

void postTransmission()
{
  digitalWrite(RS_DE_RE, LOW);
}

uint8_t dataPin = 6;
uint8_t clockPin = 7;

void updateLCD(float temperatureC, float pHValue, float weight, int gasFlow1, int gasFlow2, int storage1, int storage2)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureC, 1);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("pH: ");
  lcd.print(pHValue, 2);

  lcd.setCursor(0, 2);
  lcd.print("Weight: ");
  lcd.print(weight, 1);
  lcd.print(" Kg");

  lcd.setCursor(0, 3);
  lcd.print("GasFlow1: ");
  lcd.print(gasFlow1);

  lcd.setCursor(10, 3);
  lcd.print("GasFlow2: ");
  lcd.print(gasFlow2);

  lcd.setCursor(0, 4);
  lcd.print("Storage1: ");
  lcd.print(storage1);

  lcd.setCursor(10, 4);
  lcd.print("Storage2: ");
  lcd.print(storage2);
}

void setup()
{
  sensors.begin();
  Wire.begin();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(2, INPUT);
  Wire.beginTransmission(0x27);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.beginTransmission(0x75);
  Wire.write(0x02);
  Wire.endTransmission();

  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);
  scale.set_scale(2.319226);
  scale.tare(20);

  RS_Slave.begin(9600);
  pinMode(RS_DE_RE, OUTPUT);
  digitalWrite(RS_DE_RE, LOW);

  Serial.println("Slave IS Ready maseee");

  node.begin(8, RS_Slave);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop()
{
  float temperatureC = sensors.getTempCByIndex(0);
  float TempL = ((temperatureC - 273.15) / 10) - (5 * 3.3);

  float pHValue = 0;
  for (int test_cycle = 1; test_cycle <= 10; test_cycle++)
  {
    pHValue += analogRead(A0);
    delay(10);
  }
  pHValue = (float)pHValue / 10.0;
  delay(50);

  Voltage = pHValue * (12.0 / 1023.0);
  String dataToSend = "pH=" + String(pHValue, 2) + ",Voltage=" + String(Voltage, 3) + "\n";

  float weight = 0;
  if (scale.is_ready())
  {
    weight = scale.get_units(1) / 1000;
    total -= readings[readingIndex];
    readings[readingIndex] = weight;
    total += weight;
    readingIndex = (readingIndex + 1) % numReadings;
    average = total / numReadings;
  }

  if (RS_Slave.available())
  {
    Serial.write(RS_Slave.read());
  }

  if (Serial.available())
  {
    digitalWrite(RS_DE_RE, HIGH);
    delay(2);
    RS_Slave.write(Serial.read());
    digitalWrite(RS_DE_RE, LOW);
  }

  uint8_t gas;
  uint16_t data[10];

  gas = node.readHoldingRegisters(0x40001, 1);

  int gasFlow1 = 0;
  if (gas == node.ku8MBSuccess)
  {
    gasFlow1 = node.getResponseBuffer(gas);
    Serial.print("Value ");
    Serial.print(gas);
    Serial.print(": ");
    Serial.println(gasFlow1);
  }

  int gasFlow2 = 0;

  Serial.println(dataToSend);
  Serial.print("Tegangan Analog = ");
  Serial.print(Voltage);
  Serial.print(" _ ");

  int digital = digitalRead(2);
  delay(50);
  Serial.print("Logic Digital = ");
  Serial.print(digital);

  float calibration_value = 7.0 - (3.0 * (-5.0 / 0.45) + 2.5);
  float ph_act = (Voltage - 4.3) * (-5.0 / 1.4) + ((calibration_value / 3.3) + 12);

  Serial.print(" _ pH = ");
  Serial.println(ph_act, 2);

  int storage1 = 0;
  int storage2 = 0;

  updateLCD(temperatureC, pHValue, weight, gasFlow1, gasFlow2, storage1, storage2);
}
