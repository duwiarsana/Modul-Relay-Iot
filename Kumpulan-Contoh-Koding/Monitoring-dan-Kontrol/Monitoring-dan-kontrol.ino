// Copyright (c) 2025 Anak Agung Duwi Arsana
// Website: https://duwiarsana.com

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#define DHTPIN 12
#define RELAY1_PIN 4
#define RELAY2_PIN 5

const char* ssid = "SSID"; //ganti pakai nama hotspot yang ada
const char* password = "PASSWORD"; //ganti juga password yang sesuai

ESP8266WebServer server(80);

struct ModulConfig {
  bool relay1;
  bool relay2;
  byte sensorType; // 0 = DHT11, 1 = DHT22, 2 = DS18B20
};

ModulConfig config;
bool relay1 = false;
bool relay2 = false;
float suhu = 0;
float kelembaban = 0;
unsigned long lastDHTRead = 0;

DHT* dht = nullptr;
OneWire* oneWire = nullptr;
DallasTemperature* sensors = nullptr;

void initSensor() {
  if (dht != nullptr) { delete dht; dht = nullptr; }
  if (sensors != nullptr) { delete sensors; sensors = nullptr; }
  if (oneWire != nullptr) { delete oneWire; oneWire = nullptr; }

  if (config.sensorType == 0) {
    dht = new DHT(DHTPIN, DHT11);
    dht->begin();
    Serial.println("Sensor Inited: DHT11");
  } else if (config.sensorType == 1) {
    dht = new DHT(DHTPIN, DHT22);
    dht->begin();
    Serial.println("Sensor Inited: DHT22");
  } else if (config.sensorType == 2) {
    oneWire = new OneWire(DHTPIN);
    sensors = new DallasTemperature(oneWire);
    sensors->begin();
    Serial.println("Sensor Inited: DS18B20");
  }
}

void saveRelayState() {
  config.relay1 = relay1;
  config.relay2 = relay2;
  EEPROM.put(0, config);
  EEPROM.commit();
}

void loadConfig() {
  EEPROM.get(0, config);
  if (config.sensorType > 2) {
    config.sensorType = 0; // Default DHT11
  }
  if (config.relay1 != 0 && config.relay1 != 1) config.relay1 = false;
  if (config.relay2 != 0 && config.relay2 != 1) config.relay2 = false;
  
  relay1 = config.relay1;
  relay2 = config.relay2;
  
  digitalWrite(RELAY1_PIN, relay1 ? HIGH : LOW);
  digitalWrite(RELAY2_PIN, relay2 ? HIGH : LOW);
  
  Serial.print("Loaded sensor: "); Serial.println(config.sensorType);
}

// HTML & CSS Template disimpan di Flash Memory (PROGMEM)
const char html_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>IoT Smart Relay Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- Google Fonts & FontAwesome -->
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700;800&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <!-- Chart.js -->
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  
  <style>
    :root {
      --bg-color: #0b0f19;
      --card-bg: rgba(20, 26, 42, 0.65);
      --card-border: rgba(255, 255, 255, 0.08);
      --text-main: #f3f4f6;
      --text-sub: #9ca3af;
      --primary: #00f2fe;
      --secondary: #4facfe;
      --success: #10b981;
      --success-glow: rgba(16, 185, 129, 0.2);
      --danger: #ef4444;
      --danger-glow: rgba(239, 68, 68, 0.2);
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
      justify-content: flex-start;
      padding: 20px;
      overflow-x: hidden;
      position: relative;
    }

    /* Ambient Background Glows */
    body::before, body::after {
      content: '';
      position: absolute;
      width: 300px;
      height: 300px;
      border-radius: 50%;
      filter: blur(120px);
      z-index: -1;
      opacity: 0.35;
    }
    body::before {
      background: var(--primary);
      top: 15%;
      left: -10%;
    }
    body::after {
      background: var(--secondary);
      bottom: 15%;
      right: -10%;
    }

    .container {
      width: 100%;
      max-width: 500px;
      display: flex;
      flex-direction: column;
      gap: 20px;
      z-index: 1;
    }

    /* Header */
    header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 10px 0;
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
      color: #0b0f19;
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

    .status-badge {
      display: flex;
      align-items: center;
      gap: 6px;
      font-size: 10px;
      font-weight: 700;
      color: var(--success);
      background: rgba(16, 185, 129, 0.08);
      border: 1px solid rgba(16, 185, 129, 0.2);
      padding: 6px 12px;
      border-radius: 20px;
    }

    .pulse-dot {
      width: 6px;
      height: 6px;
      background-color: var(--success);
      border-radius: 50%;
      box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7);
      animation: pulse 1.5s infinite;
    }

    @keyframes pulse {
      0% {
        transform: scale(0.95);
        box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7);
      }
      70% {
        transform: scale(1);
        box-shadow: 0 0 0 6px rgba(16, 185, 129, 0);
      }
      100% {
        transform: scale(0.95);
        box-shadow: 0 0 0 0 rgba(16, 185, 129, 0);
      }
    }

    /* Grid Layout */
    .grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 16px;
    }

    /* Card */
    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 20px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
      display: flex;
      flex-direction: column;
      gap: 12px;
      transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    }

    .card.active {
      border-color: rgba(0, 242, 254, 0.3);
      box-shadow: 0 8px 32px rgba(0, 242, 254, 0.08);
    }

    .card-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .card-title {
      font-size: 13px;
      font-weight: 700;
      color: var(--text-main);
      display: flex;
      align-items: center;
      gap: 6px;
    }

    /* Toggle Switch */
    .switch {
      position: relative;
      display: inline-block;
      width: 40px;
      height: 22px;
    }

    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #374151;
      transition: .3s;
      border-radius: 22px;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 14px;
      width: 14px;
      left: 4px;
      bottom: 4px;
      background-color: #9ca3af;
      transition: .3s;
      border-radius: 50%;
    }

    input:checked + .slider {
      background: linear-gradient(135deg, var(--secondary), var(--primary));
    }

    input:checked + .slider:before {
      transform: translateX(18px);
      background-color: #0b0f19;
    }

    .state-label {
      font-size: 24px;
      font-weight: 800;
      color: var(--text-sub);
      margin-top: 4px;
    }

    .state-label.active {
      color: #fff;
      text-shadow: 0 0 10px rgba(0, 242, 254, 0.3);
    }

    /* Temp Widget */
    .temp-info {
      display: flex;
      flex-direction: column;
      gap: 4px;
    }

    .temp-value {
      font-size: 34px;
      font-weight: 800;
      color: #fff;
      display: flex;
      align-items: baseline;
      line-height: 1;
    }

    .temp-unit {
      font-size: 18px;
      color: var(--text-sub);
      font-weight: 500;
      margin-left: 2px;
    }

    /* Chart Card */
    .chart-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 20px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
    }

    .chart-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 15px;
    }

    .chart-title {
      font-size: 13px;
      font-weight: 700;
      color: var(--text-main);
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .chart-title i {
      color: #8b5cf6;
    }

    /* Footer */
    footer {
      text-align: center;
      padding: 15px 0;
      color: var(--text-sub);
      font-size: 12px;
      width: 100%;
      border-top: 1px solid rgba(255, 255, 255, 0.05);
      margin-top: auto;
    }

    footer a {
      color: var(--primary);
      text-decoration: none;
      font-weight: 550;
    }

    footer a:hover {
      text-decoration: underline;
    }
  </style>
</head>
<body>

<div class="container">
  <!-- Header -->
  <header>
    <div class="logo-area">
      <div class="logo-icon">
        <i class="fa-solid fa-microchip"></i>
      </div>
      <div class="logo-title">
        <h1>IoT Smart Relay</h1>
        <p>ESP8266 Integrated Module</p>
      </div>
    </div>
    <div class="status-badge">
      <div class="pulse-dot"></div>
      <span>CONNECTED</span>
    </div>
  </header>

  <!-- Relay Grid -->
  <div class="grid">
    <!-- Card Relay 1 -->
    <div class="card" id="card-r1">
      <div class="card-header">
        <span class="card-title"><i class="fa-solid fa-lightbulb"></i> Relay 1</span>
        <label class="switch">
          <input type="checkbox" id="btn-r1" onchange="toggleRelay(1, this.checked)">
          <span class="slider"></span>
        </label>
      </div>
      <div class="state-label" id="txt-r1">OFF</div>
    </div>

    <!-- Card Relay 2 -->
    <div class="card" id="card-r2">
      <div class="card-header">
        <span class="card-title"><i class="fa-solid fa-power-off"></i> Relay 2</span>
        <label class="switch">
          <input type="checkbox" id="btn-r2" onchange="toggleRelay(2, this.checked)">
          <span class="slider"></span>
        </label>
      </div>
      <div class="state-label" id="txt-r2">OFF</div>
    </div>
  </div>

  <!-- Temperature & Humidity Display Cards -->
  <div class="grid">
    <!-- Suhu Card -->
    <div class="card">
      <div class="temp-info">
        <span class="card-title" style="color: var(--text-sub);"><i class="fa-solid fa-thermometer-half" style="color: #ef4444;"></i> <span id="lblSensor">Sensor</span></span>
        <span style="font-size: 11px; color: var(--text-sub);">Suhu</span>
      </div>
      <div style="display: flex; align-items: center; justify-content: space-between; margin-top: 10px;">
        <div class="temp-value">
          <span id="temp">0.0</span>
          <span class="temp-unit">°C</span>
        </div>
        <div style="background: rgba(239, 68, 68, 0.08); width: 48px; height: 48px; border-radius: 50%; display: flex; align-items: center; justify-content: center; border: 1px solid rgba(239, 68, 68, 0.15);">
          <i class="fa-solid fa-temperature-high" style="font-size: 20px; color: #ef4444;"></i>
        </div>
      </div>
    </div>

    <!-- Kelembaban Card -->
    <div class="card" id="cardHumi">
      <div class="temp-info">
        <span class="card-title" style="color: var(--text-sub);"><i class="fa-solid fa-droplet" style="color: #3b82f6;"></i> Kelembaban</span>
        <span style="font-size: 11px; color: var(--text-sub);">Udara</span>
      </div>
      <div style="display: flex; align-items: center; justify-content: space-between; margin-top: 10px;">
        <div class="temp-value">
          <span id="humi">0</span>
          <span class="temp-unit">%</span>
        </div>
        <div style="background: rgba(59, 130, 246, 0.08); width: 48px; height: 48px; border-radius: 50%; display: flex; align-items: center; justify-content: center; border: 1px solid rgba(59, 130, 246, 0.15);">
          <i class="fa-solid fa-droplet" style="font-size: 20px; color: #3b82f6;"></i>
        </div>
      </div>
    </div>
  </div>

  <!-- Sensor Selector Card -->
  <div class="grid">
    <div class="card" style="grid-column: span 2; display: flex; flex-direction: row; justify-content: space-between; align-items: center; padding: 12px 20px;">
      <span class="card-title" style="font-size: 13px; color: var(--text-sub); display: flex; align-items: center; gap: 8px;"><i class="fa-solid fa-gears" style="color: var(--primary);"></i> Jenis Sensor (GPIO 12)</span>
      <select id="sensorTypeSelect" onchange="changeSensor(this.value)" style="background: rgba(0, 0, 0, 0.3); border: 1px solid rgba(255, 255, 255, 0.08); color: #fff; border-radius: 8px; padding: 6px 12px; outline: none; font-size: 12px; font-weight: 600; cursor: pointer; color-scheme: dark;">
        <option value="0">DHT11</option>
        <option value="1">DHT22</option>
        <option value="2">DS18B20</option>
      </select>
    </div>
  </div>

  <!-- Chart Card -->
  <div class="chart-card">
    <div class="chart-header">
      <span class="chart-title"><i class="fa-solid fa-chart-line"></i> Tren Suhu (°C)</span>
    </div>
    <canvas id="chart" height="130"></canvas>
  </div>

  <!-- Footer -->
  <footer>
    &copy; 2026 - <a href="https://duwiarsana.com" target="_blank">Anak Agung Duwi Arsana</a> | IoT Relay Module
  </footer>
</div>

<script>
  let tempData = [], timeLabels = [];
  const tempSpan = document.getElementById("temp");
  const humiSpan = document.getElementById("humi");
  const cardHumi = document.getElementById("cardHumi");
  const r1Switch = document.getElementById("btn-r1");
  const r2Switch = document.getElementById("btn-r2");
  const r1Text = document.getElementById("txt-r1");
  const r2Text = document.getElementById("txt-r2");
  const r1Card = document.getElementById("card-r1");
  const r2Card = document.getElementById("card-r2");

  const ctx = document.getElementById("chart").getContext("2d");
  
  const chart = new Chart(ctx, {
    type: "line",
    data: {
      labels: timeLabels,
      datasets: [{
        label: "Suhu (°C)",
        data: tempData,
        borderColor: "#00f2fe",
        borderWidth: 2,
        backgroundColor: "rgba(0, 242, 254, 0.06)",
        fill: true,
        tension: 0.4,
        pointBackgroundColor: "#4facfe",
        pointBorderColor: "#0b0f19",
        pointBorderWidth: 1.5,
        pointRadius: 4,
        pointHoverRadius: 6
      }]
    },
    options: {
      responsive: true,
      plugins: { legend: { display: false } },
      scales: {
        x: { grid: { color: "rgba(255, 255, 255, 0.03)" }, ticks: { color: "#9ca3af", font: { size: 10 } } },
        y: { grid: { color: "rgba(255, 255, 255, 0.03)" }, ticks: { color: "#9ca3af", font: { size: 10 } } }
      }
    }
  });

  function updateRelayUI(id, isON) {
    const sw = (id === 1) ? r1Switch : r2Switch;
    const txt = (id === 1) ? r1Text : r2Text;
    const card = (id === 1) ? r1Card : r2Card;

    sw.checked = isON;
    if (isON) {
      txt.innerText = "ON";
      txt.className = "state-label active";
      card.classList.add("active");
    } else {
      txt.innerText = "OFF";
      txt.className = "state-label";
      card.classList.remove("active");
    }
  }

  function fetchData() {
    fetch('/data').then(res => res.json()).then(data => {
      tempSpan.innerText = data.temp.toFixed(1);
      
      // Update sensor label
      const sensorNames = ["Sensor DHT11", "Sensor DHT22", "Sensor DS18B20"];
      document.getElementById("lblSensor").innerText = sensorNames[data.sensor];
      document.getElementById("sensorTypeSelect").value = data.sensor;

      // Handle kelembaban card opacity if DS18B20
      if (data.sensor === 2) {
        cardHumi.style.opacity = "0.35";
        humiSpan.innerText = "--";
      } else {
        cardHumi.style.opacity = "1";
        humiSpan.innerText = Math.round(data.humi);
      }

      updateRelayUI(1, data.r1);
      updateRelayUI(2, data.r2);

      const now = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
      timeLabels.push(now);
      tempData.push(data.temp);
      
      if (tempData.length > 10) {
        timeLabels.shift();
        tempData.shift();
      }
      chart.update();
    }).catch(err => console.error("Error fetching data:", err));
  }

  function toggleRelay(id, isChecked) {
    const state = isChecked ? 1 : 0;
    fetch('/set?relay=' + id + '&state=' + state)
      .then(res => {
        if(res.ok) {
          updateRelayUI(id, isChecked);
        }
      })
      .catch(err => {
        console.error("Error setting relay:", err);
        if(id === 1) r1Switch.checked = !isChecked;
        if(id === 2) r2Switch.checked = !isChecked;
      });
  }

  function changeSensor(type) {
    fetch('/setsensor?type=' + type)
      .then(res => {
        if(res.ok) {
          fetchData();
        }
      })
      .catch(err => console.error("Error setting sensor:", err));
  }

  setInterval(fetchData, 2000);
  window.onload = fetchData;
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", html_template);
}

void handleData() {
  String json = "{";
  json += "\"temp\":" + String(suhu) + ",";
  json += "\"humi\":" + String(kelembaban) + ",";
  json += "\"r1\":" + String(relay1 ? "true" : "false") + ",";
  json += "\"r2\":" + String(relay2 ? "true" : "false") + ",";
  json += "\"sensor\":" + String(config.sensorType);
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetRelay() {
  if (server.hasArg("relay") && server.hasArg("state")) {
    int id = server.arg("relay").toInt();
    int state = server.arg("state").toInt();
    if (id == 1) { relay1 = (state == 1); digitalWrite(RELAY1_PIN, relay1 ? HIGH : LOW); }
    if (id == 2) { relay2 = (state == 1); digitalWrite(RELAY2_PIN, relay2 ? HIGH : LOW); }
    saveRelayState();
  }
  server.send(200, "text/plain", "OK");
}

void handleSetSensor() {
  if (server.hasArg("type")) {
    config.sensorType = server.arg("type").toInt();
    saveRelayState();
    initSensor();
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  
  loadConfig();
  initSensor();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/set", handleSetRelay);
  server.on("/setsensor", handleSetSensor);
  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - lastDHTRead > 2000) {
    lastDHTRead = millis();
    if (config.sensorType == 0 || config.sensorType == 1) {
      if (dht != nullptr) {
        float t = dht->readTemperature();
        float h = dht->readHumidity();
        if (!isnan(t)) suhu = t;
        if (!isnan(h)) kelembaban = h;
      }
    } else if (config.sensorType == 2) {
      if (sensors != nullptr) {
        sensors->requestTemperatures();
        float t = sensors->getTempCByIndex(0);
        if (t != DEVICE_DISCONNECTED_C) suhu = t;
        kelembaban = 0;
      }
    }
  }
}
