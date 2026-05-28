// ESP8266 Data Logger & 2-Channel Relay Controller via Google Sheets
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

#define RELAY_1 4      // Relay 1 terhubung ke GPIO 4 (D2)
#define RELAY_2 5      // Relay 2 terhubung ke GPIO 5 (D1)
#define DHTPIN 12      // Sensor DHT11 terhubung ke GPIO 12 (D6)
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Interval pengiriman data & query kontrol (default: 15 detik agar kontrol relay responsif)
const unsigned long postingInterval = 15 * 1000; 
unsigned long lastPostingTime = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println("\n--- ESP8266 Data Logger & Relay Controller ---");
  
  // Konfigurasi pin Relay sebagai Output
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  
  // Matikan relay saat awal start (LOW = OFF pada modul relay active-high)
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  
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
  // Kirim data & cek kontrol secara berkala
  if (millis() - lastPostingTime >= postingInterval || lastPostingTime == 0) {
    if (WiFi.status() == WL_CONNECTED) {
      sendAndCheckData();
    } else {
      Serial.println("WiFi terputus! Pengiriman data ditunda.");
    }
  }
}

void sendAndCheckData() {
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
  
  Serial.println("Mengirim data ke Google Sheets & mengecek status relay...");
  
  // Gunakan WiFiClientSecure karena Google Apps Script mewajibkan HTTPS
  WiFiClientSecure client;
  client.setInsecure(); // Bypass validasi sertifikat root SSL agar tidak kadaluarsa
  
  HTTPClient http;
  
  // Inisialisasi HTTPClient dengan URL dan client secure
  if (http.begin(client, url)) {
    // SANGAT PENTING: Aktifkan follow redirect karena Google Apps Script 
    // mengalihkan request (302 Redirect) ke server penyimpanan Google
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    
    // Kirim HTTP GET request
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      Serial.printf("HTTP Response Code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        payload.trim(); // Hapus whitespace atau newlines
        Serial.println("Respon dari Sheets: " + payload);
        
        // Parse respon status relay (format: "relay1,relay2", contoh: "1,0")
        int commaIndex = payload.indexOf(',');
        if (commaIndex > 0) {
          String r1Status = payload.substring(0, commaIndex);
          String r2Status = payload.substring(commaIndex + 1);
          
          int r1Val = r1Status.toInt();
          int r2Val = r2Status.toInt();
          
          // Update status fisik relay
          digitalWrite(RELAY_1, r1Val == 1 ? HIGH : LOW);
          digitalWrite(RELAY_2, r2Val == 1 ? HIGH : LOW);
          
          Serial.printf("Relay Diperbarui -> R1: %s, R2: %s\n", 
                        r1Val == 1 ? "ON" : "OFF", 
                        r2Val == 1 ? "ON" : "OFF");
        }
        
        lastPostingTime = millis(); // Update waktu terakhir sukses
      }
    } else {
      Serial.printf("HTTP Request gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
  } else {
    Serial.println("Gagal menghubungkan ke server!");
  }
}
