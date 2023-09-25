// Definisikan versi perangkat yang digunakan (opsional)
// #define SIM800L_IP5306_VERSION_20190610
// #define SIM800L_AXP192_VERSION_20200327
// #define SIM800C_AXP192_VERSION_20200609
#define SIM800L_IP5306_VERSION_20200811

// Tentukan konsol serial untuk pencetakan debug jika diperlukan
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG          SerialMon
#include <string.h>
#include "utilities.h"

// Set serial untuk konsol debug (ke Serial Monitor, kecepatan default 115200)
#define SerialMon Serial
// Set serial untuk perintah AT (ke modul)
#define SerialAT  Serial1

#define SIM800 Serial2

// Konfigurasikan perpustakaan TinyGSM
#define TINY_GSM_MODEM_SIM800          // Modem adalah SIM800
#define TINY_GSM_RX_BUFFER      1024   // Set buffer RX ke 1 KB

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

char* lower = "";
char* response = " ";
String res = "";
bool new_data = 0;

#define SMS_TARGET  "+6285883707299"
char* msg = "+cmt: \"+6285883707299\"";

void setup()
{

  // Set kecepatan baud konsol
  SerialMon.begin(115200);

  // Mulai manajemen daya
  if (setupPMU() == false) {
    Serial.println("Kesalahan pengaturan daya");
  }

  // Beberapa operasi awal
  setupModem();

  // Set kecepatan baud dan pin UART untuk modul GSM
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  delay(6000);

  SerialMon.println("Menginisialisasi modem...");
  modem.restart();
  modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Info Modem: ");
  SerialMon.println(modemInfo);
  delay(1000);

  SerialAT.println("AT"); // Setelah uji tautan berhasil, akan kembali ke OK
  updateSerial();

  SerialAT.println("AT+CMGF=1"); // Konfigurasi mode TEKS
  updateSerial();
  SerialAT.println("AT+CNMI=1,2,0,0,0"); // Menentukan cara pesan SMS yang baru tiba harus ditangani
  updateSerial();
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    SerialAT.write(Serial.read()); // Majukan apa yang diterima Serial ke Port Serial Perangkat Lunak
  }
  while (SerialAT.available())
  {
    //Serial.write(SerialAT.read());// Majukan apa yang diterima Port Serial Perangkat Lunak ke Port Serial

    char add = SerialAT.read();
    res = res + add;
    delay(1);
    new_data = 1;
  }

  if (new_data) {
    response = &res[0];
    //------------------------------------------------- Mengonversi setiap karakter String ke bentuk huruf kecil
    const int length = strlen( response ); // dapatkan panjang teks
    lower = ( char* )malloc( length + 1 ); // alokasikan 'length' byte + 1 (untuk terminator nol) dan ubah ke char*
    lower[ length ] = 0; // atur byte terakhir ke terminator nol
    //------------------------------------------------- salin semua byte karakter ke buffer baru menggunakan tolower
    for ( int i = 0; i < length; i++ )
    {
      lower[ i ] = tolower( response[ i ] );
    }
    Serial.println(lower);// cetak String dalam bentuk karakter kecil
    Serial.println("\n");

    res = "";
    lower = "";
    new_data = 0;

  }
}
