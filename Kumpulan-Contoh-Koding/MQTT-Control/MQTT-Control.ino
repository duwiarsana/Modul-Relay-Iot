// Copyright (c) 2026 Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
//
// Contoh project kontrol 2-Channel Relay dan monitoring DHT11 via MQTT (JSON Format)
// Broker MQTT yang digunakan secara default adalah broker gratis: broker.hivemq.com

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Definisi PIN sesuai dengan layout hardware modul relay
#define DHTPIN 12        // GPIO 12 (D6)
#define DHTTYPE DHT11    // Menggunakan sensor DHT11 (Ubah ke DHT22 jika memakai sensor DHT22)
#define RELAY1_PIN 4     // GPIO 4 (D2) - Relay 1 (Aktif HIGH)
#define RELAY2_PIN 5     // GPIO 5 (D1) - Relay 2 (Aktif HIGH)

// Konfigurasi WiFi
const char* ssid = "ASUS_6C";
const char* password = "b&5d6ebc72";

// Konfigurasi MQTT Broker (HiveMQ gratisan)
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Topic MQTT akan dibuat unik menggunakan Chip ID ESP8266 agar tidak bertabrakan dengan user lain
String base_topic;
String topic_data;        // Topic tunggal untuk output JSON (Suhu, Kelembaban, Status Relay)
String topic_relay1_cmd;  // Topic untuk menerima perintah Relay 1
String topic_relay2_cmd;  // Topic untuk menerima perintah Relay 2

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastMsg = 0;
float temp = 0;
float hum = 0;

// Status Relay (false = OFF, true = ON)
bool relay1_state = false;
bool relay2_state = false;

// Fungsi untuk mengirimkan payload data berformat JSON
void publishData() {
  String json = "{";
  json += "\"suhu\":" + (isnan(temp) ? "null" : String(temp, 2)) + ",";
  json += "\"kelembaban\":" + (isnan(hum) ? "null" : String(hum, 2)) + ",";
  json += "\"relay1\":" + String(relay1_state ? 1 : 0) + ",";
  json += "\"relay2\":" + String(relay2_state ? 1 : 0);
  json += "}";
  
  client.publish(topic_data.c_str(), json.c_str(), true);
  Serial.print("Data terkirim ke MQTT: ");
  Serial.println(json);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Callback fungsi saat ada pesan masuk dari MQTT Broker
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan masuk pada topic: [");
  Serial.print(topic);
  Serial.print("] ");
  
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  Serial.println(messageTemp);

  bool stateChanged = false;

  // Kontrol Relay 1
  if (String(topic) == topic_relay1_cmd) {
    if (messageTemp == "1" || messageTemp == "ON") {
      digitalWrite(RELAY1_PIN, HIGH);
      relay1_state = true;
      Serial.println("Relay 1 -> ON");
      stateChanged = true;
    } else if (messageTemp == "0" || messageTemp == "OFF") {
      digitalWrite(RELAY1_PIN, LOW);
      relay1_state = false;
      Serial.println("Relay 1 -> OFF");
      stateChanged = true;
    }
  }
  
  // Kontrol Relay 2
  else if (String(topic) == topic_relay2_cmd) {
    if (messageTemp == "1" || messageTemp == "ON") {
      digitalWrite(RELAY2_PIN, HIGH);
      relay2_state = true;
      Serial.println("Relay 2 -> ON");
      stateChanged = true;
    } else if (messageTemp == "0" || messageTemp == "OFF") {
      digitalWrite(RELAY2_PIN, LOW);
      relay2_state = false;
      Serial.println("Relay 2 -> OFF");
      stateChanged = true;
    }
  }

  // Jika ada perubahan status relay, langsung kirim update data JSON terbaru
  if (stateChanged) {
    publishData();
  }
}

void reconnect() {
  // Loop sampai terhubung kembali
  while (!client.connected()) {
    Serial.print("Mencoba koneksi MQTT...");
    // Membuat Client ID unik dari Chip ID
    String clientId = "ESP8266Client-" + String(ESP.getChipId());
    
    // Mencoba terhubung ke Broker
    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung ke MQTT Broker!");
      
      // Subscribe ke topic command relay setelah berhasil konek
      client.subscribe(topic_relay1_cmd.c_str());
      client.subscribe(topic_relay2_cmd.c_str());
      Serial.println("Subscribed to command topics.");
      
      // Kirim status awal data
      publishData();
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Setup Pin Mode untuk Relay
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  
  // Default awal relay MATI (LOW)
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);

  // Inisialisasi Sensor DHT
  dht.begin();

  // Inisialisasi Topic MQTT unik berdasarkan Chip ID ESP
  String chipId = String(ESP.getChipId());
  base_topic = "modul-relay-iot/" + chipId;
  
  topic_data = base_topic + "/data";
  topic_relay1_cmd = base_topic + "/relay1/cmd";
  topic_relay2_cmd = base_topic + "/relay2/cmd";

  Serial.println("\n--- MQTT Relay & DHT11 Module (JSON) ---");
  Serial.print("Unique MQTT Client ID: ESP8266Client-"); Serial.println(chipId);
  Serial.println("Daftar topic yang digunakan:");
  Serial.print("  - Output Data (JSON): "); Serial.println(topic_data);
  Serial.print("  - Kontrol Relay 1 (Kirim ON/OFF atau 1/0): "); Serial.println(topic_relay1_cmd);
  Serial.print("  - Kontrol Relay 2 (Kirim ON/OFF atau 1/0): "); Serial.println(topic_relay2_cmd);
  Serial.println("----------------------------------------");

  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  // Membaca sensor & mengirim data setiap 10 detik
  if (now - lastMsg > 10000) {
    lastMsg = now;

    // Membaca kelembaban dan suhu
    float newHum = dht.readHumidity();
    float newTemp = dht.readTemperature();

    // Cek apakah pembacaan sensor gagal
    if (isnan(newHum) || isnan(newTemp)) {
      Serial.println("Gagal membaca dari sensor DHT!");
      Serial.println("Tip: Pastikan kabel terhubung dengan benar, atau ganti '#define DHTTYPE DHT11' ke 'DHT22' di kode jika menggunakan sensor DHT22.");
    } else {
      temp = newTemp;
      hum = newHum;
      
      Serial.print("Suhu: ");
      Serial.print(temp);
      Serial.print(" *C, Kelembaban: ");
      Serial.print(hum);
      Serial.println(" %");
    }

    // Tetap kirim JSON data status terbaru ke broker walaupun sensor error (dengan value null jika error)
    publishData();
  }
}
