// IoT Relay Module Alexa Local Control ESP8266
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#define ESPALEXA_MAX_DEVICES 2 // Membatasi alokasi RAM untuk device Espalexa
#include <Espalexa.h>
#include <DHT.h>

// Ganti dengan kredensial WiFi Anda
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

// Definisi GPIO sesuai dengan modul relay IoT
#define RELAY1 4   // GPIO 4 (D2) - Relay 1 (Aktif HIGH)
#define RELAY2 5   // GPIO 5 (D1) - Relay 2 (Aktif HIGH)
#define DHTPIN 12  // GPIO 12 (D6) - Sensor DHT11
#define DHTTYPE DHT11

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Objek Espalexa
Espalexa espalexa;

// Variabel untuk interval pembacaan DHT11 secara non-blocking
unsigned long lastReadTime = 0;
const unsigned long readInterval = 5000; // Baca DHT setiap 5 detik

// Callback function untuk Relay 1 ketika dikontrol oleh Alexa
void relay1Callback(uint8_t brightness) {
  // Alexa mengirimkan nilai 0 untuk OFF, dan 255 untuk ON.
  // Jika device diatur tingkat keterangannya (dim), nilai berkisar 1 - 254.
  if (brightness == 0) {
    digitalWrite(RELAY1, LOW);
    Serial.println("Alexa: Relay 1 dimatikan (OFF)");
  } else {
    digitalWrite(RELAY1, HIGH);
    Serial.print("Alexa: Relay 1 dinyalakan (ON) - Level: ");
    Serial.println(brightness);
  }
}

// Callback function untuk Relay 2 ketika dikontrol oleh Alexa
void relay2Callback(uint8_t brightness) {
  if (brightness == 0) {
    digitalWrite(RELAY2, LOW);
    Serial.println("Alexa: Relay 2 dimatikan (OFF)");
  } else {
    digitalWrite(RELAY2, HIGH);
    Serial.print("Alexa: Relay 2 dinyalakan (ON) - Level: ");
    Serial.println(brightness);
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\n--- IoT Relay Module Alexa Control ---");

  // Konfigurasi pin relay sebagai output
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  
  // Set kondisi awal relay mati (LOW)
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // Inisialisasi sensor DHT11
  dht.begin();

  // Koneksi ke jaringan WiFi
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Tambahkan virtual device ke Espalexa
  // Nama pertama adalah nama perangkat yang akan terdeteksi di aplikasi Alexa
  espalexa.addDevice("Relay Satu", relay1Callback);
  espalexa.addDevice("Relay Dua", relay2Callback);

  // Jalankan server Espalexa
  espalexa.begin();
  
  Serial.println("Espalexa telah dimulai!");
  Serial.println("Silakan lakukan 'Discovery Devices' pada aplikasi Amazon Alexa Anda.");
}

void loop() {
  // Handle komunikasi Espalexa
  espalexa.loop();
  
  // Membaca sensor DHT11 secara berkala tanpa menggunakan delay()
  unsigned long currentMillis = millis();
  if (currentMillis - lastReadTime >= readInterval) {
    lastReadTime = currentMillis;
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    // Periksa apakah pembacaan sensor berhasil
    if (isnan(h) || isnan(t)) {
      Serial.println("Gagal membaca dari sensor DHT11!");
    } else {
      Serial.print("Suhu: ");
      Serial.print(t);
      Serial.print(" °C | Kelembaban: ");
      Serial.print(h);
      Serial.println(" %");
    }
  }
  
  delay(1); // Delay kecil untuk kestabilan background task ESP8266
}
