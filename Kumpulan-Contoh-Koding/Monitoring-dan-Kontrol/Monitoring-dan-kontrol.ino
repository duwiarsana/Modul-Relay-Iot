// Copyright (c) 2025 Anak Agung Duwi Arsana
// Website: https://duwiarsana.com

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <EEPROM.h>

#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define RELAY1_PIN 4
#define RELAY2_PIN 5

const char* ssid = "SSID"; //ganti pakai nama hotspot yang ada
const char* password = "PASSWORD"; //ganti juga password yang sesuai

ESP8266WebServer server(80);

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
  relay1 = EEPROM.read(0);
  relay2 = EEPROM.read(1);
  digitalWrite(RELAY1_PIN, relay1 ? LOW : HIGH);
  digitalWrite(RELAY2_PIN, relay2 ? LOW : HIGH);
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>IoT Dashboard - AJAX</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <link href="https://fonts.googleapis.com/css2?family=Poppins&display=swap" rel="stylesheet">
  <style>
    body { background: #121212; color: #fff; font-family: 'Poppins', sans-serif; text-align: center; padding: 20px; }
    h2 { color: #00ffe1; }
    .section { background: #1f1f1f; padding: 20px; margin: 20px auto; border-radius: 12px; max-width: 500px; }
    .btn { padding: 10px 20px; font-size: 16px; border: none; border-radius: 6px; margin: 5px; cursor: pointer; transition: 0.2s; min-width: 100px; }
    .btn:hover { transform: scale(1.05); }
    .btn-on { background-color: #00e676; color: black; }
    .btn-off { background-color: #ff5252; color: black; }
    .value { font-size: 24px; font-weight: bold; }
    footer { margin-top: 40px; font-size: 12px; color: #888; }
  </style>
</head>
<body>
  <h2>Kontrol Relay</h2>
  <div class="section">
    <p>Relay 1: <span class="value" id="r1">OFF</span></p>
    <button class="btn btn-on" onclick="setRelay(1, 0)">ON</button>
    <button class="btn btn-off" onclick="setRelay(1, 1)">OFF</button>
    <p>Relay 2: <span class="value" id="r2">OFF</span></p>
    <button class="btn btn-on" onclick="setRelay(2, 0)">ON</button>
    <button class="btn btn-off" onclick="setRelay(2, 1)">OFF</button>
  </div>

  <h2>Monitoring Suhu</h2>
  <div class="section">
    <p>Suhu Saat Ini: <span class="value" id="temp">0</span> °C</p>
    <canvas id="chart" height="140"></canvas>
  </div>

  <footer>
    &copy; 2025 - Anak Agung Duwi Arsana | duwiarsana.com
  </footer>

<script>
  let tempData = [], timeLabels = [];
  const tempSpan = document.getElementById("temp");
  const r1Span = document.getElementById("r1");
  const r2Span = document.getElementById("r2");
  const ctx = document.getElementById("chart").getContext("2d");
  const chart = new Chart(ctx, {
    type: "line",
    data: {
      labels: timeLabels,
      datasets: [{
        label: "Suhu (°C)",
        data: tempData,
        borderColor: "#00ffe1",
        backgroundColor: "rgba(0,255,225,0.1)",
        fill: true,
        tension: 0.3
      }]
    },
    options: { scales: { x: { display: false }, y: { beginAtZero: true } } }
  });

  function fetchData() {
    fetch('/data').then(res => res.json()).then(data => {
      tempSpan.innerText = data.temp;
      r1Span.innerText = data.r1 ? "OFF" : "ON";
      r2Span.innerText = data.r2 ? "OFF" : "ON";

      const now = new Date().toLocaleTimeString();
      timeLabels.push(now);
      tempData.push(data.temp);
      if (tempData.length > 20) {
        timeLabels.shift(); tempData.shift();
      }
      chart.update();
    });
  }

  function setRelay(id, state) {
    fetch('/set?relay=' + id + '&state=' + state).then(fetchData);
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
    if (id == 1) { relay1 = state; digitalWrite(RELAY1_PIN, relay1 ? LOW : HIGH); }
    if (id == 2) { relay2 = state; digitalWrite(RELAY2_PIN, relay2 ? LOW : HIGH); }
    saveRelayState();
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  loadRelayState();

  dht.begin();

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/set", handleSetRelay);
  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - lastDHTRead > 2000) {
    lastDHTRead = millis();
    float t = dht.readTemperature();
    if (!isnan(t)) suhu = t;
  }
}
