# 📊 Monitoring & Kontrol Relay - Mode Access Point (Captive Portal)

Contoh koding ini mengaktifkan modul ESP8266 sebagai **Access Point (AP)** mandiri. Modul akan memancarkan jaringan WiFi sendiri bernama `IoT-Smart-Relay`, memungkinkan Anda untuk memonitor grafik suhu/kelembaban secara real-time dan mengontrol 2-channel relay secara offline tanpa membutuhkan router eksternal.

Proyek ini dilengkapi dengan fitur **Captive Portal** sehingga ketika smartphone terhubung ke WiFi modul, halaman kontrol web akan otomatis muncul (seperti login WiFi di hotel/kafe).

---

## 🛠️ Persiapan Library

Pastikan library berikut sudah terinstal di Arduino IDE Anda:

1. **DHT sensor library** (oleh Adafruit)
2. **Adafruit Unified Sensor** (dependency untuk DHT)
3. **OneWire** (oleh Paul Stoffregen)
4. **DallasTemperature** (oleh Miles Burton) - Jika menggunakan sensor suhu DS18B20.

---

## 🧩 Fitur & Konfigurasi Pinout

* **Relay 1**: Disambungkan ke **GPIO 4 (D2)**
* **Relay 2**: Disambungkan ke **GPIO 5 (D1)**
* **Sensor Temp/Hum**: Disambungkan ke **GPIO 12 (D6)**
  * Mendukung 3 jenis sensor yang dapat dipilih langsung melalui drop-down menu di web: **DHT11**, **DHT22**, atau **DS18B20**.
* **Nama WiFi (SSID) Default**: `IoT-Smart-Relay` (tanpa password/open network).
* **Alamat IP Default**: `192.168.4.1`

---

## 💻 Cara Menggunakan

1. Upload program `Monitoring-dan-kontrol-AP.ino` ke modul ESP8266 Anda.
2. Nyalakan modul. Cari sinyal WiFi bernama **`IoT-Smart-Relay`** melalui HP/Laptop Anda dan hubungkan.
3. Portal login/kontrol web akan otomatis muncul pada layar HP Anda. Jika tidak muncul otomatis, buka browser lalu ketik alamat **`http://192.168.4.1`** atau alamat web acak apa saja.
4. Anda dapat:
   * Mengaktifkan/menonaktifkan masing-masing relay.
   * Melihat nilai suhu dan kelembaban.
   * Memilih jenis sensor yang terpasang (DHT11/DHT22/DS18B20) melalui menu pengaturan jenis sensor.
   * Melihat pergerakan grafik tren suhu secara real-time.
