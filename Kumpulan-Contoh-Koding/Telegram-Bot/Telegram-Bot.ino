// Telegram Bot Controller & Temperature Alert ESP8266
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <EEPROM.h>

// WiFi credentials (ubah sesuai dengan WiFi Anda)
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

#define RELAY1 4
#define RELAY2 5
#define DHTPIN 12
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
AsyncWebServer server(80);

struct BotConfig {
  char token[80];    // Token Telegram Bot
  char chatId[24];   // Chat ID Telegram User/Group
  float tempAlert;   // Batas alarm suhu
  bool alertEnabled; // Aktif/nonaktif alarm
};

BotConfig config;
float currentTemp = 0.0;
float currentHumidity = 0.0;
unsigned long lastDHTRead = 0;
unsigned long lastBotCheck = 0;
long lastUpdateId = 0;
bool alertSent = false;

WiFiClientSecure client;

void saveConfig() {
  Serial.println("Saving config...");
  EEPROM.put(0, config);
  EEPROM.commit();
}

void loadConfig() {
  Serial.println("Loading config...");
  EEPROM.get(0, config);
  
  // Validasi nilai kosong dari EEPROM fresh
  if (config.token[0] == 255 || strlen(config.token) == 0) {
    strcpy(config.token, "");
  }
  if (config.chatId[0] == 255 || strlen(config.chatId) == 0) {
    strcpy(config.chatId, "");
  }
  if (isnan(config.tempAlert) || config.tempAlert < 10.0f || config.tempAlert > 80.0f) {
    config.tempAlert = 35.0f;
  }
  if (config.alertEnabled != true && config.alertEnabled != false) {
    config.alertEnabled = false;
  }

  Serial.print("Bot Token: "); Serial.println(config.token);
  Serial.print("Chat ID: "); Serial.println(config.chatId);
  Serial.print("Alarm Temp: "); Serial.println(config.tempAlert);
}

// Mengirim pesan Telegram secara asinkron menggunakan client secure
void sendTelegramMessage(String chatId, String message) {
  if (strlen(config.token) == 0 || chatId.length() == 0) return;
  
  WiFiClientSecure secureClient;
  secureClient.setInsecure(); // Mengabaikan validasi fingerprint SSL agar tahan lama/tidak kadaluarsa
  
  if (secureClient.connect("api.telegram.org", 443)) {
    // URL Encode manual karakter krusial
    message.replace(" ", "%20");
    message.replace("\n", "%0A");
    message.replace(":", "%3A");
    message.replace(",", "%2C");
    message.replace("!", "%21");
    
    String url = "/bot" + String(config.token) + "/sendMessage?chat_id=" + chatId + "&text=" + message;
    
    secureClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                       "Host: api.telegram.org\r\n" +
                       "Connection: close\r\n\r\n");
    
    // Tunggu data masuk untuk dikosongkan agar koneksi tertutup rapi
    while (secureClient.connected()) {
      secureClient.read();
    }
  }
}

// Handler Perintah Chat Bot Telegram
void handleBotCommand(String cmd, String chatId) {
  Serial.print("Telegram CMD: ");
  Serial.println(cmd);
  cmd.trim();
  
  if (cmd == "/start") {
    String reply = "👋 Halo! Saya adalah ESP8266 IoT Relay Bot.\n\n";
    reply += "Perintah yang tersedia:\n";
    reply += "📱 /status - Cek kondisi relay & sensor suhu\n";
    reply += "🟢 /relay1_on - Nyalakan Relay 1\n";
    reply += "🔴 /relay1_off - Matikan Relay 1\n";
    reply += "🟢 /relay2_on - Nyalakan Relay 2\n";
    reply += "🔴 /relay2_off - Matikan Relay 2\n\n";
    reply += "Pastikan Chat ID (" + chatId + ") sudah disetel di Web Panel modul Anda agar notifikasi alarm suhu otomatis dapat dikirim.";
    sendTelegramMessage(chatId, reply);
  } 
  else if (cmd == "/status") {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    String r1State = (digitalRead(RELAY1) == HIGH) ? "ON 🟢" : "OFF 🔴";
    String r2State = (digitalRead(RELAY2) == HIGH) ? "ON 🟢" : "OFF 🔴";
    
    String reply = "📊 *STATUS MODUL RELAY*\n\n";
    if (!isnan(t)) {
      reply += "🌡️ Suhu: " + String(t, 1) + " °C\n";
      reply += "💧 Kelembaban: " + String(h, 0) + " %\n\n";
    } else {
      reply += "🌡️ Sensor DHT11: Error/Tidak Terbaca\n\n";
    }
    reply += "💡 Relay 1: " + r1State + "\n";
    reply += "💡 Relay 2: " + r2State + "\n\n";
    reply += "⚙️ Alert Temp: " + String(config.tempAlert, 1) + " °C (" + (config.alertEnabled ? "Aktif" : "Nonaktif") + ")";
    
    sendTelegramMessage(chatId, reply);
  }
  else if (cmd == "/relay1_on") {
    digitalWrite(RELAY1, HIGH);
    sendTelegramMessage(chatId, "💡 Relay 1 Berhasil Dinyalakan (ON)!");
  }
  else if (cmd == "/relay1_off") {
    digitalWrite(RELAY1, LOW);
    sendTelegramMessage(chatId, "🔌 Relay 1 Berhasil Dimatikan (OFF)!");
  }
  else if (cmd == "/relay2_on") {
    digitalWrite(RELAY2, HIGH);
    sendTelegramMessage(chatId, "💡 Relay 2 Berhasil Dinyalakan (ON)!");
  }
  else if (cmd == "/relay2_off") {
    digitalWrite(RELAY2, LOW);
    sendTelegramMessage(chatId, "🔌 Relay 2 Berhasil Dimatikan (OFF)!");
  }
}

// Memeriksa chat masuk dari Telegram Server (Long Polling ringan)
void checkTelegram() {
  if (strlen(config.token) == 0) return;
  
  client.setInsecure();
  if (!client.connect("api.telegram.org", 443)) {
    return;
  }

  String url = "/bot" + String(config.token) + "/getUpdates?limit=1";
  if (lastUpdateId > 0) {
    url += "&offset=" + String(lastUpdateId + 1);
  }
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: api.telegram.org\r\n" +
               "Connection: close\r\n\r\n");

  // Lewati headers HTTP
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  // Baca konten JSON response
  String body = client.readString();
  
  // Parsing text, update_id, dan chat_id secara hemat RAM tanpa library JSON eksternal
  int updateIdx = body.indexOf("\"update_id\":");
  if (updateIdx != -1) {
    int commaIdx = body.indexOf(",", updateIdx);
    if (commaIdx != -1) {
      String updIdStr = body.substring(updateIdx + 12, commaIdx);
      lastUpdateId = updIdStr.toInt();
    }
    
    int textIdx = body.indexOf("\"text\":\"");
    if (textIdx != -1) {
      int endQuoteIdx = body.indexOf("\"", textIdx + 8);
      if (endQuoteIdx != -1) {
        String msgText = body.substring(textIdx + 8, endQuoteIdx);
        
        int chatIdx = body.indexOf("\"chat\":{");
        String senderChatId = "";
        if (chatIdx != -1) {
          int idIdx = body.indexOf("\"id\":", chatIdx);
          if (idIdx != -1) {
            int endIdIdx = body.indexOf(",", idIdx);
            if (endIdIdx != -1) {
              senderChatId = body.substring(idIdx + 5, endIdIdx);
              senderChatId.trim();
            }
          }
        }
        
        if (senderChatId.length() > 0) {
          handleBotCommand(msgText, senderChatId);
        }
      }
    }
  }
}

// Page Web Dashboard Konfigurasi di Flash Memory (PROGMEM)
const char html_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>Telegram Bot IoT Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700;800&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  
  <style>
    :root {
      --bg-color: #0b0f19;
      --card-bg: rgba(22, 28, 45, 0.65);
      --card-border: rgba(255, 255, 255, 0.08);
      --text-main: #f3f4f6;
      --text-sub: #9ca3af;
      --primary: #22d3ee;
      --secondary: #3b82f6;
      --accent: #240b36;
      --success: #10b981;
      --warning: #f59e0b;
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
      z-index: -1; opacity: 0.25; top: 15%; left: 15%;
    }
    body::after {
      content: ''; position: absolute; width: 280px; height: 280px;
      background: var(--secondary); border-radius: 50%; filter: blur(120px);
      z-index: -1; opacity: 0.25; bottom: 15%; right: 15%;
    }
    .container { width: 100%; max-width: 480px; z-index: 1; }
    header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 24px; }
    .logo-area { display: flex; align-items: center; gap: 12px; }
    .logo-icon {
      background: linear-gradient(135deg, var(--secondary), var(--primary));
      width: 44px; height: 44px; border-radius: 12px;
      display: flex; align-items: center; justify-content: center;
      box-shadow: 0 4px 15px rgba(34, 211, 238, 0.3);
    }
    .logo-icon i { color: #0b0f19; font-size: 20px; }
    .logo-title h1 { font-size: 16px; font-weight: 700; letter-spacing: -0.5px; }
    .logo-title p { font-size: 10px; color: var(--text-sub); }
    
    .status-badge {
      background: rgba(34, 211, 238, 0.08);
      border: 1px solid rgba(34, 211, 238, 0.25);
      padding: 6px 12px;
      border-radius: 20px;
      display: flex; align-items: center; gap: 8px;
      font-size: 10px; font-weight: 700; color: var(--primary);
    }
    .pulse-dot {
      width: 6px; height: 6px; background-color: var(--primary); border-radius: 50%;
      box-shadow: 0 0 0 0 rgba(34, 211, 238, 0.7);
      animation: pulse 1.6s infinite;
    }
    @keyframes pulse {
      0% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(34, 211, 238, 0.7); }
      70% { transform: scale(1); box-shadow: 0 0 0 6px rgba(34, 211, 238, 0); }
      100% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(34, 211, 238, 0); }
    }
    
    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(16px);
      border-radius: 20px;
      padding: 24px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
      margin-bottom: 20px;
    }
    .card-title { font-size: 13px; font-weight: 700; color: var(--text-main); margin-bottom: 16px; display: flex; align-items: center; gap: 8px; text-transform: uppercase; letter-spacing: 0.5px; }
    .card-title i { color: var(--primary); }
    
    /* Inputs */
    .input-group { margin-bottom: 16px; display: flex; flex-direction: column; gap: 6px; }
    .input-group label { font-size: 12px; color: var(--text-sub); font-weight: 600; }
    .input-wrapper { position: relative; display: flex; align-items: center; }
    .input-wrapper i.prefix-icon { position: absolute; left: 12px; color: var(--text-sub); font-size: 14px; }
    .input-wrapper i.toggle-pwd { position: absolute; right: 12px; color: var(--text-sub); font-size: 14px; cursor: pointer; }
    
    input[type="text"], input[type="password"], input[type="number"] {
      width: 100%;
      background-color: rgba(0, 0, 0, 0.2);
      border: 1px solid rgba(255, 255, 255, 0.08);
      color: #fff;
      padding: 12px 38px;
      font-size: 14px;
      font-weight: 600;
      border-radius: 10px;
      outline: none;
      transition: all 0.3s ease;
    }
    input:focus { border-color: var(--primary); box-shadow: 0 0 8px rgba(34, 211, 238, 0.2); }
    
    /* Toggle switch */
    .switch-row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 12px; }
    .switch-row label.switch-lbl { font-size: 13px; color: var(--text-main); font-weight: 600; }
    .switch { position: relative; display: inline-block; width: 44px; height: 24px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider {
      position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0;
      background-color: #374151; transition: .3s; border-radius: 24px;
    }
    .slider:before {
      position: absolute; content: ""; height: 16px; width: 16px; left: 4px; bottom: 4px;
      background-color: #9ca3af; transition: .3s; border-radius: 50%;
    }
    input:checked + .slider { background: linear-gradient(135deg, var(--secondary), var(--primary)); }
    input:checked + .slider:before { transform: translateX(20px); background-color: #0b0f19; }
    
    /* Buttons */
    .btn {
      width: 100%;
      border: none;
      color: #0b0f19;
      padding: 12px;
      border-radius: 10px;
      font-size: 13px;
      font-weight: 700;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      transition: all 0.3s ease;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    .btn-primary {
      background: linear-gradient(135deg, var(--secondary), var(--primary));
      box-shadow: 0 4px 15px rgba(34, 211, 238, 0.25);
    }
    .btn-primary:hover { transform: translateY(-1px); box-shadow: 0 6px 20px rgba(34, 211, 238, 0.4); }
    .btn-sec {
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid rgba(255, 255, 255, 0.08);
      color: #fff;
    }
    .btn-sec:hover { background: rgba(255, 255, 255, 0.1); }
    .btn:active { transform: translateY(0); }
    
    /* Telemetry grid */
    .tele-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 20px; }
    .tele-card {
      background: rgba(255,255,255,0.02);
      border: 1px solid var(--card-border);
      border-radius: 12px;
      padding: 12px 16px;
      display: flex; align-items: center; justify-content: space-between;
    }
    .tele-info span { font-size: 10px; color: var(--text-sub); text-transform: uppercase; font-weight: 600; }
    .tele-info p { font-size: 16px; font-weight: 700; color: #fff; }
    .tele-card i { font-size: 16px; color: var(--text-sub); }
    .tele-card.active i { color: var(--primary); text-shadow: 0 0 8px rgba(34, 211, 238, 0.5); }
    
    /* Toast */
    #toast {
      position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%) translateY(100px);
      background: rgba(16, 185, 129, 0.95); border: 1px solid rgba(16, 185, 129, 0.3);
      padding: 12px 24px; border-radius: 30px; color: #fff; font-size: 13px; font-weight: 600;
      display: flex; align-items: center; gap: 8px; box-shadow: 0 8px 30px rgba(0, 0, 0, 0.4);
      transition: transform 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275); z-index: 999;
    }
    #toast.show { transform: translateX(-50%) translateY(0); }
    
    footer { text-align: center; font-size: 11px; color: var(--text-sub); }
    footer a { color: var(--primary); text-decoration: none; font-weight: 500; }
  </style>
</head>
<body>
<div class="container">
  <header>
    <div class="logo-area">
      <div class="logo-icon"><i class="fa-brands fa-telegram"></i></div>
      <div class="logo-title">
        <h1>Telegram Bot IoT</h1>
        <p>ESP8266 Alert Modul</p>
      </div>
    </div>
    <div class="status-badge"><div class="pulse-dot"></div><span>ONLINE</span></div>
  </header>
  
  <!-- Telemetry Card -->
  <div class="tele-grid">
    <div class="tele-card">
      <div class="tele-info"><span>SUHU</span><p><span id="txtTemp">--.-</span>°C</p></div>
      <i class="fa-solid fa-thermometer"></i>
    </div>
    <div class="tele-card">
      <div class="tele-info"><span>HUMIDITY</span><p><span id="txtHumi">--</span>%</p></div>
      <i class="fa-solid fa-droplet"></i>
    </div>
    <div class="tele-card" id="cardR1">
      <div class="tele-info"><span>RELAY 1</span><p id="lblR1">OFF</p></div>
      <i class="fa-solid fa-lightbulb"></i>
    </div>
    <div class="tele-card" id="cardR2">
      <div class="tele-info"><span>RELAY 2</span><p id="lblR2">OFF</p></div>
      <i class="fa-solid fa-power-off"></i>
    </div>
  </div>

  <div class="card">
    <div class="card-title"><i class="fa-solid fa-gears"></i> Konfigurasi Bot</div>
    <form id="botForm">
      <!-- Bot Token -->
      <div class="input-group">
        <label>Bot Token Telegram</label>
        <div class="input-wrapper">
          <i class="fa-solid fa-key prefix-icon"></i>
          <input type="password" name="token" id="botToken" placeholder="Masukkan token bot...">
          <i class="fa-solid fa-eye toggle-pwd" onclick="togglePassword()"></i>
        </div>
      </div>
      
      <!-- Chat ID -->
      <div class="input-group">
        <label>Chat ID Telegram</label>
        <div class="input-wrapper">
          <i class="fa-solid fa-user prefix-icon"></i>
          <input type="text" name="chatid" id="chatId" placeholder="Masukkan Chat ID Anda...">
        </div>
      </div>
      
      <!-- Temp Alert Threshold -->
      <div class="input-group">
        <label>Batas Alarm Suhu (°C)</label>
        <div class="input-wrapper">
          <i class="fa-solid fa-bell prefix-icon"></i>
          <input type="number" step="0.1" name="temp" id="tempAlert" placeholder="Contoh: 35.0">
        </div>
      </div>
      
      <!-- Alert Enabled Toggle -->
      <div class="switch-row" style="margin: 20px 0;">
        <label class="switch-lbl">Kirim Notifikasi Alarm Suhu</label>
        <label class="switch">
          <input type="checkbox" name="alert" id="alertSwitch">
          <span class="slider"></span>
        </label>
      </div>
      
      <button class="btn btn-primary" type="submit" style="margin-bottom: 12px;">
        <i class="fa-solid fa-floppy-disk"></i> Simpan Konfigurasi
      </button>
      
      <button class="btn btn-sec" type="button" onclick="sendTestMessage()">
        <i class="fa-brands fa-telegram"></i> Kirim Pesan Tes Bot
      </button>
    </form>
  </div>
  
  <footer>
    Dibuat oleh <a href="https://duwiarsana.com" target="_blank">Duwi Arsana</a>
  </footer>
</div>

<div id="toast"><i class="fa-solid fa-circle-check"></i> Konfigurasi berhasil disimpan!</div>

<script>
  function togglePassword() {
    const pwdInput = document.getElementById("botToken");
    const eyeIcon = document.querySelector(".toggle-pwd");
    if (pwdInput.type === "password") {
      pwdInput.type = "text";
      eyeIcon.className = "fa-solid fa-eye-slash toggle-pwd";
    } else {
      pwdInput.type = "password";
      eyeIcon.className = "fa-solid fa-eye toggle-pwd";
    }
  }

  function fetchStatus() {
    fetch('/status')
      .then(res => res.json())
      .then(data => {
        document.getElementById("txtTemp").innerText = data.temp.toFixed(1);
        document.getElementById("txtHumi").innerText = Math.round(data.humi);
        
        // Update Relays
        const c1 = document.getElementById("cardR1");
        const c2 = document.getElementById("cardR2");
        const l1 = document.getElementById("lblR1");
        const l2 = document.getElementById("lblR2");

        if (data.r1) { c1.classList.add("active"); l1.innerText = "ON"; }
        else { c1.classList.remove("active"); l1.innerText = "OFF"; }

        if (data.r2) { c2.classList.add("active"); l2.innerText = "ON"; }
        else { c2.classList.remove("active"); l2.innerText = "OFF"; }
      })
      .catch(err => console.error(err));
  }

  // Load config
  fetch('/config')
    .then(res => res.json())
    .then(data => {
      document.getElementById("botToken").value = data.token;
      document.getElementById("chatId").value = data.chatid;
      document.getElementById("tempAlert").value = data.temp;
      document.getElementById("alertSwitch").checked = data.alert;
    })
    .catch(err => console.error(err));

  // Save config
  document.getElementById("botForm").addEventListener("submit", function(e) {
    e.preventDefault();
    const formData = new FormData(this);
    // Handle checkbox value
    if (!formData.has("alert")) {
      formData.append("alert", "false");
    } else {
      formData.set("alert", "true");
    }
    const params = new URLSearchParams(formData).toString();
    
    fetch('/save?' + params)
      .then(res => {
        if(res.ok) {
          const toast = document.getElementById("toast");
          toast.className = "show";
          setTimeout(() => { toast.className = ""; }, 2500);
        } else {
          alert("Gagal menyimpan konfigurasi!");
        }
      })
      .catch(err => {
        console.error(err);
        alert("Gagal menyimpan konfigurasi!");
      });
  });

  function sendTestMessage() {
    fetch('/test')
      .then(res => {
        if(res.ok) {
          alert("Pesan tes berhasil dikirim! Silakan periksa chat bot Telegram Anda.");
        } else {
          alert("Gagal mengirim pesan tes. Pastikan Token & Chat ID sudah benar.");
        }
      })
      .catch(err => {
        console.error(err);
        alert("Gagal menghubungi modul!");
      });
  }

  setInterval(fetchStatus, 3000);
  window.onload = fetchStatus;
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

  dht.begin();
  loadConfig();

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Serve static UI dari flash (PROGMEM)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html_template);
  });

  // API Status Telemetry
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"temp\":" + String(currentTemp) + ",";
    json += "\"humi\":" + String(currentHumidity) + ",";
    json += "\"r1\":" + String((digitalRead(RELAY1) == HIGH) ? "true" : "false") + ",";
    json += "\"r2\":" + String((digitalRead(RELAY2) == HIGH) ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
  });

  // API Config Loader
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"token\":\"" + String(config.token) + "\",";
    json += "\"chatid\":\"" + String(config.chatId) + "\",";
    json += "\"temp\":" + String(config.tempAlert) + ",";
    json += "\"alert\":" + String(config.alertEnabled ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
  });

  // API Config Saver
  server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("token")) {
      String tokenVal = request->getParam("token")->value();
      strncpy(config.token, tokenVal.c_str(), sizeof(config.token));
    }
    if (request->hasParam("chatid")) {
      String chatIdVal = request->getParam("chatid")->value();
      strncpy(config.chatId, chatIdVal.c_str(), sizeof(config.chatId));
    }
    if (request->hasParam("temp")) {
      config.tempAlert = request->getParam("temp")->value().toFloat();
    }
    if (request->hasParam("alert")) {
      config.alertEnabled = (request->getParam("alert")->value() == "true");
    }
    saveConfig();
    request->send(200, "text/plain", "OK");
  });

  // API Test Bot Message
  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
    if (strlen(config.token) > 0 && strlen(config.chatId) > 0) {
      sendTelegramMessage(String(config.chatId), "🤖 Halo! Ini adalah pesan tes sukses dari Modul IoT Relay ESP8266 Anda.");
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Error: Token atau ChatID kosong!");
    }
  });

  server.begin();
}

void loop() {
  // Baca DHT11 berkala setiap 2 detik
  if (millis() - lastDHTRead > 2000) {
    lastDHTRead = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t)) {
      currentTemp = t;
      currentHumidity = h;
      
      // Logika Notifikasi Peringatan Suhu
      if (config.alertEnabled && strlen(config.chatId) > 0) {
        if (currentTemp >= config.tempAlert && !alertSent) {
          String alertMsg = "⚠️ *PERINGATAN ALARM SUHU!*\n\n";
          alertMsg += "Suhu terdeteksi melebihi batas aman!\n";
          alertMsg += "🌡️ Suhu Saat Ini: " + String(currentTemp, 1) + " °C\n";
          alertMsg += "⚙️ Batas Alarm: " + String(config.tempAlert, 1) + " °C\n\n";
          alertMsg += "💡 Relay 1: " + String((digitalRead(RELAY1) == HIGH) ? "ON" : "OFF") + "\n";
          alertMsg += "💡 Relay 2: " + String((digitalRead(RELAY2) == HIGH) ? "ON" : "OFF");
          
          sendTelegramMessage(String(config.chatId), alertMsg);
          alertSent = true;
        } 
        else if (currentTemp < config.tempAlert - 1.0f && alertSent) {
          // Reset alert status jika suhu sudah aman kembali
          String safeMsg = "✅ *SUHU KEMBALI NORMAL*\n\n";
          safeMsg += "Suhu modul sudah turun di bawah batas aman.\n";
          safeMsg += "🌡️ Suhu Saat Ini: " + String(currentTemp, 1) + " °C";
          sendTelegramMessage(String(config.chatId), safeMsg);
          alertSent = false;
        }
      }
    }
  }

  // Periksa pesan Telegram masuk setiap 3 detik (non-blocking)
  if (millis() - lastBotCheck > 3000) {
    lastBotCheck = millis();
    checkTelegram();
  }
}
