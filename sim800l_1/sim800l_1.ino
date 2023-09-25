// #define SIM800L_AXP192_VERSION_20200327
#define SIM800L_IP5306_VERSION_20200811

// Konfigurasikan perpustakaan TinyGSM
#define TINY_GSM_MODEM_SIM800          // Modem adalah SIM800
#define TINY_GSM_RX_BUFFER      1024   // Set buffer RX ke 1 KB

#include <TinyGsmClient.h>

// Pinout untuk TTGO ESP32 SIM800L
#define MODEM_RST           5
#define MODEM_TX            26
#define MODEM_RX            27
// #define MODEM_POWER_ON    23
#define MODEM_POWER_KEY     4
#define MODEM_SIM800_POWER  23

// Inisialisasi objek modem SIM800L
TinyGsm modem(Serial2);

// Nomor penerima SMS
const char* smsTarget = "+6285883707299";

void setup() {
  Serial.begin(115200);
  delay(200);

  // Aktifkan modem SIM800L
  pinMode(MODEM_SIM800_POWER, OUTPUT);
  digitalWrite(MODEM_SIM800_POWER, HIGH);
  delay(1000);
  digitalWrite(MODEM_SIM800_POWER, LOW);

  Serial2.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Tunggu sampai modul SIM800L siap (Anda dapat mengaktifkan bagian ini jika diperlukan)
  while (!modem.waitForNetwork()) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  // Kirim SMS
  sendSMS(smsTarget, "haloo cok");

  Serial.println("Selesai.");
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
}

void sendSMS(const char* number, const char* message) {
  Serial.println("Mengirim SMS...");
  if (modem.sendSMS(number, message)) {
    Serial.println("SMS berhasil dikirim.");
  } else {
    Serial.println("Gagal mengirim SMS.");
  }
}