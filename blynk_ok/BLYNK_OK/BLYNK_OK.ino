#include <Wire.h>

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

#define SIM800_RX         26
#define SIM800_TX         27
#define SIM800_RST        5
#define SIM800_PWRKEY     4
#define SIM800_POWER_ON   23

#define SMS_TARGET ""  //+6285888203401, +6285156808903

float received_ph_act = 0.0;
float received_voltage = 0.0;
float received_tempInCelsius = 0.0;
float received_average = 0.0;
float received_TempL = 0.0;
uint16_t received_gasValue = 0;

TinyGsm modem(Serial2);
TinyGsmClient client(modem);

unsigned long previousMillis = 0;
const unsigned long interval = 1 * 60 * 1000; // Interval 1 menit

void setup() {
  Serial.begin(115200);
  Wire.begin(0x75);
  Wire.onReceive(receiveData);

  pinMode(SIM800_PWRKEY, OUTPUT);
  pinMode(SIM800_POWER_ON, OUTPUT);

  // Inisialisasi SIM800
  digitalWrite(SIM800_POWER_ON, HIGH);
  digitalWrite(SIM800_PWRKEY, LOW);
  delay(1000);
  digitalWrite(SIM800_PWRKEY, HIGH);
  delay(2000);

  Serial2.begin(115200, SERIAL_8N1, SIM800_RX, SIM800_TX);
  delay(6000);

  if (!modem.restart()) {
    Serial.println("Fail detect SIM Card!");
    while (true);
  } else {
    Serial.println("SIM Card Ready to use !");
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    char* token;
    char* targetNumbers = strdup(SMS_TARGET);

    token = strtok(targetNumbers, ", ");
    while (token != NULL) {
      sendSMS(token);
      token = strtok(NULL, ", ");
      delay(1000);
    }

    free(targetNumbers);
  }

}

void receiveData(int byteCount) {

  byte received_ph_act_bytes[sizeof(float)];
  byte received_voltage_bytes[sizeof(float)];
  byte received_temp_bytes[sizeof(float)];
  byte received_average_bytes[sizeof(float)];
  byte received_tempL_bytes[sizeof(float)];
  byte received_gasValue_bytes[sizeof(uint16_t)];
  // Baca byte-byte yang diterima
  Wire.readBytes(received_ph_act_bytes, sizeof(float));
  Wire.readBytes(received_voltage_bytes, sizeof(float));
  Wire.readBytes(received_temp_bytes, sizeof(float));
  Wire.readBytes(received_average_bytes, sizeof(float));
  Wire.readBytes(received_tempL_bytes, sizeof(float));
  Wire.readBytes(received_gasValue_bytes, sizeof(uint16_t));

  // Konversi byte-byte ke float dan uint16_t
  memcpy(&received_ph_act, received_ph_act_bytes, sizeof(float));
  memcpy(&received_voltage, received_voltage_bytes, sizeof(float));
  memcpy(&received_tempInCelsius, received_temp_bytes, sizeof(float));
  memcpy(&received_average, received_average_bytes, sizeof(float));
  memcpy(&received_TempL, received_tempL_bytes, sizeof(float));
  memcpy(&received_gasValue, received_gasValue_bytes, sizeof(uint16_t));

  // Print the received values
  Serial.print("Received pH: ");
  Serial.println(received_ph_act);

  Serial.print("Received Temp Liquid: ");
  Serial.println(received_voltage);

  Serial.print("Received temp Ambien: ");
  Serial.println(received_tempInCelsius);

  Serial.print("Received Weight: ");
  Serial.println(received_average);

  Serial.print("Received Gas Flow: ");
  Serial.println(received_gasValue);

  delay(1000);
}

void sendSMS(const char* targetNumber) {
  String message = "pH: " + String(received_ph_act, 2) + "\n";
  message += "Temp Liquid: " + String(received_voltage, 2) + "\n";
  message += "temperature Ambien: " + String(received_tempInCelsius, 2) + "\n";
  message += "Weight: " + String(received_average, 2) + "\n";
  message += "Temp Liquid: " + String(received_TempL, 2) + "\n";
  message += "Gas Flow: " + String(received_gasValue) + "\n";

  if (modem.sendSMS(targetNumber, message)) {
    Serial.print("Pesan SMS terkirim ke ");
    Serial.println(targetNumber);
  } else {
    Serial.print("Gagal mengirim SMS ke ");
    Serial.println(targetNumber);
  }
}
