/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP32 chip.

  NOTE: This requires ESP32 support package:
    https://github.com/espressif/arduino-esp32

  Please be sure to select the right ESP32 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6ppTalDNd"
#define BLYNK_TEMPLATE_NAME "esp32"
#define BLYNK_AUTH_TOKEN            "r6S9mkVUmfGAdmJsEtDI356b_xKIooYp"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <SoftwareSerial.h>
#include <HTTPClient.h>

SoftwareSerial sim800lSerial(2, 3);

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "OPTIMASI SISTEM DIGITAL";
char pass[] = "osd191067";

void setup()
{

   sim800lSerial.begin(9600);
  // Debug console
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
  if (sim800lSerial.available()) {
    int sensorValue = sim800lSerial.parseInt();
    sendToBlynk(sensorValue);
  }
}

void sendToBlynk(int value) {
  HTTPClient http;

  // Construct the URL using your Blynk token and the value
  String url = "http://blynk-cloud.com/r6S9mkVUmfGAdmJsEtDI356b_xKIooYp/update/V1?value=" + String(value);

  // Send HTTP GET request to Blynk server
  http.begin(url);
  int httpResponseCode = http.GET();
  http.end();
}
