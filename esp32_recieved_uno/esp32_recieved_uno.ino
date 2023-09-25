void setup() {
  Serial.begin(9600);  // Inisialisasi komunikasi serial dengan kecepatan 9600 bps
}

void loop() {
  if (Serial.available()) {  // Cek apakah data tersedia di komunikasi serial
    String receivedData = Serial.readString();  // Membaca data yang diterima
    Serial.print("Received data: ");
    Serial.println(receivedData);  // Menampilkan data yang diterima
  }
}
