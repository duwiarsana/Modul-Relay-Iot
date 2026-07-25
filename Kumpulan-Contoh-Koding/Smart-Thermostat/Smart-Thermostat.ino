// Smart Thermostat ESP8266 Web Interface
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

// WiFi credentials (ubah sesuai dengan WiFi Anda)
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

#define RELAY_COOLER 4 // Relay 1 (Cooler / Exhaust fan)
#define RELAY_HEATER 5 // Relay 2 (Heater / Pemanas)
#define DHTPIN 12      // Sensor Pin

struct ThermostatSetting {
  float targetTemp;
  float hysteresis;
  int mode; // 0: Auto, 1: Manual Cooler, 2: Manual Heater, 3: Off
  int sensorType; // 0: DHT11, 1: DHT22, 2: DS18B20
};

ThermostatSetting settings;
float currentTemp = 0.0;
float currentHumidity = 0.0;
bool coolerActive = false;
bool heaterActive = false;
unsigned long lastDHTRead = 0;

DHT* dht = nullptr;
OneWire* oneWire = nullptr;
DallasTemperature* sensors = nullptr;

AsyncWebServer server(80);

void initSensor() {
  if (dht != nullptr) { delete dht; dht = nullptr; }
  if (sensors != nullptr) { delete sensors; sensors = nullptr; }
  if (oneWire != nullptr) { delete oneWire; oneWire = nullptr; }

  pinMode(DHTPIN, INPUT_PULLUP);

  if (settings.sensorType == 0) {
    dht = new DHT(DHTPIN, DHT11);
    dht->begin();
    Serial.println("Sensor Inited: DHT11");
  } else if (settings.sensorType == 1) {
    dht = new DHT(DHTPIN, DHT22);
    dht->begin();
    Serial.println("Sensor Inited: DHT22");
  } else if (settings.sensorType == 2) {
    oneWire = new OneWire(DHTPIN);
    sensors = new DallasTemperature(oneWire);
    sensors->begin();
    Serial.println("Sensor Inited: DS18B20");
  }
}

void saveSettings() {
  Serial.println("Saving settings...");
  EEPROM.put(0, settings);
  EEPROM.commit();
}

void loadSettings() {
  Serial.println("Loading settings...");
  EEPROM.get(0, settings);
  
  // Validasi jika EEPROM kosong atau bernilai acak
  if (isnan(settings.targetTemp) || settings.targetTemp < 10.0f || settings.targetTemp > 45.0f) {
    settings.targetTemp = 26.0f;
  }
  if (isnan(settings.hysteresis) || settings.hysteresis < 0.1f || settings.hysteresis > 5.0f) {
    settings.hysteresis = 1.0f;
  }
  if (settings.mode < 0 || settings.mode > 3) {
    settings.mode = 0; // Default: AUTO
  }
  if (settings.sensorType < 0 || settings.sensorType > 2) {
    settings.sensorType = 0; // Default: DHT11
  }
  
  Serial.print("Target Temp: "); Serial.println(settings.targetTemp);
  Serial.print("Hysteresis: "); Serial.println(settings.hysteresis);
  Serial.print("Mode: "); Serial.println(settings.mode);
  Serial.print("Sensor Type: "); Serial.println(settings.sensorType);
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

    body::before, body::after {
      content: '';
      position: absolute;
      width: 300px;
      height: 300px;
      border-radius: 50%;
      filter: blur(120px);
      z-index: -1;
      opacity: 0.2;
    }
    body::before {
      background: var(--primary);
      top: 10%;
      left: 10%;
    }
    body::after {
      background: #8b5cf6;
      bottom: 10%;
      right: 10%;
    }

    .container {
      width: 100%;
      max-width: 440px;
      z-index: 1;
    }

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
      background: linear-gradient(135deg, #8b5cf6, var(--primary));
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
      font-weight: 750;
      letter-spacing: -0.5px;
    }

    .logo-title p {
      font-size: 10px;
      color: var(--text-sub);
    }

    .sys-status {
      padding: 6px 12px;
      border-radius: 20px;
      font-size: 10px;
      font-weight: 800;
      letter-spacing: 0.5px;
      text-transform: uppercase;
      transition: all 0.3s ease;
    }

    .standby {
      background: rgba(156, 163, 175, 0.08);
      border: 1px solid rgba(156, 163, 175, 0.2);
      color: var(--text-sub);
    }

    .cooling {
      background: rgba(0, 242, 254, 0.08);
      border: 1px solid rgba(0, 242, 254, 0.25);
      color: var(--primary);
      text-shadow: 0 0 8px rgba(0, 242, 254, 0.3);
    }

    .heating {
      background: rgba(255, 94, 98, 0.08);
      border: 1px solid rgba(255, 94, 98, 0.25);
      color: var(--accent-red);
      text-shadow: 0 0 8px rgba(255, 94, 98, 0.3);
    }

    /* Circular Dial */
    .dial-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(16px);
      border-radius: 24px;
      padding: 24px;
      display: flex;
      flex-direction: column;
      align-items: center;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
      margin-bottom: 16px;
    }

    .dial-container {
      position: relative;
      width: 200px;
      height: 200px;
      display: flex;
      align-items: center;
      justify-content: center;
    }

    .dial-svg {
      width: 100%;
      height: 100%;
      transform: rotate(0deg);
    }

    .dial-track {
      stroke: rgba(255, 255, 255, 0.03);
    }

    .dial-progress {
      transition: stroke-dashoffset 0.6s cubic-bezier(0.4, 0, 0.2, 1);
    }

    .dial-content {
      position: absolute;
      display: flex;
      flex-direction: column;
      align-items: center;
      text-align: center;
    }

    .dial-label {
      font-size: 9px;
      font-weight: 700;
      color: var(--text-sub);
      letter-spacing: 1px;
      text-transform: uppercase;
      margin-bottom: 2px;
    }

    .dial-value {
      font-size: 38px;
      font-weight: 800;
      color: #fff;
      display: flex;
      align-items: baseline;
      line-height: 1;
    }

    .dial-unit {
      font-size: 18px;
      color: var(--text-sub);
      font-weight: 500;
      margin-left: 1px;
    }

    .dial-target {
      font-size: 11px;
      color: var(--text-sub);
      margin-top: 4px;
      font-weight: 600;
    }

    /* Grid Layout */
    .tele-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      margin-bottom: 16px;
    }

    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(16px);
      border-radius: 20px;
      padding: 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.3);
      transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
    }

    .card-info span {
      font-size: 9px;
      font-weight: 700;
      color: var(--text-sub);
      letter-spacing: 0.5px;
      text-transform: uppercase;
    }

    .card-info p {
      font-size: 16px;
      font-weight: 750;
      color: #fff;
      margin-top: 2px;
    }

    .card-icon {
      width: 38px;
      height: 38px;
      background: rgba(255, 255, 255, 0.02);
      border: 1px solid rgba(255, 255, 255, 0.05);
      border-radius: 10px;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 16px;
      color: var(--text-sub);
      transition: all 0.4s ease;
    }

    /* Active Glowing Classes */
    .active-cooler {
      border-color: rgba(0, 242, 254, 0.3);
      box-shadow: 0 8px 25px var(--accent-blue-glow);
    }
    .active-cooler .card-icon {
      background: rgba(0, 242, 254, 0.1);
      color: var(--primary);
      border-color: rgba(0, 242, 254, 0.2);
      box-shadow: 0 0 10px rgba(0, 242, 254, 0.2);
    }
    .active-cooler p {
      color: var(--primary);
    }

    .active-heater {
      border-color: rgba(255, 94, 98, 0.3);
      box-shadow: 0 8px 25px var(--accent-red-glow);
    }
    .active-heater .card-icon {
      background: rgba(255, 94, 98, 0.1);
      color: var(--accent-red);
      border-color: rgba(255, 94, 98, 0.2);
      box-shadow: 0 0 10px rgba(255, 94, 98, 0.2);
    }
    .active-heater p {
      color: var(--accent-red);
    }

    /* Adjusters & Controls */
    .settings-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      margin-bottom: 16px;
    }

    .adjuster-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 20px;
      padding: 16px;
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 8px;
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
      gap: 14px;
    }

    .btn-adj {
      width: 32px;
      height: 32px;
      border-radius: 50%;
      background: rgba(255, 255, 255, 0.03);
      border: 1px solid rgba(255, 255, 255, 0.06);
      color: #fff;
      font-size: 12px;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      transition: all 0.2s;
    }

    .btn-adj:hover {
      background: rgba(255, 255, 255, 0.08);
      transform: scale(1.05);
    }

    .btn-adj:active {
      transform: scale(0.95);
    }

    .adj-value {
      font-size: 18px;
      font-weight: 750;
      color: #fff;
      min-width: 44px;
      text-align: center;
    }

    /* Mode Selector Card */
    .mode-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 20px;
      padding: 16px;
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.3);
    }

    .mode-title {
      font-size: 11px;
      font-weight: 700;
      color: var(--text-sub);
      margin-bottom: 12px;
      text-transform: uppercase;
      letter-spacing: 0.5px;
      display: flex;
      align-items: center;
      gap: 6px;
    }

    .mode-title i {
      color: var(--primary);
    }

    .mode-buttons {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 8px;
    }

    .btn-mode {
      background: rgba(255, 255, 255, 0.02);
      border: 1px solid rgba(255, 255, 255, 0.05);
      border-radius: 10px;
      padding: 10px 0;
      color: var(--text-sub);
      font-size: 12px;
      font-weight: 700;
      cursor: pointer;
      transition: all 0.3s;
      text-transform: uppercase;
    }

    .btn-mode:hover {
      background: rgba(255, 255, 255, 0.05);
      color: #fff;
    }

    .btn-mode.active {
      background: linear-gradient(135deg, var(--secondary), var(--primary));
      color: #080b13;
      border: none;
      box-shadow: 0 4px 15px rgba(0, 242, 254, 0.25);
    }

    /* Toast */
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
      z-index: 99;
    }

    #toast.show {
      transform: translateX(-50%) translateY(0);
    }

    footer {
      text-align: center;
      font-size: 11px;
      color: var(--text-sub);
      padding-top: 16px;
    }

    footer a {
      color: var(--primary);
      text-decoration: none;
      font-weight: 550;
    }
  </style>
</head>
<body>

<div class="container">
  <header>
    <div class="logo-area">
      <div class="logo-icon"><i class="fa-solid fa-temperature-empty"></i></div>
      <div class="logo-title">
        <h1>Smart Thermostat</h1>
        <p>Climate Control System</p>
      </div>
    </div>
    <div class="sys-status standby" id="statusBadge">STANDBY</div>
  </header>

  <!-- Circular Thermostat Dial -->
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
    <div class="card" id="cardHumi">
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

  <!-- Sensor Selector -->
  <div class="mode-card" style="margin-top: 16px; display: flex; flex-direction: row; justify-content: space-between; align-items: center; padding: 15px 20px;">
    <div class="mode-title" style="margin-bottom: 0; display: flex; align-items: center; gap: 8px;"><i class="fa-solid fa-microchip"></i> Jenis Sensor (GPIO 12)</div>
    <select id="sensorTypeSelect" onchange="setSensor(this.value)" style="background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.08); color: #fff; border-radius: 8px; padding: 8px 16px; outline: none; font-size: 13px; font-weight: 600; cursor: pointer; color-scheme: dark;">
      <option value="0">DHT11</option>
      <option value="1">DHT22</option>
      <option value="2">DS18B20</option>
    </select>
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
    // Normalisasi visual dial dari range 15°C sampai 38°C
    const minTemp = 15;
    const maxTemp = 38;
    let percent = (tempVal - minTemp) / (maxTemp - minTemp);
    if (percent < 0) percent = 0;
    if (percent > 1) percent = 1;
    
    const offset = strokeDash - (percent * strokeDash);
    progressCircle.style.strokeDashoffset = offset;
  }

  function fetchStatus() {
    fetch('/status')
      .then(res => res.json())
      .then(data => {
        // Update Telemetry
        document.getElementById("currTemp").innerText = data.temp.toFixed(1);
        updateDial(data.temp);

        // Handle sensor label and humidity widget opacity
        document.getElementById("sensorTypeSelect").value = data.sensor;
        const cardHumi = document.getElementById("cardHumi");
        if (data.sensor === 2) {
          cardHumi.style.opacity = "0.35";
          document.getElementById("currHumi").innerText = "--";
        } else {
          cardHumi.style.opacity = "1";
          document.getElementById("currHumi").innerText = Math.round(data.humi);
        }

        // Update Adjusters
        targetTemp = data.target;
        hysteresis = data.hyst;
        document.getElementById("adjTarget").innerText = targetTemp.toFixed(1);
        document.getElementById("targetVal").innerText = targetTemp.toFixed(1);
        document.getElementById("adjHyst").innerText = hysteresis.toFixed(1);

        // Update Relays Card Glowing
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
    for (let i = 0; i <= 3; i++) {
      const btn = document.getElementById(`mode${i}`);
      if (i === currentMode) btn.classList.add("active");
      else btn.classList.remove("active");
    }
    triggerSave();
  }

  function setSensor(sensorVal) {
    fetch(`/set?sensor=${sensorVal}`)
      .then(res => {
        if (res.ok) {
          const toast = document.getElementById("toast");
          toast.className = "show";
          setTimeout(() => { toast.className = ""; }, 2500);
          fetchStatus();
        }
      })
      .catch(err => console.error(err));
  }

  function triggerSave() {
    if (saveTimeout) clearTimeout(saveTimeout);
    
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

  loadSettings();
  initSensor();

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
    json += "\"mode\":" + String(settings.mode) + ",";
    json += "\"sensor\":" + String(settings.sensorType);
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
    if (request->hasParam("sensor")) {
      settings.sensorType = request->getParam("sensor")->value().toInt();
      initSensor();
    }
    saveSettings();
    request->send(200, "text/plain", "OK");
  });

  server.begin();

  // Inisialisasi mDNS
  if (MDNS.begin("smart-thermostat")) {
    Serial.println("mDNS responder started! Akses di: http://smart-thermostat.local");
  }
}


void loop() {
  MDNS.update();

  // Baca DHT11 berkala setiap 2 detik
  if (millis() - lastDHTRead > 2000) {
    lastDHTRead = millis();
    float t = NAN;
    float h = NAN;
    
    if (settings.sensorType == 0 || settings.sensorType == 1) {
      if (dht != nullptr) {
        t = dht->readTemperature();
        h = dht->readHumidity();
        if (isnan(t) || isnan(h)) {
          delay(50);
          t = dht->readTemperature();
          h = dht->readHumidity();
        }
      }
    } else if (settings.sensorType == 2) {
      if (sensors != nullptr) {
        sensors->requestTemperatures();
        t = sensors->getTempCByIndex(0);
        if (t == DEVICE_DISCONNECTED_C) t = NAN;
        h = 0.0; // DS18B20 tidak mengukur kelembaban
      }
    }

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
