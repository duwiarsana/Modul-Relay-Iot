// ESP8266 Data Logger to Google Sheets (DHT11)
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

// WiFi credentials (ubah sesuai dengan WiFi Anda)
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

// Google Apps Script Web App URL (ganti dengan URL Web App hasil deploy Anda)
const char* gasWebUrl = "https://script.google.com/macros/s/YOUR_GAS_WEB_APP_ID/exec";

#define DHTPIN 12      // Sensor DHT11 terhubung ke GPIO 12 (D6)
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Interval pengiriman data (default: 10 menit = 600000 ms)
const unsigned long postingInterval = 10 * 60 * 1000;
unsigned long lastPostingTime = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println("\n--- ESP8266 Data Logger to Google Sheets ---");
  
  // Inisialisasi sensor DHT
  dht.begin();
  
  // Memulai koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Kirim data secara berkala atau kirim pertama kali saat startup setelah sukses koneksi
  if (millis() - lastPostingTime >= postingInterval || lastPostingTime == 0) {
    if (WiFi.status() == WL_CONNECTED) {
      sendDataToGoogleSheets();
    } else {
      Serial.println("WiFi terputus! Pengiriman data ditunda.");
    }
  }
}

void sendDataToGoogleSheets() {
  // Baca nilai suhu dan kelembaban
  float temp = dht.readTemperature();
  float humi = dht.readHumidity();

  // Validasi pembacaan sensor
  if (isnan(temp) || isnan(humi)) {
    Serial.println("Gagal membaca dari sensor DHT11!");
    return;
  }

  Serial.printf("Suhu: %.1f *C, Kelembaban: %.1f %%\n", temp, humi);

  // Buat URL lengkap dengan query parameter
  // format: https://script.google.com/macros/s/ID/exec?temp=VAL&humi=VAL
  String url = String(gasWebUrl) + "?temp=" + String(temp, 1) + "&humi=" + String(humi, 1);
  
  Serial.println("Mengirim data ke Google Sheets...");
  
  // Gunakan WiFiClientSecure karena Google Apps Script mewajibkan HTTPS (SSL/TLS)
  WiFiClientSecure client;
  
  // Set insecure agar tidak perlu memvalidasi sertifikat SSL root (tidak repot mengurus sertifikat kadaluarsa)
  client.setInsecure();
  
  HTTPClient http;
  
  // Inisialisasi HTTPClient dengan URL dan client secure
  if (http.begin(client, url)) {
    // SANGAT PENTING: Aktifkan follow redirect karena Google Apps Script
    // mengarahkan (302 redirect) request ke server penyimpanan data Google
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    
    // Kirim HTTP GET request
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      Serial.printf("HTTP Response Code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Response dari server: " + payload);
        lastPostingTime = millis(); // Update waktu terakhir pengiriman sukses
      }
    } else {
      Serial.printf("HTTP Request gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
  } else {
    Serial.println("Gagal menghubungkan ke server!");
  }
}
