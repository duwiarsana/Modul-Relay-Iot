# 🌡️ Smart Thermostat ESP8266 dengan Web Interface Modern

Contoh program ini menunjukkan cara membuat sistem **Smart Thermostat** berbasis **ESP8266** dengan tampilan antarmuka web (Web Dashboard) yang modern, interaktif, dan responsif. Sistem ini mampu mengendalikan pendingin (Cooler/Exhaust) dan pemanas (Heater) secara otomatis berdasarkan target suhu dan histeresis, serta mendukung beberapa jenis sensor suhu populer.

Project ini dibuat khusus untuk **modul Relay IoT 2 channel berbasis ESP8266** karya [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana).

---

## 🔧 Fitur Utama

- **Web Dashboard Modern & Responsif:** Antarmuka gelap (dark mode) premium dengan ornamen gradient, dial melingkar dinamis, ikon FontAwesome, dan performa tinggi (tanpa reload halaman).
- **Mendukung Multi-Sensor:** Kompatibel dengan sensor **DHT11**, **DHT22**, dan **DS18B20** pada pin GPIO yang sama (dapat diubah langsung dari halaman web secara real-time).
- **Mode Kontrol Fleksibel:**
  - **Auto:** Mengontrol cooler dan heater otomatis berdasarkan target suhu dan nilai histeresis yang Anda tetapkan.
  - **Cool:** Mengaktifkan Cooler secara manual.
  - **Heat:** Mengaktifkan Heater secara manual.
  - **Off:** Mematikan semua relay.
- **Penyimpanan EEPROM:** Menyimpan semua pengaturan terakhir (target suhu, histeresis, mode, jenis sensor) secara otomatis ke memori flash ESP8266 sehingga tidak hilang saat listrik padam.
- **Dukungan mDNS:** Anda dapat mengakses web panel dengan mengetik `http://smart-thermostat.local` di browser tanpa perlu menghafal alamat IP.
- **UI Pintar:** Widget kelembaban otomatis memudar (fade-out) ketika menggunakan sensor DS18B20 karena sensor tersebut tidak mendeteksi kelembaban.

---

## 📚 Library yang Dibutuhkan

Sebelum melakukan upload, pastikan Anda telah menginstal library berikut melalui **Library Manager** di Arduino IDE:

1. **ESPAsyncWebServer** (oleh me-no-dev)
2. **ESPAsyncTCP** (oleh me-no-dev)
3. **DHT sensor library** (oleh Adafruit)
4. **Adafruit Unified Sensor** (oleh Adafruit)
5. **OneWire** (oleh Paul Stoffregen)
6. **DallasTemperature** (oleh Miles Burton)

---

## 🚀 Cara Penggunaan

### 1. Konfigurasi Sketch Arduino
1. Buka file [Smart-Thermostat.ino](Smart-Thermostat.ino) di Arduino IDE.
2. Cari dan ubah konfigurasi koneksi WiFi Anda pada baris kode berikut:
   ```cpp
   const char* ssid = "your-SSID";          // Ganti dengan SSID WiFi Anda
   const char* password = "your-PASSWORD";  // Ganti dengan Password WiFi Anda
   ```
3. Lakukan upload kode program ke board ESP8266 Anda (Wemos D1 Mini, NodeMCU, atau ESP-12E/F).

### 2. Mengakses Halaman Control Panel
1. Setelah proses upload selesai, buka **Serial Monitor** pada baudrate `115200`.
2. Pastikan ESP8266 Anda telah tersambung ke jaringan WiFi Anda dan catat alamat IP yang ditampilkan (contoh: `192.168.1.15`).
3. Buka web browser di HP atau Laptop yang terhubung ke jaringan WiFi yang sama, lalu ketik alamat IP tersebut, atau cukup akses menggunakan link mDNS:  
   👉 **[http://smart-thermostat.local](http://smart-thermostat.local)**
4. Selamat! Anda sekarang dapat memantau suhu secara langsung dan mengatur target thermostat Anda secara nirkabel.

---

## 📌 Skema Sambungan Modul

| Komponen | Pin Modul | Pin GPIO ESP8266 |
| :--- | :--- | :--- |
| **Relay 1 (Cooler / Fan)** | D2 | GPIO 4 |
| **Relay 2 (Heater / Pemanas)** | D1 | GPIO 5 |
| **Sensor (DHT11/DHT22/DS18B20)** | D6 (JST Connector) | GPIO 12 |

> [!NOTE]
> Jika Anda menggunakan sensor DS18B20, pastikan Anda telah menambahkan resistor pull-up sebesar `4.7kΩ` di antara jalur VCC dan Data (biasanya pada modul Relay IoT Duwi Arsana, resistor pull-up ini sudah terintegrasi atau dapat dipasang secara modular).

---

## 🛒 Beli Modulnya

Modul Relay IoT ini bisa kamu beli di Tokopedia:  
👉 [https://tokopedia.link/BubYYGYAYRb](https://tokopedia.link/BubYYGYAYRb)

---

## ☕ Dukung Saya

Kalau project ini bermanfaat, kamu bisa support saya lewat Saweria. Donasi dari kamu bikin saya makin semangat bikin konten edukasi elektronik & IoT!

👉 [https://saweria.co/duwiarsana](https://saweria.co/duwiarsana)

<p align="left">
  <img src="https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=https://saweria.co/duwiarsana" alt="QR Saweria">
</p>

---

## 📄 Lisensi

Kode ini dirilis dengan lisensi [MIT](https://opensource.org/licenses/MIT).  
Boleh dipakai bebas untuk project pribadi, edukasi, atau dikembangkan lebih lanjut.

**Dibuat oleh Duwi Arsana**  
🔗 [https://duwiarsana.com](https://duwiarsana.com)  
📺 [YouTube: Anak Agung Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana)  
📩 Email: anakagungduwiarsana@gmail.com
