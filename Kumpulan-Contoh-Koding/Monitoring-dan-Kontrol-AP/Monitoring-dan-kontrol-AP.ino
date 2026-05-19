// Copyright (c) 2025 Anak Agung Duwi Arsana
// Website: https://duwiarsana.com

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <DHT.h>
#include <EEPROM.h>

#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define RELAY1_PIN 4
#define RELAY2_PIN 5

// Pengaturan Access Point (AP)
const char* ap_ssid = "IoT-Smart-Relay";
const char* ap_password = ""; // Kosongkan jika ingin open network tanpa password

ESP8266WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

bool relay1 = false;
bool relay2 = false;
float suhu = 0;

unsigned long lastDHTRead = 0;

void saveRelayState() {
  EEPROM.write(0, relay1);
  EEPROM.write(1, relay2);
  EEPROM.commit();
}

void loadRelayState() {
  relay1 = (EEPROM.read(0) == 1);
  relay2 = (EEPROM.read(1) == 1);
  digitalWrite(RELAY1_PIN, relay1 ? HIGH : LOW);
  digitalWrite(RELAY2_PIN, relay2 ? HIGH : LOW);
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>IoT Smart Relay Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <!-- Google Fonts & FontAwesome -->
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <!-- Chart.js CDN -->
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

    /* Background Ambient Glows */
    body::before {
      content: '';
      position: absolute;
      width: 300px;
      height: 300px;
      background: var(--primary);
      border-radius: 50%;
      filter: blur(120px);
      z-index: -1;
      opacity: 0.25;
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
      opacity: 0.25;
      bottom: 10%;
      right: 10%;
    }

    .container {
      width: 100%;
      max-width: 480px;
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
      width: 46px;
      height: 46px;
      border-radius: 12px;
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 4px 20px rgba(0, 242, 254, 0.35);
    }

    .logo-icon i {
      color: #0b0f19;
      font-size: 20px;
    }

    .logo-title h1 {
      font-size: 18px;
      font-weight: 700;
      letter-spacing: -0.5px;
      background: linear-gradient(to right, #ffffff, #c7d2fe);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }

    .logo-title p {
      font-size: 11px;
      color: var(--text-sub);
    }

    .status-badge {
      background: rgba(16, 185, 129, 0.1);
      border: 1px solid rgba(16, 185, 129, 0.25);
      padding: 6px 12px;
      border-radius: 20px;
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .status-badge span {
      font-size: 10px;
      font-weight: 700;
      color: var(--success);
      letter-spacing: 0.5px;
    }

    .pulse-dot {
      width: 6px;
      height: 6px;
      background-color: var(--success);
      border-radius: 50%;
      box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7);
      animation: pulse 1.6s infinite;
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

    /* Cards Grid */
    .grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 16px;
      margin-bottom: 20px;
    }

    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      -webkit-backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 20px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.35);
      transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
      position: relative;
      overflow: hidden;
    }

    .card.active {
      border-color: rgba(0, 242, 254, 0.4);
      box-shadow: 0 8px 32px rgba(0, 242, 254, 0.15);
    }

    .card.active::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 4px;
      background: linear-gradient(to right, var(--secondary), var(--primary));
    }

    .card-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 20px;
    }

    .card-title {
      font-size: 13px;
      font-weight: 600;
      color: var(--text-main);
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .card-title i {
      font-size: 16px;
      color: var(--text-sub);
      transition: color 0.3s;
    }

    .card.active .card-title i {
      color: var(--primary);
    }

    /* Switch Style */
    .switch {
      position: relative;
      display: inline-block;
      width: 44px;
      height: 24px;
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
      border-radius: 24px;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 16px;
      width: 16px;
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
      transform: translateX(20px);
      background-color: #0b0f19;
    }

    .state-label {
      font-size: 26px;
      font-weight: 800;
      color: var(--text-sub);
      letter-spacing: -0.5px;
      transition: color 0.3s;
    }

    .state-label.active {
      color: #fff;
      text-shadow: 0 0 15px rgba(255, 255, 255, 0.4);
    }

    /* Temp Widget styles */
    .temp-info {
      display: flex;
      flex-direction: column;
      gap: 4px;
      margin-bottom: 15px;
    }

    .temp-value {
      font-size: 34px;
      font-weight: 800;
      letter-spacing: -1px;
      color: #fff;
      display: flex;
      align-items: baseline;
    }

    .temp-unit {
      font-size: 18px;
      color: var(--text-sub);
      margin-left: 2px;
      font-weight: 500;
    }

    /* Chart Card */
    .chart-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      -webkit-backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 20px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.35);
      margin-bottom: 24px;
    }

    .chart-header {
      margin-bottom: 15px;
    }

    .chart-title {
      font-size: 13px;
      font-weight: 600;
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .chart-title i {
      color: var(--primary);
    }

    /* Footer */
    footer {
      text-align: center;
      font-size: 11px;
      color: var(--text-sub);
      padding: 10px 0;
      border-top: 1px solid rgba(255, 255, 255, 0.05);
    }

    footer a {
      color: var(--primary);
      text-decoration: none;
      font-weight: 500;
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
      <span>AP PORTAL</span>
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

  <!-- Temperature Display Card -->
  <div class="grid">
    <div class="card" style="grid-column: span 2;">
      <div class="temp-info">
        <span class="card-title" style="color: var(--text-sub);"><i class="fa-solid fa-thermometer-half" style="color: #ef4444;"></i> Sensor DHT11</span>
        <span style="font-size: 12px; color: var(--text-sub);">Real-time Telemetry</span>
      </div>
      <div style="display: flex; align-items: center; justify-content: space-between;">
        <div>
          <p style="font-size: 12px; color: var(--text-sub); margin-bottom: 2px;">SUHU RUANGAN</p>
          <div class="temp-value">
            <span id="temp">0</span>
            <span class="temp-unit">°C</span>
          </div>
        </div>
        <div style="background: rgba(239, 68, 68, 0.08); width: 64px; height: 64px; border-radius: 50%; display: flex; align-items: center; justify-content: center; border: 1px solid rgba(239, 68, 68, 0.15);">
          <i class="fa-solid fa-fire-flame-simple" style="font-size: 28px; color: #ef4444;"></i>
        </div>
      </div>
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
  const r1Switch = document.getElementById("btn-r1");
  const r2Switch = document.getElementById("btn-r2");
  const r1Text = document.getElementById("txt-r1");
  const r2Text = document.getElementById("txt-r2");
  const r1Card = document.getElementById("card-r1");
  const r2Card = document.getElementById("card-r2");

  const ctx = document.getElementById("chart").getContext("2d");
  
  // Custom styled Chart.js configuration
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
      plugins: {
        legend: { display: false }
      },
      scales: {
        x: {
          grid: { display: false },
          ticks: { color: "#6b7280", font: { size: 10 } }
        },
        y: {
          grid: { color: "rgba(255,255,255,0.03)" },
          ticks: { color: "#6b7280", font: { size: 10 } }
        }
      }
    }
  });

  function updateRelayUI(id, isActive) {
    const sw = id === 1 ? r1Switch : r2Switch;
    const txt = id === 1 ? r1Text : r2Text;
    const card = id === 1 ? r1Card : r2Card;

    sw.checked = isActive;
    if (isActive) {
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
      tempSpan.innerText = data.temp;
      
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
        // Revert switch if failed
        if(id === 1) r1Switch.checked = !isChecked;
        if(id === 2) r2Switch.checked = !isChecked;
      });
  }

  setInterval(fetchData, 2000);
  window.onload = fetchData;
</script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"temp\":" + String(suhu) + ",";
  json += "\"r1\":" + String(relay1 ? "true" : "false") + ",";
  json += "\"r2\":" + String(relay2 ? "true" : "false");
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

void handleNotFound() {
  // Redirect ke root (Captive Portal)
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  loadRelayState();

  dht.begin();

  // Memulai mode Access Point (AP)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  Serial.println("\nAccess Point Berhasil Dibuat!");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Memulai DNS Server untuk Captive Portal (mengarahkan semua domain ke IP ESP8266)
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/set", handleSetRelay);
  server.onNotFound(handleNotFound); // Menangani request redirect captive portal
  server.begin();
}

void loop() {
  dnsServer.processNextRequest(); // Proses request DNS captive portal
  server.handleClient();
  
  if (millis() - lastDHTRead > 2000) {
    lastDHTRead = millis();
    float t = dht.readTemperature();
    if (!isnan(t)) suhu = t;
  }
}
