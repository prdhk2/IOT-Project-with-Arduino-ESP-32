#define BLYNK_TEMPLATE_ID "TMPL6ppTalDNd"
#define BLYNK_TEMPLATE_NAME "esp32"
#define BLYNK_AUTH_TOKEN "r6S9mkVUmfGAdmJsEtDI356b_xKIooYp"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "r6S9mkVUmfGAdmJsEtDI356b_xKIooYp";  // Your Blynk auth token

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, "OPTIMASI SISTEM DIGITAL", "osd191067");
}

void loop() {
  Blynk.run();
  if (Serial.available()) {
    int sensorData = Serial.parseInt();
    Blynk.virtualWrite(V5, sensorData);  // Send data to Blynk virtual pin V5
  }
}
