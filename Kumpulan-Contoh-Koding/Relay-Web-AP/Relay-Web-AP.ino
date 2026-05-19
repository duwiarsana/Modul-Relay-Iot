// Web Relay Control AP ESP8266 (Captive Portal)
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

// Definisi pin relay
#define RELAY1 4
#define RELAY2 5

// Pengaturan Access Point (AP)
const char* ap_ssid = "IoT-WiFi-Relay";
const char* ap_password = ""; // Kosongkan jika ingin open network tanpa password

AsyncWebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

void setup() {
  Serial.begin(115200);

  // Set pin relay sebagai output
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // Memulai mode Access Point (AP)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  Serial.println("\nAccess Point Berhasil Dibuat!");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Memulai DNS Server untuk Captive Portal
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Halaman utama (kontrol relay via browser)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>IoT Relay Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <style>
    :root {
      --bg-color: #0d0f17;
      --card-bg: rgba(22, 28, 45, 0.6);
      --card-border: rgba(255, 255, 255, 0.08);
      --text-main: #f3f4f6;
      --text-sub: #9ca3af;
      --primary: #00f2fe;
      --secondary: #4facfe;
      --success: #10b981;
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
      overflow: hidden;
    }
    body::before {
      content: '';
      position: absolute;
      width: 250px;
      height: 250px;
      background: var(--primary);
      border-radius: 50%;
      filter: blur(100px);
      z-index: -1;
      opacity: 0.3;
      top: 20%;
      left: 20%;
    }
    body::after {
      content: '';
      position: absolute;
      width: 250px;
      height: 250px;
      background: #8b5cf6;
      border-radius: 50%;
      filter: blur(100px);
      z-index: -1;
      opacity: 0.3;
      bottom: 20%;
      right: 20%;
    }
    .container { width: 100%; max-width: 420px; z-index: 1; }
    header { text-align: center; margin-bottom: 30px; }
    .logo-icon {
      background: linear-gradient(135deg, var(--secondary), var(--primary));
      width: 50px;
      height: 50px;
      border-radius: 14px;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 12px;
      box-shadow: 0 4px 15px rgba(0, 242, 254, 0.3);
    }
    .logo-icon i { color: #0d0f17; font-size: 24px; }
    h1 { font-size: 22px; font-weight: 700; background: linear-gradient(to right, #fff, #c7d2fe); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }
    p.subtitle { font-size: 12px; color: var(--text-sub); margin-top: 4px; }
    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      backdrop-filter: blur(12px);
      -webkit-backdrop-filter: blur(12px);
      border-radius: 20px;
      padding: 20px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
      margin-bottom: 20px;
    }
    .relay-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 15px 0;
    }
    .relay-row:not(:last-child) { border-bottom: 1px solid rgba(255, 255, 255, 0.05); }
    .relay-info { display: flex; align-items: center; gap: 15px; }
    .relay-icon {
      width: 40px;
      height: 40px;
      border-radius: 10px;
      background: rgba(255, 255, 255, 0.05);
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 18px;
      color: var(--text-sub);
      transition: all 0.3s ease;
    }
    .active .relay-icon {
      background: rgba(0, 242, 254, 0.1);
      color: var(--primary);
      text-shadow: 0 0 8px rgba(0, 242, 254, 0.5);
    }
    .relay-details h3 { font-size: 15px; font-weight: 600; }
    .relay-details span { font-size: 11px; color: var(--text-sub); font-weight: 500; }
    .active .relay-details span { color: var(--primary); }
    
    /* Switch */
    .switch { position: relative; display: inline-block; width: 50px; height: 26px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider {
      position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0;
      background-color: #374151; transition: .3s; border-radius: 34px;
      border: 1px solid rgba(255,255,255,0.03);
    }
    .slider:before {
      position: absolute; content: ""; height: 18px; width: 18px; left: 3px; bottom: 3px;
      background-color: #9ca3af; transition: .3s; border-radius: 50%;
    }
    input:checked + .slider { background: linear-gradient(135deg, var(--secondary), var(--primary)); }
    input:checked + .slider:before { transform: translateX(24px); background-color: #0d0f17; }
    
    footer { text-align: center; font-size: 11px; color: var(--text-sub); margin-top: 10px; }
    footer a { color: var(--primary); text-decoration: none; font-weight: 500; }
  </style>
</head>
<body>
<div class="container">
  <header>
    <div class="logo-icon"><i class="fa-solid fa-toggle-on"></i></div>
    <h1>WiFi Relay Controller</h1>
    <p class="subtitle">ESP8266 2-Channel Switch (AP)</p>
  </header>
  <div class="card">
    <!-- Relay 1 Row -->
    <div class="relay-row" id="row-r1">
      <div class="relay-info">
        <div class="relay-icon"><i class="fa-solid fa-lightbulb"></i></div>
        <div class="relay-details">
          <h3>Relay 1</h3>
          <span id="status-r1">OFF</span>
        </div>
      </div>
      <label class="switch">
        <input type="checkbox" id="btn-r1" onchange="toggleRelay(1, this.checked)">
        <span class="slider"></span>
      </label>
    </div>
    <!-- Relay 2 Row -->
    <div class="relay-row" id="row-r2">
      <div class="relay-info">
        <div class="relay-icon"><i class="fa-solid fa-power-off"></i></div>
        <div class="relay-details">
          <h3>Relay 2</h3>
          <span id="status-r2">OFF</span>
        </div>
      </div>
      <label class="switch">
        <input type="checkbox" id="btn-r2" onchange="toggleRelay(2, this.checked)">
        <span class="slider"></span>
      </label>
    </div>
  </div>
  <footer>
    Dibuat oleh <a href="https://duwiarsana.com" target="_blank">Duwi Arsana</a>
  </footer>
</div>
<script>
  const r1Row = document.getElementById("row-r1");
  const r2Row = document.getElementById("row-r2");
  const r1Switch = document.getElementById("btn-r1");
  const r2Switch = document.getElementById("btn-r2");
  const r1Status = document.getElementById("status-r1");
  const r2Status = document.getElementById("status-r2");

  function updateUI(id, active) {
    const row = id === 1 ? r1Row : r2Row;
    const sw = id === 1 ? r1Switch : r2Switch;
    const label = id === 1 ? r1Status : r2Status;
    
    sw.checked = active;
    if (active) {
      row.classList.add("active");
      label.innerText = "ACTIVE (ON)";
    } else {
      row.classList.remove("active");
      label.innerText = "INACTIVE (OFF)";
    }
  }

  function checkStatus() {
    fetch('/status').then(res => res.json()).then(data => {
      updateUI(1, data.r1);
      updateUI(2, data.r2);
    }).catch(err => console.error(err));
  }

  function toggleRelay(id, active) {
    const action = active ? 'on' : 'off';
    fetch('/relay' + id + '/' + action)
      .then(res => {
        if (res.ok) {
          updateUI(id, active);
        } else {
          if (id === 1) r1Switch.checked = !active;
          if (id === 2) r2Switch.checked = !active;
        }
      })
      .catch(err => {
        console.error(err);
        if (id === 1) r1Switch.checked = !active;
        if (id === 2) r2Switch.checked = !active;
      });
  }

  window.onload = checkStatus;
</script>
</body>
</html>
)rawliteral";
    request->send(200, "text/html", html);
  });

  // Endpoint status relay untuk sinkronisasi web interface
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json =
        "{\"r1\":" + String(digitalRead(RELAY1) == HIGH ? "true" : "false") +
        ",\"r2\":" + String(digitalRead(RELAY2) == HIGH ? "true" : "false") +
        "}";
    request->send(200, "application/json", json);
  });

  // Endpoint untuk kontrol Relay 1 ON
  server.on("/relay1/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(RELAY1, HIGH);
    request->send(200, "text/plain", "Relay 1 ON");
  });

  // Endpoint untuk kontrol Relay 1 OFF
  server.on("/relay1/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(RELAY1, LOW);
    request->send(200, "text/plain", "Relay 1 OFF");
  });

  // Endpoint untuk kontrol Relay 2 ON
  server.on("/relay2/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(RELAY2, HIGH);
    request->send(200, "text/plain", "Relay 2 ON");
  });

  // Endpoint untuk kontrol Relay 2 OFF
  server.on("/relay2/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(RELAY2, LOW);
    request->send(200, "text/plain", "Relay 2 OFF");
  });

  // Redirect all unknown requests to the AP root for Captive Portal functionality
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("http://192.168.4.1/");
  });

  // Mulai server
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
