// model
#define SIM800L_IP5306_VERSION_20190610

#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon

#include "utilities.h"

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800          // Modem is SIM800
#define TINY_GSM_RX_BUFFER      1024   // Set RX buffer to 1Kb

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

#define SMS_TARGET  "+6285888203401, +6285156808903, +6285238378672"
// #define CALL_TARGET "+6285156808903"

// unsigned long previousMillis = 0;
// const long interval = 10000; // Interval 10 detik

void setup()
{
    SerialMon.begin(115200);
    delay(10);

    if (!setupPMU()) {
        Serial.println("Setting power error");
    }

    setupModem();
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(6000);
}

void loop()
{
    // unsigned long currentMillis = millis();

    // if (currentMillis - previousMillis >= interval) {
        // previousMillis = currentMillis;

        SerialMon.println("Initializing modem...");
        // modem.restart();

        delay(10000);

        String imei = modem.getIMEI();
        DBG("IMEI:", imei);
        Serial.print(imei);

        String message = " Testing bos";
        char* token;
        char* numbers = strdup(SMS_TARGET);

        token = strtok(numbers, ", ");
        while (token != NULL) {
            bool res = modem.sendSMS(token, message);
            DBG("SMS to", token, ":", res ? "OK" : "fail");
            token = strtok(NULL, ", ");
            delay(1000);
        }

        free(numbers); // Membebaskan memori
    // }

    // modem.maintain(); 
}
