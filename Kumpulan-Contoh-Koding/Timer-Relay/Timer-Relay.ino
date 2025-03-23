// Timer Relay ESP8266 Web Interface
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

#define RELAY1 4
#define RELAY2 5

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800, 60000); // GMT+7
AsyncWebServer server(80);

// Struktur untuk menyimpan waktu ON dan OFF relay
struct TimerSetting {
  int hourOn;
  int minuteOn;
  int hourOff;
  int minuteOff;
};

TimerSetting relay1Setting;
TimerSetting relay2Setting;

// Fungsi untuk menyimpan pengaturan ke EEPROM
void saveSettings() {
  Serial.println("Saving settings...");
  EEPROM.put(0, relay1Setting);
  EEPROM.put(sizeof(TimerSetting), relay2Setting);
  EEPROM.commit();
  Serial.println("Settings saved:");
  Serial.print("Relay1 ON: "); Serial.print(relay1Setting.hourOn); Serial.print(":"); Serial.println(relay1Setting.minuteOn);
  Serial.print("Relay1 OFF: "); Serial.print(relay1Setting.hourOff); Serial.print(":"); Serial.println(relay1Setting.minuteOff);
  Serial.print("Relay2 ON: "); Serial.print(relay2Setting.hourOn); Serial.print(":"); Serial.println(relay2Setting.minuteOn);
  Serial.print("Relay2 OFF: "); Serial.print(relay2Setting.hourOff); Serial.print(":"); Serial.println(relay2Setting.minuteOff);
}

// Fungsi untuk memuat pengaturan dari EEPROM
void loadSettings() {
  Serial.println("Loading settings...");
  EEPROM.get(0, relay1Setting);
  EEPROM.get(sizeof(TimerSetting), relay2Setting);
  Serial.println("Settings loaded:");
  Serial.print("Relay1 ON: "); Serial.print(relay1Setting.hourOn); Serial.print(":"); Serial.println(relay1Setting.minuteOn);
  Serial.print("Relay1 OFF: "); Serial.print(relay1Setting.hourOff); Serial.print(":"); Serial.println(relay1Setting.minuteOff);
  Serial.print("Relay2 ON: "); Serial.print(relay2Setting.hourOn); Serial.print(":"); Serial.println(relay2Setting.minuteOn);
  Serial.print("Relay2 OFF: "); Serial.print(relay2Setting.hourOff); Serial.print(":"); Serial.println(relay2Setting.minuteOff);
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // Tampilkan alamat IP dari ESP8266
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Mulai NTP client
  timeClient.begin();
  timeClient.update();

  // Tampilkan waktu saat ini
  Serial.print("Current Time: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.println(timeClient.getMinutes());

  loadSettings();

  // Endpoint utama (halaman web)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // HTML tampilan dan form pengaturan waktu ON/OFF relay
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/milligram/1.4.1/milligram.min.css'>";
    html += "<style>body{background-color:#121212; color:white; text-align:center; font-family:sans-serif;}";
    html += "h2{color: #fff;} button{background-color:#333; color:white; border:none; padding:10px; border-radius:5px;}";
    html += "input{background-color:#222; color:white; border:1px solid #555; padding:5px; margin:5px; border-radius:5px;}";
    html += "#timeDisplay{font-size:48px; font-weight:bold; margin:10px;}</style></head><body>";
    html += "<h2>Relay Timer Settings</h2>";
    html += "<p><span id='timeDisplay'></span></p>";
    html += "<script>function updateTime() { fetch('/time').then(response => response.text()).then(data => document.getElementById('timeDisplay').innerText = data); } setInterval(updateTime, 1000); updateTime();</script>";
    html += "<form action='/set' method='GET' style='display: flex; flex-direction: column; align-items: center; max-width: 300px; margin: auto;'>";
    html += "<div style='display: flex; justify-content: space-between; width: 100%;'><label>Relay 1 ON:</label> <input type='time' name='ron1' value='" + String(relay1Setting.hourOn) + ":" + String(relay1Setting.minuteOn) + "'></div>";
    html += "<div style='display: flex; justify-content: space-between; width: 100%;'><label>Relay 1 OFF:</label> <input type='time' name='roff1' value='" + String(relay1Setting.hourOff) + ":" + String(relay1Setting.minuteOff) + "'></div>";
    html += "<div style='display: flex; justify-content: space-between; width: 100%;'><label>Relay 2 ON:</label> <input type='time' name='ron2' value='" + String(relay2Setting.hourOn) + ":" + String(relay2Setting.minuteOn) + "'></div>";
    html += "<div style='display: flex; justify-content: space-between; width: 100%;'><label>Relay 2 OFF:</label> <input type='time' name='roff2' value='" + String(relay2Setting.hourOff) + ":" + String(relay2Setting.minuteOff) + "'></div>";
    html += "<div style='width: 100%; display: flex; justify-content: center; margin-top: 10px;'><button class='button-primary' type='submit' style='width: 220px; height: 50px; padding: 0; font-size: 18px; background-color: #4CAF50; color: white; border: none; border-radius: 8px; cursor: pointer; transition: 0.3s; display: flex; align-items: center; justify-content: center; font-weight: bold;'>Simpan</button></div></form></body></html>";
    request->send(200, "text/html", html);
  });

  // Endpoint untuk menyimpan pengaturan waktu dari form
  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("ron1")) {
      String timeStr = request->getParam("ron1")->value();
      relay1Setting.hourOn = timeStr.substring(0, 2).toInt();
      relay1Setting.minuteOn = timeStr.substring(3, 5).toInt();
    }
    if(request->hasParam("roff1")) {
      String timeStr = request->getParam("roff1")->value();
      relay1Setting.hourOff = timeStr.substring(0, 2).toInt();
      relay1Setting.minuteOff = timeStr.substring(3, 5).toInt();
    }
    if(request->hasParam("ron2")) {
      String timeStr = request->getParam("ron2")->value();
      relay2Setting.hourOn = timeStr.substring(0, 2).toInt();
      relay2Setting.minuteOn = timeStr.substring(3, 5).toInt();
    }
    if(request->hasParam("roff2")) {
      String timeStr = request->getParam("roff2")->value();
      relay2Setting.hourOff = timeStr.substring(0, 2).toInt();
      relay2Setting.minuteOff = timeStr.substring(3, 5).toInt();
    }
    saveSettings();
    request->redirect("/");
  });

  // Endpoint untuk menampilkan waktu saat ini ke halaman web
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    String currentTime = String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds());
    request->send(200, "text/plain", currentTime);
  });

  server.begin();
}

void loop() {
  // Update waktu dari NTP server
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  // Log waktu saat ini ke Serial Monitor
  Serial.print("Current Time: ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.println(currentMinute);

  // Kontrol Relay 1
  if (currentHour == relay1Setting.hourOn && currentMinute == relay1Setting.minuteOn) {
    digitalWrite(RELAY1, HIGH);
  } else if (currentHour == relay1Setting.hourOff && currentMinute == relay1Setting.minuteOff) {
    digitalWrite(RELAY1, LOW);
  }

  // Kontrol Relay 2
  if (currentHour == relay2Setting.hourOn && currentMinute == relay2Setting.minuteOn) {
    digitalWrite(RELAY2, HIGH);
  } else if (currentHour == relay2Setting.hourOff && currentMinute == relay2Setting.minuteOff) {
    digitalWrite(RELAY2, LOW);
  }

  delay(1000); // Periksa setiap detik
}
