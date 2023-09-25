#include "HX711.h"

HX711 scale;

uint8_t dataPin = 6;
uint8_t clockPin = 7;

const int numReadings = 10; // rata' pembacaan
float readings[numReadings];
int readingIndex = 0;
float total = 0;

void setup() {
  Serial.begin(115200);
  scale.begin(dataPin, clockPin);

  scale.set_scale(14.92440); // kalibrasi 
  scale.tare(20);
}

void loop() {
  // convert gram ke kg
  if (scale.is_ready()) {

    float weight = scale.get_units(1) / 1000;
    total -= readings[readingIndex];
    readings[readingIndex] = weight;
    total += weight;

    readingIndex = (readingIndex + 1) % numReadings;

    float average = total / numReadings;

    Serial.print("Berat: ");
    Serial.println(average, 3);

    delay(200);
  }
}
