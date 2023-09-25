#include <Wire.h>
#include <EEPROM.h>

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

#define SIM800_RX         26
#define SIM800_TX         27
#define SIM800_RST        5
#define SIM800_PWRKEY     4
#define SIM800_POWER_ON   23

#define No_HP ""  //+6285888203401, +6285156808903, +6285238378672

float received_ph_act = 0.0;
// float received_tempL = 0.0;
float received_tempInCelsius = 0.0;
float received_Berat = 0.0;
float received_tempL = 0.0;
float received_gas = 0;

TinyGsm modem(Serial2);
TinyGsmClient client(modem);

unsigned long previousMillis = 0;
const unsigned long interval = 1 * 60 * 1000; // menit

#define BERAT_TERKHIR_TERTINGGI 0
float BeratTertinggi = 0.0;

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
    Serial.println("SIM Card TIDAK TERDETEKSI");
    while (true);
  } else {
    Serial.println("SIM Card TERDETEKSI");
  }

  EEPROM.get(BERAT_TERKHIR_TERTINGGI, BeratTertinggi);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    char* token;
    char* No_Tujuan = strdup(No_HP);

    token = strtok(No_Tujuan, ", ");
    while (token != NULL) {
      sendSMS(token);
      token = strtok(NULL, ", ");
      delay(1000);
    }

    free(No_Tujuan);
  }
}

void receiveData(int byteCount) {
  byte received_ph_act_bytes[sizeof(float)];
  byte received_tempL_bytes[sizeof(float)];
  byte received_temp_bytes[sizeof(float)];
  byte received_Berat_bytes[sizeof(float)];
  // byte received_tempL_bytes[sizeof(float)];
  byte received_gas_bytes[sizeof(float)];

  Wire.readBytes(received_ph_act_bytes, sizeof(float));
  Wire.readBytes(received_tempL_bytes, sizeof(float));
  Wire.readBytes(received_temp_bytes, sizeof(float));
  Wire.readBytes(received_Berat_bytes, sizeof(float));
  // Wire.readBytes(received_tempL_bytes, sizeof(float));
  Wire.readBytes(received_gas_bytes, sizeof(float));

  memcpy(&received_ph_act, received_ph_act_bytes, sizeof(float));
  memcpy(&received_tempL, received_tempL_bytes, sizeof(float));
  memcpy(&received_tempInCelsius, received_temp_bytes, sizeof(float));
  memcpy(&received_Berat, received_Berat_bytes, sizeof(float));
  // memcpy(&received_tempL, received_tempL_bytes, sizeof(float));
  memcpy(&received_gas, received_gas_bytes, sizeof(float));

  if (received_Berat > BeratTertinggi) {
    BeratTertinggi = received_Berat;

    EEPROM.put(BERAT_TERKHIR_TERTINGGI, BeratTertinggi);
    EEPROM.commit();
  }
   // Print the received values
  Serial.print("Received pH: ");
  Serial.println(received_ph_act);
  Serial.print("Received Temp Liquid: ");
  Serial.println(received_tempL);
  Serial.print("Received temp Ambien: ");
  Serial.println(received_tempInCelsius);
  Serial.print("Received Weight: ");
  Serial.println(received_Berat);
  Serial.print("Received Gas Flow: ");
  Serial.println(received_gas);
  delay(1000);
}

void sendSMS(const char* No_Tujuan) {
  
  String message = " Laporan Test IOT PT. OSDD senin 11/9/2023" " | ";
  message += "pH : "            + String(received_ph_act, 2) + " | ";
  message += "Temp Liquid : "         + String(received_tempL, 2) + " 'C | ";
  message += "Temp Ambien : "         + String(received_tempInCelsius, 2) + " 'C | ";
  message += "Timbangan Terakhir : "  + String(BeratTertinggi, 2) + " Kg | ";
  message += "Gas Flow : "            + String(received_gas) + " m3";

  if (modem.sendSMS(No_Tujuan, message)) {
    Serial.print("Pesan SMS terkirim ke ");
    Serial.println(No_Tujuan);
  } else {
    Serial.print("Gagal mengirim SMS ke ");
    Serial.println(No_Tujuan);
  }
}
