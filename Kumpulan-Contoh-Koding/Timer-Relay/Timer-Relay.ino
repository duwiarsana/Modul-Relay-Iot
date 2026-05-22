// Timer Relay ESP8266 Web Interface
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
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
int gmtOffset = 8; // Default GMT+8 (Bali / WITA)

// Fungsi untuk menyimpan pengaturan ke EEPROM
void saveSettings() {
  Serial.println("Saving settings...");
  EEPROM.put(0, relay1Setting);
  EEPROM.put(sizeof(TimerSetting), relay2Setting);
  EEPROM.put(2 * sizeof(TimerSetting), gmtOffset);
  EEPROM.commit();
  Serial.println("Settings saved:");
  Serial.print("Relay1 ON: "); Serial.print(relay1Setting.hourOn); Serial.print(":"); Serial.println(relay1Setting.minuteOn);
  Serial.print("Relay1 OFF: "); Serial.print(relay1Setting.hourOff); Serial.print(":"); Serial.println(relay1Setting.minuteOff);
  Serial.print("Relay2 ON: "); Serial.print(relay2Setting.hourOn); Serial.print(":"); Serial.println(relay2Setting.minuteOn);
  Serial.print("Relay2 OFF: "); Serial.print(relay2Setting.hourOff); Serial.print(":"); Serial.println(relay2Setting.minuteOff);
  Serial.print("GMT Offset: "); Serial.println(gmtOffset);
}

// Fungsi untuk memuat pengaturan dari EEPROM
void loadSettings() {
  Serial.println("Loading settings...");
  EEPROM.get(0, relay1Setting);
  EEPROM.get(sizeof(TimerSetting), relay2Setting);
  EEPROM.get(2 * sizeof(TimerSetting), gmtOffset);
  
  // Validasi nilai agar tidak berantakan jika EEPROM masih kosong (default 255)
  if (relay1Setting.hourOn < 0 || relay1Setting.hourOn > 23 || relay1Setting.minuteOn < 0 || relay1Setting.minuteOn > 59) {
    relay1Setting = {0, 0, 0, 0};
  }
  if (relay2Setting.hourOn < 0 || relay2Setting.hourOn > 23 || relay2Setting.minuteOn < 0 || relay2Setting.minuteOn > 59) {
    relay2Setting = {0, 0, 0, 0};
  }
  if (gmtOffset < -12 || gmtOffset > 14) {
    gmtOffset = 8; // Default GMT+8
  }

  // Update NTP client offset
  timeClient.setTimeOffset(gmtOffset * 3600);

  Serial.println("Settings loaded:");
  Serial.print("Relay1 ON: "); Serial.print(relay1Setting.hourOn); Serial.print(":"); Serial.println(relay1Setting.minuteOn);
  Serial.print("Relay1 OFF: "); Serial.print(relay1Setting.hourOff); Serial.print(":"); Serial.println(relay1Setting.minuteOff);
  Serial.print("Relay2 ON: "); Serial.print(relay2Setting.hourOn); Serial.print(":"); Serial.println(relay2Setting.minuteOn);
  Serial.print("Relay2 OFF: "); Serial.print(relay2Setting.hourOff); Serial.print(":"); Serial.println(relay2Setting.minuteOff);
  Serial.print("GMT Offset: "); Serial.println(gmtOffset);
}

// Fungsi helper untuk memformat waktu menjadi format dua digit (HH:MM) untuk input HTML time
String formatTimeVal(int hour, int minute) {
  char buf[6];
  sprintf(buf, "%02d:%02d", hour, minute);
  return String(buf);
}

// Halaman HTML yang disimpan di flash memory (PROGMEM) untuk hemat RAM (mencegah stack overflow)
const char html_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>IoT Relay Scheduler</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <style>
    :root {
      --bg-color: #0b0f19;
      --card-bg: rgba(22, 28, 45, 0.6);
      --card-border: rgba(255, 255, 255, 0.08);
      --text-main: #f3f4f6;
      --text-sub: #9ca3af;
      --primary: #8b5cf6;
      --secondary: #d946ef;
      --accent: #00f2fe;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Plus Jakarta Sans', sans-serif; }
    body {
      background-color: var(--bg-color);
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      padding: 20px;
      position: relative;
      overflow-x: hidden;
    }
    body::before {
      content: ''; position: absolute; width: 280px; height: 280px;
      background: var(--primary); border-radius: 50%; filter: blur(120px);
      z-index: -1; opacity: 0.3; top: 15%; left: 15%;
    }
    body::after {
      content: ''; position: absolute; width: 280px; height: 280px;
      background: var(--secondary); border-radius: 50%; filter: blur(120px);
      z-index: -1; opacity: 0.25; bottom: 15%; right: 15%;
    }
    .container { width: 100%; max-width: 540px; z-index: 1; }
    header { text-align: center; margin-bottom: 25px; }
    .logo-icon {
      background: linear-gradient(135deg, var(--primary), var(--secondary));
      width: 48px; height: 48px; border-radius: 12px;
      display: inline-flex; align-items: center; justify-content: center;
      margin-bottom: 10px; box-shadow: 0 4px 15px rgba(139, 92, 246, 0.3);
    }
    .logo-icon i { color: #0b0f19; font-size: 22px; }
    h1 { font-size: 20px; font-weight: 700; background: linear-gradient(to right, #fff, #f472b6); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }
    p.subtitle { font-size: 11px; color: var(--text-sub); margin-top: 3px; }
    
    /* Clock Widget */
    .clock-card {
      background: rgba(255, 255, 255, 0.03);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(10px);
      border-radius: 20px;
      padding: 20px;
      text-align: center;
      margin-bottom: 20px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
    }
    .clock-title { font-size: 12px; color: var(--text-sub); font-weight: 600; text-transform: uppercase; letter-spacing: 1px; display: flex; align-items: center; justify-content: center; gap: 8px; margin-bottom: 5px; }
    .clock-title i { color: var(--accent); }
    #timeDisplay { font-size: 40px; font-weight: 800; background: linear-gradient(to right, #fff, var(--accent)); -webkit-background-clip: text; -webkit-text-fill-color: transparent; font-variant-numeric: tabular-nums; }
    
    /* Main Schedule Card */
    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 24px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
      margin-bottom: 20px;
      position: relative;
    }
    .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; margin-bottom: 25px; }
    @media (max-width: 480px) { .grid { grid-template-columns: 1fr; gap: 15px; } }
    
    .section-title { font-size: 14px; font-weight: 700; color: var(--text-main); border-bottom: 1px solid rgba(255,255,255,0.06); padding-bottom: 8px; margin-bottom: 12px; display: flex; align-items: center; gap: 8px; }
    .section-title i { color: var(--primary); }
    
    .input-group { margin-bottom: 12px; display: flex; flex-direction: column; gap: 6px; }
    .input-group label { font-size: 12px; color: var(--text-sub); font-weight: 500; }
    .input-wrapper { position: relative; display: flex; align-items: center; }
    .input-wrapper i { position: absolute; left: 12px; color: var(--text-sub); font-size: 14px; z-index: 2; }
    
    input[type="time"], select {
      width: 100%;
      background-color: rgba(0, 0, 0, 0.2);
      border: 1px solid rgba(255, 255, 255, 0.08);
      color: #fff;
      padding: 10px 10px 10px 38px;
      font-size: 14px;
      font-weight: 600;
      border-radius: 10px;
      outline: none;
      transition: all 0.3s ease;
      color-scheme: dark;
    }
    input[type="time"]:focus, select:focus {
      border-color: var(--primary);
      box-shadow: 0 0 8px rgba(139, 92, 246, 0.3);
    }
    
    .btn-submit {
      width: 100%;
      background: linear-gradient(135deg, var(--primary), var(--secondary));
      border: none;
      color: #fff;
      padding: 14px;
      border-radius: 12px;
      font-size: 15px;
      font-weight: 700;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 10px;
      transition: all 0.3s ease;
      box-shadow: 0 4px 15px rgba(139, 92, 246, 0.3);
    }
    .btn-submit:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(139, 92, 246, 0.5);
    }
    .btn-submit:active { transform: translateY(0); }
    
    #toast {
      position: fixed;
      bottom: 20px;
      left: 50%;
      transform: translateX(-50%) translateY(100px);
      background: rgba(16, 185, 129, 0.95);
      border: 1px solid rgba(16, 185, 129, 0.3);
      padding: 12px 24px;
      border-radius: 30px;
      color: #fff;
      font-size: 13px;
      font-weight: 600;
      display: flex;
      align-items: center;
      gap: 8px;
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.3);
      transition: transform 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
      z-index: 999;
    }
    #toast.show { transform: translateX(-50%) translateY(0); }
    
    footer { text-align: center; font-size: 11px; color: var(--text-sub); }
    footer a { color: var(--accent); text-decoration: none; font-weight: 500; }
  </style>
</head>
<body>
<div class="container">
  <header>
    <div class="logo-icon"><i class="fa-solid fa-clock"></i></div>
    <h1>WiFi Relay Scheduler</h1>
    <p class="subtitle">ESP8266 Time-Based Control</p>
  </header>
  
  <div class="clock-card">
    <div class="clock-title"><i class="fa-solid fa-clock-rotate-left"></i> Waktu Saat Ini</div>
    <div id="timeDisplay">--:--:--</div>
  </div>
  
  <div class="card">
    <form id="schedulerForm">
      <!-- Timezone Selector -->
      <div style="margin-bottom: 20px; border-bottom: 1px solid rgba(255,255,255,0.06); padding-bottom: 15px; display: flex; flex-direction: column; gap: 6px;">
        <div class="section-title" style="border: none; padding-bottom: 0; margin-bottom: 0;"><i class="fa-solid fa-earth-asia"></i> Zona Waktu (GMT)</div>
        <div class="input-wrapper">
          <i class="fa-solid fa-globe"></i>
          <select id="gmtSelect" name="gmt">
            <!-- Dynamically populated -->
          </select>
        </div>
      </div>

      <div class="grid">
        <!-- Relay 1 column -->
        <div>
          <div class="section-title"><i class="fa-solid fa-lightbulb"></i> Relay 1 Settings</div>
          <div class="input-group">
            <label>Relay 1 ON Time</label>
            <div class="input-wrapper">
              <i class="fa-solid fa-play"></i>
              <input type="time" name="ron1">
            </div>
          </div>
          <div class="input-group">
            <label>Relay 1 OFF Time</label>
            <div class="input-wrapper">
              <i class="fa-solid fa-stop"></i>
              <input type="time" name="roff1">
            </div>
          </div>
        </div>
        
        <!-- Relay 2 column -->
        <div>
          <div class="section-title"><i class="fa-solid fa-power-off"></i> Relay 2 Settings</div>
          <div class="input-group">
            <label>Relay 2 ON Time</label>
            <div class="input-wrapper">
              <i class="fa-solid fa-play"></i>
              <input type="time" name="ron2">
            </div>
          </div>
          <div class="input-group">
            <label>Relay 2 OFF Time</label>
            <div class="input-wrapper">
              <i class="fa-solid fa-stop"></i>
              <input type="time" name="roff2">
            </div>
          </div>
        </div>
      </div>
      
      <button class="btn-submit" type="submit">
        <i class="fa-solid fa-floppy-disk"></i> Simpan Jadwal
      </button>
    </form>
  </div>
  
  <footer>
    Dibuat oleh <a href="https://duwiarsana.com" target="_blank">Duwi Arsana</a>
  </footer>
</div>

<div id="toast"><i class="fa-solid fa-circle-check"></i> Pengaturan berhasil disimpan!</div>

<script>
  // Populate GMT options
  const select = document.getElementById("gmtSelect");
  for (let i = -12; i <= 14; i++) {
    const opt = document.createElement("option");
    opt.value = i;
    opt.textContent = `GMT${i >= 0 ? '+' : ''}${i}`;
    select.appendChild(opt);
  }

  function updateTime() {
    fetch('/time')
      .then(res => res.text())
      .then(data => {
        document.getElementById('timeDisplay').innerText = data;
      })
      .catch(err => console.error(err));
  }
  setInterval(updateTime, 1000);
  updateTime();

  // Load settings
  fetch('/settings')
    .then(res => res.json())
    .then(data => {
      select.value = data.gmt;
      document.querySelector('input[name="ron1"]').value = data.ron1;
      document.querySelector('input[name="roff1"]').value = data.roff1;
      document.querySelector('input[name="ron2"]').value = data.ron2;
      document.querySelector('input[name="roff2"]').value = data.roff2;
    })
    .catch(err => console.error("Error loading settings:", err));

  // Submit form
  document.getElementById("schedulerForm").addEventListener("submit", function(e) {
    e.preventDefault();
    const formData = new FormData(this);
    const params = new URLSearchParams(formData).toString();
    fetch('/set?' + params)
      .then(res => {
        if(res.ok) {
          const toast = document.getElementById("toast");
          toast.className = "show";
          setTimeout(() => { toast.className = ""; }, 3000);
        } else {
          alert("Gagal menyimpan jadwal!");
        }
      })
      .catch(err => {
        console.error(err);
        alert("Gagal menghubungi ESP8266!");
      });
  });
</script>
</body>
</html>
)rawliteral";

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
    request->send_P(200, "text/html", html_template);
  });

  // Endpoint JSON untuk mengambil pengaturan saat ini
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"gmt\":" + String(gmtOffset) + ",";
    json += "\"ron1\":\"" + formatTimeVal(relay1Setting.hourOn, relay1Setting.minuteOn) + "\",";
    json += "\"roff1\":\"" + formatTimeVal(relay1Setting.hourOff, relay1Setting.minuteOff) + "\",";
    json += "\"ron2\":\"" + formatTimeVal(relay2Setting.hourOn, relay2Setting.minuteOn) + "\",";
    json += "\"roff2\":\"" + formatTimeVal(relay2Setting.hourOff, relay2Setting.minuteOff) + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });

  // Endpoint untuk menyimpan pengaturan waktu dari form
  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("gmt")) {
      gmtOffset = request->getParam("gmt")->value().toInt();
      timeClient.setTimeOffset(gmtOffset * 3600);
      timeClient.update();
    }
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
    request->send(200, "text/plain", "OK");
  });

  // Endpoint untuk menampilkan waktu saat ini ke halaman web
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    char buf[9];
    sprintf(buf, "%02d:%02d:%02d", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
    request->send(200, "text/plain", String(buf));
  });

  server.begin();

  // Inisialisasi mDNS
  if (MDNS.begin("timer-relay")) {
    Serial.println("mDNS responder started! Akses di: http://timer-relay.local");
  }
}


void loop() {
  MDNS.update();

  static unsigned long lastTimerUpdate = 0;
  if (millis() - lastTimerUpdate >= 1000) {
    lastTimerUpdate = millis();

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
  }
}
