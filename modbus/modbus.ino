#include <ModbusMaster.h>

ModbusMaster node;

void setup() {
  Serial.begin(4800);
  node.begin(4, Serial);
}

void loop() {
  uint8_t result;
  uint16_t data[2];

  result = node.readHoldingRegisters(0x0000, 2);

  if (result == node.ku8MBSuccess) {
    data[0] = node.getResponseBuffer(0);
    data[1] = node.getResponseBuffer(1);

    // Konversi nilai aliran kumulatif
    uint32_t cumulativeFlow = (uint32_t)data[1] << 16 | data[0];

    Serial.print("Aliran Kumulatif: ");
    Serial.println(cumulativeFlow);
  } else {
    Serial.println("Gagal membaca gas meter");
  }

  delay(1000);
}
