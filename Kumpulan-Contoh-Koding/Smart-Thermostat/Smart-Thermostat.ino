// Smart Thermostat ESP8266 Web Interface
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <EEPROM.h>

// WiFi credentials (ubah sesuai dengan WiFi Anda)
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

#define RELAY_COOLER 4 // Relay 1 (Cooler / Exhaust fan)
#define RELAY_HEATER 5 // Relay 2 (Heater / Pemanas)
#define DHTPIN 12      // Sensor DHT11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
AsyncWebServer server(80);

struct ThermostatSetting {
  float targetTemp;
  float hysteresis;
  int mode; // 0: Auto, 1: Manual Cooler, 2: Manual Heater, 3: Off
};

ThermostatSetting settings;
float currentTemp = 0.0;
float currentHumidity = 0.0;
bool coolerActive = false;
bool heaterActive = false;
unsigned long lastDHTRead = 0;

void saveSettings() {
  Serial.println("Saving settings...");
  EEPROM.put(0, settings);
  EEPROM.commit();
}

void loadSettings() {
  Serial.println("Loading settings...");
  EEPROM.get(0, settings);
  
  // Validasi jika EEPROM kosong atau bernilai acak (NaN atau out of range)
  if (isnan(settings.targetTemp) || settings.targetTemp < 10.0f || settings.targetTemp > 45.0f) {
    settings.targetTemp = 26.0f;
  }
  if (isnan(settings.hysteresis) || settings.hysteresis < 0.1f || settings.hysteresis > 5.0f) {
    settings.hysteresis = 1.0f;
  }
  if (settings.mode < 0 || settings.mode > 3) {
    settings.mode = 0; // Default: AUTO
  }
  
  Serial.print("Target Temp: "); Serial.println(settings.targetTemp);
  Serial.print("Hysteresis: "); Serial.println(settings.hysteresis);
  Serial.print("Mode: "); Serial.println(settings.mode);
}

// HTML & CSS Template disimpan di Flash Memory (PROGMEM) untuk efisiensi RAM
const char html_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>Smart Thermostat Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- Google Fonts & FontAwesome -->
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700;800&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  
  <style>
    :root {
      --bg-color: #080b13;
      --card-bg: rgba(17, 22, 37, 0.65);
      --card-border: rgba(255, 255, 255, 0.08);
      --text-main: #f3f4f6;
      --text-sub: #9ca3af;
      --primary: #00f2fe;
      --secondary: #4facfe;
      --accent-red: #ff5e62;
      --accent-red-glow: rgba(255, 94, 98, 0.4);
      --accent-blue-glow: rgba(0, 242, 254, 0.4);
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: 'Plus Jakarta Sans', sans-serif;
    }

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

    /* Ambient Background Glows */
    body::before {
      content: '';
      position: absolute;
      width: 300px;
      height: 300px;
      background: var(--primary);
      border-radius: 50%;
      filter: blur(120px);
      z-index: -1;
      opacity: 0.2;
      top: 10%;
      left: 10%;
    }

    body::after {
      content: '';
      position: absolute;
      width: 300px;
      height: 300px;
      background: #8b5cf6;
      border-radius: 50%;
      filter: blur(120px);
      z-index: -1;
      opacity: 0.2;
      bottom: 10%;
      right: 10%;
    }

    .container {
      width: 100%;
      max-width: 450px;
      z-index: 1;
    }

    /* Header */
    header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 24px;
    }

    .logo-area {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    .logo-icon {
      background: linear-gradient(135deg, var(--secondary), var(--primary));
      width: 44px;
      height: 44px;
      border-radius: 12px;
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 4px 15px rgba(0, 242, 254, 0.3);
    }

    .logo-icon i {
      color: #080b13;
      font-size: 20px;
    }

    .logo-title h1 {
      font-size: 16px;
      font-weight: 700;
      letter-spacing: -0.5px;
    }

    .logo-title p {
      font-size: 10px;
      color: var(--text-sub);
    }

    .sys-status {
      font-size: 11px;
      font-weight: 700;
      padding: 6px 14px;
      border-radius: 20px;
      border: 1px solid rgba(255, 255, 255, 0.08);
      background: rgba(255, 255, 255, 0.03);
      text-transform: uppercase;
      letter-spacing: 0.5px;
      transition: all 0.3s ease;
    }

    .sys-status.cooling {
      color: var(--primary);
      border-color: rgba(0, 242, 254, 0.2);
      background: rgba(0, 242, 254, 0.08);
      box-shadow: 0 0 15px rgba(0, 242, 254, 0.15);
    }

    .sys-status.heating {
      color: var(--accent-red);
      border-color: rgba(255, 94, 98, 0.2);
      background: rgba(255, 94, 98, 0.08);
      box-shadow: 0 0 15px rgba(255, 94, 98, 0.15);
    }

    .sys-status.standby {
      color: #10b981;
      border-color: rgba(16, 185, 129, 0.2);
      background: rgba(16, 185, 129, 0.08);
    }

    /* Dial Gauge Card */
    .dial-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(16px);
      border-radius: 24px;
      padding: 30px 20px;
      text-align: center;
      box-shadow: 0 12px 40px rgba(0, 0, 0, 0.4);
      margin-bottom: 20px;
      position: relative;
      overflow: hidden;
    }

    .dial-container {
      position: relative;
      width: 200px;
      height: 200px;
      margin: 0 auto 20px auto;
    }

    .dial-svg {
      width: 100%;
      height: 100%;
    }

    .dial-track {
      stroke: rgba(255, 255, 255, 0.04);
    }

    .dial-progress {
      transition: stroke-dashoffset 0.6s ease;
    }

    .dial-content {
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      width: 100%;
    }

    .dial-label {
      font-size: 10px;
      font-weight: 700;
      color: var(--text-sub);
      letter-spacing: 1px;
      text-transform: uppercase;
      margin-bottom: 2px;
    }

    .dial-value {
      font-size: 48px;
      font-weight: 800;
      letter-spacing: -1px;
      color: #fff;
      display: flex;
      align-items: baseline;
      line-height: 1;
    }

    .dial-unit {
      font-size: 20px;
      color: var(--text-sub);
      font-weight: 500;
      margin-left: 2px;
    }

    .dial-target {
      font-size: 13px;
      color: var(--text-sub);
      margin-top: 6px;
      font-weight: 500;
    }

    .dial-target span {
      color: #fff;
      font-weight: 700;
    }

    /* Telemetry grid */
    .tele-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 16px;
      margin-bottom: 20px;
    }

    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 16px 20px;
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.3);
      display: flex;
      align-items: center;
      justify-content: space-between;
    }

    .card-info {
      display: flex;
      flex-direction: column;
      gap: 2px;
    }

    .card-info span {
      font-size: 11px;
      color: var(--text-sub);
      font-weight: 600;
      text-transform: uppercase;
    }

    .card-info p {
      font-size: 18px;
      font-weight: 700;
      color: #fff;
    }

    .card-icon {
      width: 40px;
      height: 40px;
      border-radius: 12px;
      background: rgba(255, 255, 255, 0.03);
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 18px;
      color: var(--text-sub);
      transition: all 0.3s ease;
      border: 1px solid rgba(255, 255, 255, 0.02);
    }

    .card.active-cooler .card-icon {
      background: rgba(0, 242, 254, 0.1);
      color: var(--primary);
      box-shadow: 0 0 15px var(--accent-blue-glow);
      border-color: rgba(0, 242, 254, 0.2);
    }

    .card.active-heater .card-icon {
      background: rgba(255, 94, 98, 0.1);
      color: var(--accent-red);
      box-shadow: 0 0 15px var(--accent-red-glow);
      border-color: rgba(255, 94, 98, 0.2);
    }

    /* Mode selector */
    .mode-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 20px;
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.3);
      margin-bottom: 20px;
    }

    .mode-title {
      font-size: 12px;
      font-weight: 700;
      color: var(--text-sub);
      text-transform: uppercase;
      letter-spacing: 1px;
      margin-bottom: 15px;
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .mode-title i {
      color: var(--primary);
    }

    .mode-buttons {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 8px;
      background: rgba(0, 0, 0, 0.2);
      padding: 4px;
      border-radius: 12px;
      border: 1px solid rgba(255, 255, 255, 0.03);
    }

    .btn-mode {
      background: transparent;
      border: none;
      color: var(--text-sub);
      padding: 10px 5px;
      font-size: 11px;
      font-weight: 700;
      border-radius: 8px;
      cursor: pointer;
      transition: all 0.2s ease;
      text-transform: uppercase;
    }

    .btn-mode.active {
      background: linear-gradient(135deg, var(--secondary), var(--primary));
      color: #080b13;
      box-shadow: 0 4px 12px rgba(0, 242, 254, 0.25);
    }

    /* Adjusters Card */
    .settings-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 16px;
      margin-bottom: 20px;
    }

    .adjuster-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 16px 20px;
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.3);
      display: flex;
      flex-direction: column;
      gap: 10px;
    }

    .adjuster-title {
      font-size: 11px;
      font-weight: 700;
      color: var(--text-sub);
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }

    .adjuster-controls {
      display: flex;
      align-items: center;
      justify-content: space-between;
      background: rgba(0, 0, 0, 0.15);
      border: 1px solid rgba(255, 255, 255, 0.04);
      border-radius: 10px;
      padding: 4px;
    }

    .btn-adj {
      width: 32px;
      height: 32px;
      border-radius: 8px;
      border: none;
      background: rgba(255,255,255,0.04);
      color: #fff;
      font-size: 14px;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      transition: all 0.2s ease;
    }

    .btn-adj:hover {
      background: rgba(255,255,255,0.1);
    }

    .btn-adj:active {
      transform: scale(0.9);
    }

    .adj-value {
      font-size: 18px;
      font-weight: 800;
      color: #fff;
      font-variant-numeric: tabular-nums;
    }

    /* Toast Notification */
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
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.4);
      transition: transform 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
      z-index: 999;
    }

    #toast.show {
      transform: translateX(-50%) translateY(0);
    }

    footer {
      text-align: center;
      font-size: 11px;
      color: var(--text-sub);
    }

    footer a {
      color: var(--primary);
      text-decoration: none;
      font-weight: 500;
    }
  </style>
</head>
<body>

<div class="container">
  <!-- Header -->
  <header>
    <div class="logo-area">
      <div class="logo-icon">
        <i class="fa-solid fa-temperature-half"></i>
      </div>
      <div class="logo-title">
        <h1>Smart Thermostat</h1>
        <p>IoT Climate Control</p>
      </div>
    </div>
    <div id="statusBadge" class="sys-status standby">STANDBY</div>
  </header>

  <!-- Dial Card -->
  <div class="dial-card">
    <div class="dial-container">
      <svg class="dial-svg" viewBox="0 0 100 100">
        <!-- background circle track -->
        <circle class="dial-track" cx="50" cy="50" r="40" fill="none" stroke-width="5"/>
        <!-- progress circle -->
        <circle class="dial-progress" id="dialProgress" cx="50" cy="50" r="40" fill="none" stroke="url(#dialGradient)" stroke-width="5" stroke-dasharray="251.2" stroke-dashoffset="251.2" stroke-linecap="round" transform="rotate(-90 50 50)"/>
        <defs>
          <linearGradient id="dialGradient" x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stop-color="#00f2fe"/>
            <stop offset="100%" stop-color="#8b5cf6"/>
          </linearGradient>
        </defs>
      </svg>
      <div class="dial-content">
        <div class="dial-label">SUHU SEKARANG</div>
        <div class="dial-value"><span id="currTemp">--.-</span><span class="dial-unit">°C</span></div>
        <div class="dial-target">Target: <span id="targetVal">--.-</span>°C</div>
      </div>
    </div>
  </div>

  <!-- Telemetry Row -->
  <div class="tele-grid">
    <!-- Humidity Card -->
    <div class="card">
      <div class="card-info">
        <span>KELEMBABAN</span>
        <p><span id="currHumi">--</span>%</p>
      </div>
      <div class="card-icon"><i class="fa-solid fa-droplet"></i></div>
    </div>

    <!-- Cooler Card -->
    <div class="card" id="cardCooler">
      <div class="card-info">
        <span>COOLER (R1)</span>
        <p id="txtCooler">OFF</p>
      </div>
      <div class="card-icon"><i class="fa-solid fa-fan"></i></div>
    </div>

    <!-- Heater Card -->
    <div class="card" id="cardHeater" style="grid-column: span 2;">
      <div class="card-info">
        <span>HEATER (R2)</span>
        <p id="txtHeater">OFF</p>
      </div>
      <div class="card-icon" style="margin-left: auto;"><i class="fa-solid fa-fire"></i></div>
    </div>
  </div>

  <!-- Settings Grid -->
  <div class="settings-grid">
    <!-- Target Temp Adjuster -->
    <div class="adjuster-card">
      <span class="adjuster-title">Set Target Suhu</span>
      <div class="adjuster-controls">
        <button class="btn-adj" onclick="adjustTarget(-0.5)"><i class="fa-solid fa-minus"></i></button>
        <span class="adj-value" id="adjTarget">26.0</span>
        <button class="btn-adj" onclick="adjustTarget(0.5)"><i class="fa-solid fa-plus"></i></button>
      </div>
    </div>

    <!-- Hysteresis Adjuster -->
    <div class="adjuster-card">
      <span class="adjuster-title">Set Histeresis</span>
      <div class="adjuster-controls">
        <button class="btn-adj" onclick="adjustHyst(-0.1)"><i class="fa-solid fa-minus"></i></button>
        <span class="adj-value" id="adjHyst">1.0</span>
        <button class="btn-adj" onclick="adjustHyst(0.1)"><i class="fa-solid fa-plus"></i></button>
      </div>
    </div>
  </div>

  <!-- Mode Selector -->
  <div class="mode-card">
    <div class="mode-title"><i class="fa-solid fa-sliders"></i> Mode Kontrol</div>
    <div class="mode-buttons">
      <button class="btn-mode" id="mode0" onclick="setMode(0)">Auto</button>
      <button class="btn-mode" id="mode1" onclick="setMode(1)">Cool</button>
      <button class="btn-mode" id="mode2" onclick="setMode(2)">Heat</button>
      <button class="btn-mode" id="mode3" onclick="setMode(3)">Off</button>
    </div>
  </div>

  <!-- Footer -->
  <footer>
    Dibuat oleh <a href="https://duwiarsana.com" target="_blank">Duwi Arsana</a>
  </footer>
</div>

<div id="toast"><i class="fa-solid fa-circle-check"></i> Pengaturan disimpan!</div>

<script>
  let targetTemp = 26.0;
  let hysteresis = 1.0;
  let currentMode = 0;
  let saveTimeout = null;

  const progressCircle = document.getElementById("dialProgress");
  const strokeDash = 251.2; // 2 * pi * r = 2 * 3.14 * 40

  function updateDial(tempVal) {
    const minTemp = 15.0;
    const maxTemp = 40.0;
    
    // Batasi nilai temperatur untuk dial
    let val = Math.max(minTemp, Math.min(maxTemp, tempVal));
    let pct = (val - minTemp) / (maxTemp - minTemp);
    let offset = strokeDash - (strokeDash * pct);
    progressCircle.style.strokeDashoffset = offset;
  }

  function fetchStatus() {
    fetch('/status')
      .then(res => res.json())
      .then(data => {
        // Update Telemetry
        document.getElementById("currTemp").innerText = data.temp.toFixed(1);
        document.getElementById("currHumi").innerText = Math.round(data.humi);
        document.getElementById("targetVal").innerText = data.target.toFixed(1);
        
        // Update Adjusters UI (jika sedang tidak diedit)
        if (!saveTimeout) {
          targetTemp = data.target;
          hysteresis = data.hyst;
          document.getElementById("adjTarget").innerText = targetTemp.toFixed(1);
          document.getElementById("adjHyst").innerText = hysteresis.toFixed(1);
        }

        updateDial(data.temp);

        // Update Relays
        const cardCooler = document.getElementById("cardCooler");
        const cardHeater = document.getElementById("cardHeater");
        const txtCooler = document.getElementById("txtCooler");
        const txtHeater = document.getElementById("txtHeater");
        
        if (data.cooler) {
          cardCooler.className = "card active-cooler";
          txtCooler.innerText = "ACTIVE (ON)";
        } else {
          cardCooler.className = "card";
          txtCooler.innerText = "OFF";
        }

        if (data.heater) {
          cardHeater.className = "card active-heater";
          txtHeater.innerText = "ACTIVE (ON)";
        } else {
          cardHeater.className = "card";
          txtHeater.innerText = "OFF";
        }

        // Update Mode Buttons
        currentMode = data.mode;
        for (let i = 0; i <= 3; i++) {
          const btn = document.getElementById(`mode${i}`);
          if (i === currentMode) btn.classList.add("active");
          else btn.classList.remove("active");
        }

        // Update Status Badge
        const badge = document.getElementById("statusBadge");
        if (currentMode === 3) {
          badge.className = "sys-status";
          badge.innerText = "SYSTEM OFF";
        } else if (currentMode === 1) {
          badge.className = "sys-status cooling";
          badge.innerText = "MANUAL COOL";
        } else if (currentMode === 2) {
          badge.className = "sys-status heating";
          badge.innerText = "MANUAL HEAT";
        } else { // AUTO
          if (data.cooler) {
            badge.className = "sys-status cooling";
            badge.innerText = "COOLING";
          } else if (data.heater) {
            badge.className = "sys-status heating";
            badge.innerText = "HEATING";
          } else {
            badge.className = "sys-status standby";
            badge.innerText = "STANDBY";
          }
        }
      })
      .catch(err => console.error("Error fetching status:", err));
  }

  function adjustTarget(val) {
    targetTemp = parseFloat((targetTemp + val).toFixed(1));
    if (targetTemp < 15) targetTemp = 15;
    if (targetTemp > 38) targetTemp = 38;
    document.getElementById("adjTarget").innerText = targetTemp.toFixed(1);
    document.getElementById("targetVal").innerText = targetTemp.toFixed(1);
    triggerSave();
  }

  function adjustHyst(val) {
    hysteresis = parseFloat((hysteresis + val).toFixed(1));
    if (hysteresis < 0.2) hysteresis = 0.2;
    if (hysteresis > 4.0) hysteresis = 4.0;
    document.getElementById("adjHyst").innerText = hysteresis.toFixed(1);
    triggerSave();
  }

  function setMode(modeVal) {
    currentMode = modeVal;
    // Update local UI immediately for responsiveness
    for (let i = 0; i <= 3; i++) {
      const btn = document.getElementById(`mode${i}`);
      if (i === currentMode) btn.classList.add("active");
      else btn.classList.remove("active");
    }
    triggerSave();
  }

  function triggerSave() {
    if (saveTimeout) clearTimeout(saveTimeout);
    
    // Tunggu 800ms setelah user selesai menekan tombol sebelum mengirim ke ESP
    saveTimeout = setTimeout(() => {
      fetch(`/set?target=${targetTemp}&hyst=${hysteresis}&mode=${currentMode}`)
        .then(res => {
          if (res.ok) {
            const toast = document.getElementById("toast");
            toast.className = "show";
            setTimeout(() => { toast.className = ""; }, 2500);
          }
          saveTimeout = null;
          fetchStatus();
        })
        .catch(err => {
          console.error(err);
          saveTimeout = null;
        });
    }, 800);
  }

  setInterval(fetchStatus, 2000);
  window.onload = fetchStatus;
</script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  pinMode(RELAY_COOLER, OUTPUT);
  pinMode(RELAY_HEATER, OUTPUT);
  digitalWrite(RELAY_COOLER, LOW);
  digitalWrite(RELAY_HEATER, LOW);

  dht.begin();
  loadSettings();

  // Koneksi ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Route Utama
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html_template);
  });

  // API Status
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"temp\":" + String(currentTemp) + ",";
    json += "\"humi\":" + String(currentHumidity) + ",";
    json += "\"cooler\":" + String(coolerActive ? "true" : "false") + ",";
    json += "\"heater\":" + String(heaterActive ? "true" : "false") + ",";
    json += "\"target\":" + String(settings.targetTemp) + ",";
    json += "\"hyst\":" + String(settings.hysteresis) + ",";
    json += "\"mode\":" + String(settings.mode);
    json += "}";
    request->send(200, "application/json", json);
  });

  // API Set/Save
  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("target")) {
      settings.targetTemp = request->getParam("target")->value().toFloat();
    }
    if (request->hasParam("hyst")) {
      settings.hysteresis = request->getParam("hyst")->value().toFloat();
    }
    if (request->hasParam("mode")) {
      settings.mode = request->getParam("mode")->value().toInt();
    }
    saveSettings();
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  // Baca DHT11 berkala setiap 2 detik
  if (millis() - lastDHTRead > 2000) {
    lastDHTRead = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t)) currentTemp = t;
    if (!isnan(h)) currentHumidity = h;

    // Logika kontrol iklim hanya aktif jika mode = 0 (AUTO)
    if (settings.mode == 0) {
      // Logika Pendingin (Cooling)
      if (currentTemp >= settings.targetTemp + settings.hysteresis) {
        coolerActive = true;
        heaterActive = false; // Hindari heater menyala bersamaan
      } else if (currentTemp <= settings.targetTemp) {
        coolerActive = false;
      }

      // Logika Pemanas (Heating)
      if (currentTemp <= settings.targetTemp - settings.hysteresis) {
        heaterActive = true;
        coolerActive = false; // Hindari cooler menyala bersamaan
      } else if (currentTemp >= settings.targetTemp) {
        heaterActive = false;
      }
      
      // Update relay fisik
      digitalWrite(RELAY_COOLER, coolerActive ? HIGH : LOW);
      digitalWrite(RELAY_HEATER, heaterActive ? HIGH : LOW);
    }
  }

  // Jika mode MANUAL atau OFF, kontrol relay didasarkan pada pengaturan mode langsung
  if (settings.mode == 1) { // Manual Cooler
    coolerActive = true;
    heaterActive = false;
    digitalWrite(RELAY_COOLER, HIGH);
    digitalWrite(RELAY_HEATER, LOW);
  } 
  else if (settings.mode == 2) { // Manual Heater
    coolerActive = false;
    heaterActive = true;
    digitalWrite(RELAY_COOLER, LOW);
    digitalWrite(RELAY_HEATER, HIGH);
  } 
  else if (settings.mode == 3) { // OFF
    coolerActive = false;
    heaterActive = false;
    digitalWrite(RELAY_COOLER, LOW);
    digitalWrite(RELAY_HEATER, LOW);
  }
}
