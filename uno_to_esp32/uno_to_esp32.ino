void setup() {
  Serial.begin(9600);  // Inisialisasi komunikasi serial dengan kecepatan 9600 bps
}

void loop() {
  Serial.println("Hello from Arduino Uno!");  // Mengirim pesan melalui komunikasi serial
  delay(1000);  // Delay 1 detik
}
