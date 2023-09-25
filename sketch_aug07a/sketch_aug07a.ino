#include <Adafruit_ADS1X15.h>

#include <Wire.h>

#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define I2C_SDA 21  // GPIO21
#define I2C_SCL 22  // GPIO22
#define PH_SENSOR_PIN 34  // GPIO34 (Analog input)
#define DIGITAL_SENSOR_PIN 16  // GPIO16 (Digital input)
#define VOLTAGE_SENSOR_PIN 35  // GPIO35 (Analog input)
#define ONE_WIRE_BUS 4  // GPIO4 for OneWire interface

Adafruit_ADS1X15 ads;  // Library for ADS1015 ADC
LiquidCrystal_I2C lcd(0x75, 20, 4);  // LCD I2C address and dimensions
OneWire oneWire(ONE_WIRE_BUS);  // Setup OneWire instance to communicate with DS18B20 sensor
DallasTemperature sensors(&oneWire);  // Pass the OneWire reference to DallasTemperature library

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);  // Start I2C communication
  ads.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
  
  sensors.begin();  // Initialize DS18B20 temperature sensor
}

void loop() {
  int phValue = analogRead(PH_SENSOR_PIN);
  float temperature = readTemperatureFromDS18B20();
  bool digitalValue = digitalRead(DIGITAL_SENSOR_PIN);
  int voltageValue = ads.readADC_SingleEnded(0);

  lcd.setCursor(0, 0);
  lcd.print("pH: ");
  lcd.print(phValue);

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);

  lcd.setCursor(0, 2);
  lcd.print("Digital: ");
  lcd.print(digitalValue);

  lcd.setCursor(0, 3);
  lcd.print("Voltage: ");
  lcd.print(voltageValue);

  delay(1000);
}

float readTemperatureFromDS18B20() {
  sensors.requestTemperatures();  // Send command to get temperature readings
  float temperature = sensors.getTempCByIndex(0);  // Get temperature in Celsius
  return temperature;
}
