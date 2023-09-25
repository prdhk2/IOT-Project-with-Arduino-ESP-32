#include <Blynk.h>

#include <WiFi.h>
#include <Wire.h>

#define BLYNK_TEMPLATE_ID "TMPL6ppTalDNd"
#define BLYNK_TEMPLATE_NAME "esp32"

const char* ssid = "OPTIMASI SISTEM DIGITAL";
const char* password = "osd191067";

unsigned int pH_Value = 0;
float Voltage = 0.0;

void setup() {
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
}
void loop() {
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
      Serial.println("WiFi connected");

        pH_Value = 0;
        for (int test_cycle = 1; test_cycle <= 10; test_cycle++) {
          pH_Value = pH_Value + analogRead(32);
          delay(10);
        }
        
        float analog = (float)pH_Value / 10.0;
        delay(50);
        
        Voltage = analog * (5.0 / 1023.0);
        Serial.print("Tegangan Analog = ");
        Serial.print(Voltage);
        Serial.print(" _ ");
        
        int temp = 0;
        temp = analogRead(4);
        delay(50);
        Serial.print("Temperature = ");
        Serial.print(temp);
        Serial.print(" _ ");
        
        // Calculate the calibration value to set pH 7 at 3V
        float calibration_value = 7.0 - (3.0 * (-5.0 / 0.45) + 2.5);
        float ph_act = (Voltage - 2.5) * (-5.0 / 0.45) + calibration_value / 2.2;
        
        Serial.print(" _ pH = ");
        Serial.println(ph_act, 2);
        
        delay(500);
}
