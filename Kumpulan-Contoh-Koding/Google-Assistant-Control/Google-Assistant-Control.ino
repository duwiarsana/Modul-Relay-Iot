// IoT Relay Module Google Assistant Control ESP8266
// Menggunakan Platform Cloud Sinric Pro (Gratis)
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#define SINRICPRO_NOSSL // Opsional: Nonaktifkan SSL jika RAM ESP8266 sangat terbatas
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include "SinricProTemperaturesensor.h"
#include <DHT.h>

// Ganti dengan kredensial WiFi Anda
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

// Dapatkan kredensial ini dari portal sinric.pro
#define APP_KEY           "YOUR-SINRICPRO-APP-KEY"      
#define APP_SECRET        "YOUR-SINRICPRO-APP-SECRET"   
#define SWITCH_ID_1       "YOUR-DEVICE-ID-FOR-RELAY1"  // ID Perangkat Relay 1
#define SWITCH_ID_2       "YOUR-DEVICE-ID-FOR-RELAY2"  // ID Perangkat Relay 2
#define TEMP_SENSOR_ID    "YOUR-DEVICE-ID-FOR-DHT11"   // ID Perangkat Sensor DHT11

// Definisi GPIO sesuai dengan modul relay IoT
#define RELAY1 4   // GPIO 4 (D2) - Relay 1
#define RELAY2 5   // GPIO 5 (D1) - Relay 2
#define DHTPIN 12  // GPIO 12 (D6) - Sensor DHT11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Variabel untuk melacak data sensor DHT11
float temperature;
float humidity;
float lastTemperature = 0.0;
float lastHumidity = 0.0;
unsigned long lastEventTime = 0;
const unsigned long eventInterval = 60000; // Kirim data sensor ke cloud setiap 60 detik

// Callback saat Relay 1 diubah statusnya via Google Assistant / Sinric Pro
bool onPowerStateRelay1(const String &deviceId, bool &state) {
  Serial.printf("Google Assistant: Relay 1 diatur ke %s\r\n", state ? "ON" : "OFF");
  digitalWrite(RELAY1, state ? HIGH : LOW);
  return true; // Menandakan perintah berhasil diproses
}

// Callback saat Relay 2 diubah statusnya via Google Assistant / Sinric Pro
bool onPowerStateRelay2(const String &deviceId, bool &state) {
  Serial.printf("Google Assistant: Relay 2 diatur ke %s\r\n", state ? "ON" : "OFF");
  digitalWrite(RELAY2, state ? HIGH : LOW);
  return true; // Menandakan perintah berhasil diproses
}

// Fungsi untuk membaca DHT11 dan mengirimkan hasilnya ke Sinric Pro
void handleTemperatureSensor() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastEventTime >= eventInterval) {
    lastEventTime = currentMillis;

    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Gagal membaca data dari sensor DHT11!");
      return;
    }

    Serial.printf("Sensor DHT11 - Suhu: %2.1f °C, Kelembaban: %2.1f %%\r\n", temperature, humidity);

    // Kirim data hanya jika nilainya berubah untuk menghemat bandwidth
    if (temperature != lastTemperature || humidity != lastHumidity) {
      SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];
      mySensor.sendTemperatureEvent(temperature, humidity);
      lastTemperature = temperature;
      lastHumidity = humidity;
      Serial.println("Data sensor dikirim ke cloud Sinric Pro.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\n--- IoT Relay Module Google Assistant Control ---");

  // Inisialisasi Pin Relay
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // Inisialisasi Sensor DHT11
  dht.begin();

  // Koneksi WiFi
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

  // Setup Perangkat Relay 1 di Sinric Pro
  SinricProSwitch &mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerStateRelay1);

  // Setup Perangkat Relay 2 di Sinric Pro
  SinricProSwitch &mySwitch2 = SinricPro[SWITCH_ID_2];
  mySwitch2.onPowerState(onPowerStateRelay2);

  // Setup Perangkat Sensor Suhu di Sinric Pro
  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];

  // Setup Koneksi Sinric Pro
  SinricPro.onConnected([](){ 
    Serial.println("Terhubung ke cloud Sinric Pro!"); 
  });
  SinricPro.onDisconnected([](){ 
    Serial.println("Terputus dari cloud Sinric Pro."); 
  });
  
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void loop() {
  // Handle komunikasi data dengan Sinric Pro
  SinricPro.handle();
  
  // Handle pembacaan dan pengiriman data suhu/kelembaban
  handleTemperatureSensor();
}
