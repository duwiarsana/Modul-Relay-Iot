// Web Relay Control ESP8266
// Dibuat oleh Duwi Arsana - Anak Agung Duwi Arsana
// Website: https://duwiarsana.com
// YouTube: https://youtube.com/@AnakAgungDuwiArsana

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

// Ganti dengan nama dan password WiFi kamu
const char* ssid = "Pengki";
const char* password = "tehpucuk";

// Definisi pin relay
#define RELAY1 4
#define RELAY2 5

// Objek web server pada port 80
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Set pin relay sebagai output
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // Koneksi ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Tampilkan IP address ESP

  // Halaman utama (kontrol relay via browser)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{font-family:sans-serif; text-align:center;} button{padding:10px; margin:5px; font-size:18px;}</style></head><body>";
    html += "<h2>Relay Control</h2>";
    html += "<button onclick=\"fetch('/relay1/on')\">Relay 1 ON</button>";
    html += "<button onclick=\"fetch('/relay1/off')\">Relay 1 OFF</button>";
    html += "<button onclick=\"fetch('/relay2/on')\">Relay 2 ON</button>";
    html += "<button onclick=\"fetch('/relay2/off')\">Relay 2 OFF</button>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Endpoint untuk kontrol Relay 1 ON
  server.on("/relay1/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY1, HIGH);
    request->send(200, "text/plain", "Relay 1 ON");
  });

  // Endpoint untuk kontrol Relay 1 OFF
  server.on("/relay1/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY1, LOW);
    request->send(200, "text/plain", "Relay 1 OFF");
  });

  // Endpoint untuk kontrol Relay 2 ON
  server.on("/relay2/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY2, HIGH);
    request->send(200, "text/plain", "Relay 2 ON");
  });

  // Endpoint untuk kontrol Relay 2 OFF
  server.on("/relay2/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY2, LOW);
    request->send(200, "text/plain", "Relay 2 OFF");
  });

  // Mulai server
  server.begin();
}

void loop() {
  // Tidak ada proses di loop karena semua handled oleh web server async
} 
