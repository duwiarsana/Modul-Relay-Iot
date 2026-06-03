// IoT Relay Module ESP-NOW Transmitter (Remote Control) ESP8266
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <espnow.h>

// MAC Address dari ESP8266 Receiver (Dapatkan dari serial monitor modul receiver Anda)
// GANTI DENGAN MAC ADDRESS RECEIVER ANDA!
uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Definisi PIN tombol untuk kontrol nirkabel
#define BUTTON1 0  // D3 (GPIO 0) pada Wemos D1 Mini - Kontrol Relay 1
#define BUTTON2 2  // D4 (GPIO 2) pada Wemos D1 Mini - Kontrol Relay 2

// Struktur data pesan (Harus sama persis dengan yang ada di Receiver)
typedef struct struct_message {
    int relayNum; // 1 atau 2
    int command;  // 0 = OFF, 1 = ON, 2 = TOGGLE
} struct_message;

struct_message myData;

// Variabel penanganan debouncing tombol
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
const unsigned long debounceDelay = 250; // Jeda debouncing dalam milidetik

// Callback saat data terkirim
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Status Pengiriman Paket: ");
  if (sendStatus == 0){
    Serial.println("Berhasil Terkirim (Ack received)");
  }
  else{
    Serial.println("Gagal Terkirim (No ack)");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\n--- ESP-NOW Transmitter (Remote) Mode ---");

  // Inisialisasi tombol sebagai input dengan internal pull-up
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  // Set WiFi ke Station Mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Inisialisasi ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Gagal menginisialisasi ESP-NOW!");
    return;
  }

  // Set role sebagai pemancar
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  
  // Registrasi callback status pengiriman data
  esp_now_register_send_cb(OnDataSent);
  
  // Daftarkan peer (Receiver)
  int addStatus = esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  if (addStatus == 0) {
    Serial.println("Berhasil mendaftarkan alamat MAC Receiver!");
  } else {
    Serial.println("Gagal mendaftarkan alamat MAC Receiver!");
  }
}

void loop() {
  // Cek tombol 1 (Relay 1 Toggle)
  if (digitalRead(BUTTON1) == LOW) {
    if (millis() - lastDebounceTime1 > debounceDelay) {
      lastDebounceTime1 = millis();
      
      myData.relayNum = 1;
      myData.command = 2; // Perintah: TOGGLE

      Serial.println("Mengirim perintah: Toggle Relay 1");
      esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
    }
  }

  // Cek tombol 2 (Relay 2 Toggle)
  if (digitalRead(BUTTON2) == LOW) {
    if (millis() - lastDebounceTime2 > debounceDelay) {
      lastDebounceTime2 = millis();
      
      myData.relayNum = 2;
      myData.command = 2; // Perintah: TOGGLE

      Serial.println("Mengirim perintah: Toggle Relay 2");
      esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
    }
  }
  
  delay(10); // Istirahat kecil untuk stabilitas CPU ESP8266
}
