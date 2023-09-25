#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"

#define ONE_WIRE_BUS 4 // Pin for temp sensor

// weight start
HX711 scale;

uint8_t dataPin = 6; // dt 
uint8_t clockPin = 7; // sck

const int numReadings = 10;
float readings[numReadings];
int readingIndex = 0;
float total = 0;
float average = 0.0; // define average

String lcdWeightDisplay = "";

// temp start
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ph, vol, tempLiquid start
unsigned int pH_Value = 0;
float Voltage = 0.0;
float TempL = 0.0;

//storage
int storage = 0;

// milis
unsigned long lcdUpdateTime = 0;
const unsigned long lcdUpdateInterval = 500; // Update every 500 milliseconds

float convertToCelsius(int analogValue) {
  float voltage = analogValue * (5.0 / 1023.0);
  float resistance = (5000.0 * voltage) / (5.0 - voltage);
  float tempInKelvin = 1 / (1 / 298.15 + 1 / 3977.0 * log(resistance / 10000.0)) + 4;
  float tempInCelsius = tempInKelvin - 273.15;
  return round(tempInCelsius * 10);
}


void updateLCD(); // Function declaration

void setup() {
  sensors.begin();
  Wire.begin();
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(2, INPUT);
  Wire.beginTransmission(0x27);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.beginTransmission(0x75);
  Wire.write(0x02);
  Wire.endTransmission();
  // delay(400);

  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);

  scale.set_scale(2.319226); // Calibration scale factor
  scale.tare(20);
}

void updateLCD() {
  // lcd.setCursor(0, 2);
  // lcd.print(lcdWeightDisplay);
  // lcd.print(" Kg");
}

void loop() {

  //TempL 
  float temperatureKelvin = analogRead(A1);
  float TempL = ((temperatureKelvin - 273.15) / 10) - (5 * 3.3);

  // ph calibration start
  pH_Value = 0;
  for (int test_cycle = 1; test_cycle <= 10; test_cycle++) {
    pH_Value += analogRead(A0);
    delay(10);
  }
  float analog = (float)pH_Value / 10.0;
  delay(50);
  // ph calibration end

  Voltage = analog * (12.0 / 1023.0);
  String dataToSend = "pH=" + String(analog, 2) + ",Voltage=" + String(Voltage, 3) + "\n";

  // calibration weight start
  if (scale.is_ready()) {
    float weight = scale.get_units(1) / 1000;
    total -= readings[readingIndex];
    readings[readingIndex] = weight;
    total += weight;
    readingIndex = (readingIndex + 1) % numReadings;
    average = total / numReadings;

    Serial.print("Berat: ");
    Serial.println(average, 3);

    // delay(200);
  
  // calibration weight end

  // serial print start
  Serial.println(dataToSend);
  Serial.print("Tegangan Analog = ");
  Serial.print(Voltage);
  Serial.print(" _ ");

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Suhu: ");
  Serial.print(temperatureC);
  Serial.print(" °C | ");

  int digital = digitalRead(2);
  delay(50);
  Serial.print("Logic Digital = ");
  Serial.print(digital);
  float calibration_value = 7.0 - (3.0 * (-5.0 / 0.45) + 2.5);
  float ph_act = (Voltage - 4.3) * (-5.0 / 1.4) + ((calibration_value / 3.3) + 12);

  Serial.print(" _ pH = ");
  Serial.println(ph_act, 2);
  // serial print end

  // sending data to esp32 start
  byte ph_act_bytes[sizeof(float)];
  byte voltage_bytes[sizeof(float)];
  byte temp_bytes[sizeof(float)];
  byte average_bytes[sizeof(float)];
  byte TempL_bytes[sizeof(float)];

  memcpy(ph_act_bytes, &ph_act, sizeof(float));
  memcpy(voltage_bytes, &Voltage, sizeof(float));
  memcpy(temp_bytes, &temperatureC, sizeof(float));
  memcpy(average_bytes, &average, sizeof(float));
  memcpy(TempL_bytes, &TempL, sizeof(float));

  Wire.beginTransmission(0x75);
  Wire.write(ph_act_bytes, sizeof(float));
  Wire.write(voltage_bytes, sizeof(float));
  Wire.write(temp_bytes, sizeof(float));
  Wire.write(average_bytes, sizeof(float));
  Wire.write(TempL_bytes, sizeof(float));
  Wire.endTransmission();
  // sending data to esp32 end

  // lcd print start
  String newLcdWeightDisplay = "Wght  : " + String(average);
  if (newLcdWeightDisplay != lcdWeightDisplay) {
    lcdWeightDisplay = newLcdWeightDisplay;
    updateLCD();
  }

  lcd.setCursor(0, 0);
  lcd.print("TA:");
  lcd.print(temperatureC, 1);
  lcd.print("C |");
  lcd.print("TL:");
  lcd.print(TempL, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("pH:");
  lcd.print(ph_act, 2);
  lcd.print("  |");
  lcd.print("W :");
  lcd.print(average, 1);
  lcd.print(" Kg");

  lcd.setCursor(0, 2);
  lcd.print("GF1:");
  lcd.print(Voltage, 3);
  lcd.print("|");
  lcd.print("GF2:");
  lcd.print(Voltage, 3);

  lcd.setCursor(0, 3);
  lcd.print("St1:");
  lcd.print(storage, 1);
  lcd.print("    |");
  lcd.print("St2:");
  lcd.print(storage, 1); 

  }
}