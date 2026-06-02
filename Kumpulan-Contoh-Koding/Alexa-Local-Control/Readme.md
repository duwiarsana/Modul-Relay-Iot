# 🗣️ Local Voice Control (Amazon Alexa) Tanpa Cloud

Contoh koding ini memungkinkan Anda untuk mengontrol 2-channel relay secara nirkabel menggunakan perintah suara **Amazon Alexa** langsung di jaringan lokal Anda. Dengan menggunakan library **Espalexa**, ESP8266 akan bertindak sebagai perangkat tiruan (*emulation*) Philips Hue Bridge, sehingga perangkat gema (Echo) atau aplikasi Alexa di HP Anda dapat langsung mendeteksi dan mengontrol modul tanpa memerlukan server cloud luar atau setup *custom skill* pihak ketiga.

---

## 🛠️ Persiapan Library

Sebelum meng-upload kode program ke modul, Anda perlu memastikan library berikut sudah terinstal di Arduino IDE Anda:

1. **Espalexa** (oleh Aircoookie)
   - Cari di Library Manager: `Espalexa` lalu klik **Install**.
2. **DHT sensor library** (oleh Adafruit)
   - Cari di Library Manager: `DHT sensor library` lalu klik **Install**.
3. **Adafruit Unified Sensor** (dependency untuk DHT library)
   - Biasanya ditawarkan untuk diinstal otomatis saat menginstal DHT sensor library. Jika tidak, cari `Adafruit Unified Sensor` dan instal secara manual.

---

## 🧩 Konfigurasi Pinout Modul

Sesuai dengan hardware desain papan modul ini:
- **Relay 1** disambungkan ke **GPIO 4 (D2)**
- **Relay 2** disambungkan ke **GPIO 5 (D1)**
- **Sensor DHT11** disambungkan ke **GPIO 12 (D6)**

---

## 💻 Langkah Penggunaan

1. Buka file `Alexa-Local-Control.ino` menggunakan Arduino IDE atau PlatformIO.
2. Edit baris berikut dengan nama WiFi dan password di rumah/lokasi Anda:
   ```cpp
   const char* ssid = "your-SSID";
   const char* password = "your-PASSWORD";
   ```
3. Upload sketch ke modul ESP8266 Anda.
4. Buka **Serial Monitor** pada baudrate **115200** untuk melihat status koneksi WiFi dan IP Address yang didapatkan oleh modul.
5. Pastikan smartphone Anda (atau perangkat Amazon Echo/Alexa Speaker) terhubung ke **jaringan WiFi yang sama** (satu router) dengan modul ESP8266.

---

## 📱 Cara Menghubungkan ke Aplikasi Amazon Alexa

1. Buka aplikasi **Amazon Alexa** di smartphone Anda.
2. Masuk ke menu **Devices** (Perangkat) di bagian bawah.
3. Tekan tanda **"+"** di pojok kanan atas, lalu pilih **Add Device**.
4. Pilih tipe perangkat **Light** (Lampu) atau **Switch** (Sakelar), lalu pilih brand **Other** (Lainnya).
5. Klik tombol **Discover Devices**. Alexa akan melakukan pemindaian jaringan lokal selama sekitar 45 detik.
6. Setelah pemindaian selesai, Alexa akan menemukan 2 perangkat baru dengan nama:
   - **"Relay Satu"**
   - **"Relay Dua"**
7. Selesaikan proses setup dan masukkan perangkat ke ruangan yang Anda inginkan (misal: Ruang Tamu, Kamar Tidur).

---

## 🗣️ Contoh Perintah Suara

Anda bisa langsung mengontrol relay menggunakan perintah suara (dalam bahasa Inggris atau bahasa yang didukung Alexa Anda):

* *"Alexa, turn on Relay Satu."*
* *"Alexa, turn off Relay Satu."*
* *"Alexa, turn on Relay Dua."*
* *"Alexa, set Relay Satu to 50 percent."* (Espalexa mendukung tingkat kecerahan/dimmer virtual, Anda bisa menggunakannya untuk logika tambahan).

---

## 📊 Monitoring Suhu (DHT11)

Meskipun Espalexa berfokus pada emulasi saklar/lampu, modul ini tetap membaca data sensor DHT11 secara non-blocking setiap 5 detik. Anda dapat melihat output nilai **Suhu (°C)** dan **Kelembaban (%)** secara real-time melalui **Serial Monitor** Arduino IDE.
