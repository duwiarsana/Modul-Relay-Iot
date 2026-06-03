// IoT Relay Module ESP-NOW Receiver ESP8266
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <espnow.h>

// Definisi GPIO sesuai dengan modul relay IoT
#define RELAY1 4   // GPIO 4 (D2) - Relay 1
#define RELAY2 5   // GPIO 5 (D1) - Relay 2

// Struktur data pesan (Harus sama persis dengan yang dikirim oleh Transmitter)
typedef struct struct_message {
    int relayNum; // 1 atau 2
    int command;  // 0 = OFF, 1 = ON, 2 = TOGGLE
} struct_message;

struct_message incomingData;

// Fungsi callback ketika data diterima dari Transmitter
void OnDataRecv(uint8_t * mac, uint8_t *incomingDataPtr, uint8_t len) {
  // Salin data yang masuk ke variabel struct
  memcpy(&incomingData, incomingDataPtr, sizeof(incomingData));
  
  Serial.print("Menerima perintah dari MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  Serial.printf("Perintah: Relay %d -> Command %d (0:OFF, 1:ON, 2:TOGGLE)\n", incomingData.relayNum, incomingData.command);

  // Proses kontrol relay
  if (incomingData.relayNum == 1) {
    if (incomingData.command == 0) {
      digitalWrite(RELAY1, LOW);
      Serial.println("Relay 1 dimatikan.");
    } else if (incomingData.command == 1) {
      digitalWrite(RELAY1, HIGH);
      Serial.println("Relay 1 dinyalakan.");
    } else if (incomingData.command == 2) {
      digitalWrite(RELAY1, !digitalRead(RELAY1));
      Serial.printf("Relay 1 di-toggle menjadi %s.\n", digitalRead(RELAY1) ? "ON" : "OFF");
    }
  } 
  else if (incomingData.relayNum == 2) {
    if (incomingData.command == 0) {
      digitalWrite(RELAY2, LOW);
      Serial.println("Relay 2 dimatikan.");
    } else if (incomingData.command == 1) {
      digitalWrite(RELAY2, HIGH);
      Serial.println("Relay 2 dinyalakan.");
    } else if (incomingData.command == 2) {
      digitalWrite(RELAY2, !digitalRead(RELAY2));
      Serial.printf("Relay 2 di-toggle menjadi %s.\n", digitalRead(RELAY2) ? "ON" : "OFF");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\n--- ESP-NOW Receiver Mode ---");

  // Inisialisasi pin relay
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // Set WiFi ke Station Mode agar bisa menggunakan ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Print MAC Address modul ini ke Serial Monitor (Salin MAC ini untuk dipasang di Transmitter)
  Serial.print("PENTING! MAC Address Receiver Anda: ");
  Serial.println(WiFi.macAddress());

  // Inisialisasi ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Gagal menginisialisasi ESP-NOW!");
    return;
  }
  
  // Set role sebagai Combo (bisa mengirim dan menerima)
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  
  // Registrasi fungsi callback penerima data
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.println("Siap menerima sinyal kontrol...");
}

void loop() {
  // Tidak ada blocking/delay di loop agar performa penangkapan sinyal nirkabel responsif
}
