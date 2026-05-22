// Telegram Bot Controller & DHT11 Monitor ESP8266 (UniversalTelegramBot Library)
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana
//
// Library yang dibutuhkan:
// 1. UniversalTelegramBot (by Brian Lough) - Untuk interface Telegram API
// 2. ArduinoJson (by Benoit Blanchon) - DIHARUSKAN oleh UniversalTelegramBot (Gunakan versi 6.x.x)
// 3. DHT Sensor Library (by Adafruit) - Untuk sensor suhu/kelembaban DHT11
// 4. Adafruit Unified Sensor (dependency dari DHT Sensor Library)

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>

// ==========================================
// ⚙️ KONFIGURASI WIFI & TELEGRAM (Ubah di sini)
// ==========================================
const char* ssid = "YOUR_SSID";          // Nama WiFi Anda
const char* password = "YOUR_PASSWORD";  // Password WiFi Anda

// Token Telegram Bot (Dapatkan dari @BotFather)
#define BOT_TOKEN "1234567890:YOUR_BOT_TOKEN_HERE"

// Chat ID Telegram Anda (Dapatkan dari @userinfobot atau @IDBot)
// Ini berfungsi sebagai pengaman agar hanya akun Anda yang bisa mengontrol modul
#define CHAT_ID "YOUR_CHAT_ID_HERE"

// ==========================================
// 📌 PINOUT HARDWARE MODUL RELAY IOT ESP8266
// ==========================================
#define RELAY1 4      // Relay 1 terhubung ke GPIO 4 (D2)
#define RELAY2 5      // Relay 2 terhubung ke GPIO 5 (D1)
#define DHTPIN 12     // Sensor DHT11 terhubung ke GPIO 12 (D6)
#define DHTTYPE DHT11 // Jenis sensor: DHT11

// ==========================================
// 🛸 INISIALISASI OBJECT
// ==========================================
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
DHT dht(DHTPIN, DHTTYPE);

// Pengatur interval pembacaan/checking bot Telegram
const unsigned long BOT_DELAY_MS = 1000; // Cek pesan baru setiap 1 detik
unsigned long lastTimeBotRan = 0;

// Fungsi untuk memproses pesan Telegram yang masuk
void handleNewMessages(int numNewMessages) {
  Serial.print("Pesan baru masuk: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    
    // 🛡️ VALIDASI KEAMANAN:
    // Mencegah orang lain mengirim perintah ke bot Anda jika Chat ID tidak cocok
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "⚠️ Akses Ditolak! Anda tidak memiliki izin untuk mengontrol perangkat ini.", "");
      continue;
    }

    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    
    Serial.println("Pesan dari " + from_name + ": " + text);

    if (text == "/start") {
      String welcome = "👋 Halo " + from_name + "!\n\n";
      welcome += "Saya adalah Bot Telegram untuk Modul IoT Relay ESP8266.\n";
      welcome += "Menggunakan library Universal Telegram Bot.\n\n";
      welcome += "Berikut adalah daftar perintah yang tersedia:\n";
      welcome += "🟢 /relay1_on  : Menyalakan Relay 1\n";
      welcome += "🔴 /relay1_off : Mematikan Relay 1\n";
      welcome += "🟢 /relay2_on  : Menyalakan Relay 2\n";
      welcome += "🔴 /relay2_off : Mematikan Relay 2\n";
      welcome += "📊 /status     : Cek Suhu, Kelembaban, & Status Relay\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    
    else if (text == "/relay1_on") {
      digitalWrite(RELAY1, HIGH);
      bot.sendMessage(chat_id, "💡 Relay 1 berhasil dinyalakan (ON)!", "");
    }
    
    else if (text == "/relay1_off") {
      digitalWrite(RELAY1, LOW);
      bot.sendMessage(chat_id, "🔌 Relay 1 berhasil dimatikan (OFF)!", "");
    }
    
    else if (text == "/relay2_on") {
      digitalWrite(RELAY2, HIGH);
      bot.sendMessage(chat_id, "💡 Relay 2 berhasil dinyalakan (ON)!", "");
    }
    
    else if (text == "/relay2_off") {
      digitalWrite(RELAY2, LOW);
      bot.sendMessage(chat_id, "🔌 Relay 2 berhasil dimatikan (OFF)!", "");
    }
    
    else if (text == "/status") {
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      
      String r1State = (digitalRead(RELAY1) == HIGH) ? "ON 🟢" : "OFF 🔴";
      String r2State = (digitalRead(RELAY2) == HIGH) ? "ON 🟢" : "OFF 🔴";
      
      String statusMsg = "📊 *STATUS MODUL RELAY*\n\n";
      
      if (isnan(t) || isnan(h)) {
        statusMsg += "⚠️ Sensor DHT11: Gagal membaca data!\n\n";
      } else {
        statusMsg += "🌡️ Suhu: " + String(t, 1) + " °C\n";
        statusMsg += "💧 Kelembaban: " + String(h, 0) + " %\n\n";
      }
      
      statusMsg += "💡 Relay 1: " + r1State + "\n";
      statusMsg += "💡 Relay 2: " + r2State + "\n";
      
      bot.sendMessage(chat_id, statusMsg, "Markdown");
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Inisialisasi Pin Relay
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  
  // Matikan relay secara default saat pertama kali menyala
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // Inisialisasi Sensor DHT11
  dht.begin();

  // 🔒 Mengabaikan validasi SSL Certificate Telegram Server
  // agar program terus berjalan tanpa terpengaruh masa kadaluarsa sertifikat HTTPS
  client.setInsecure();

  // Koneksi ke Jaringan WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Kirim notifikasi pertama kali saat modul boot up ke Chat ID yang diset
  if (String(CHAT_ID) != "YOUR_CHAT_ID_HERE" && String(CHAT_ID) != "") {
    bot.sendMessage(CHAT_ID, "🤖 Modul IoT Relay ESP8266 aktif dan terhubung ke WiFi!", "");
  }
}

void loop() {
  // Cek pesan Telegram secara non-blocking setiap interval waktu yang ditentukan
  if (millis() - lastTimeBotRan > BOT_DELAY_MS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
