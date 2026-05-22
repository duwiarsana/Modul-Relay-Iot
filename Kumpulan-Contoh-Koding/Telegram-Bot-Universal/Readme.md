# 🤖 Kontrol Relay & Monitoring DHT11 via Telegram Bot (UniversalTelegramBot)

Contoh program ini menunjukkan cara mengontrol 2 buah relay dan memonitor suhu serta kelembaban dari sensor **DHT11** menggunakan **ESP8266** melalui aplikasi chat **Telegram**. Project ini menggunakan library **UniversalTelegramBot** yang sangat populer dan stabil untuk mengintegrasikan ESP8266 dengan Telegram API.

Project ini dibuat khusus untuk **modul Relay IoT 2 channel berbasis ESP8266** karya [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana).

---

## 🔧 Fitur

- **Kontrol Relay Nirkabel:** Nyalakan dan matikan Relay 1 & Relay 2 langsung lewat chat Telegram.
- **Monitoring Sensor DHT11:** Dapatkan data suhu (°C) dan kelembaban (%) real-time lewat perintah chat.
- **Fitur Keamanan (Chat ID Lock):** Hanya memproses perintah dari Chat ID pemilik terdaftar. Perintah dari orang lain otomatis ditolak.
- **Koneksi Aman & Awet:** Menggunakan `WiFiClientSecure` dengan mode *insecure* agar bot tetap berjalan tanpa khawatir sertifikat SSL Telegram kadaluarsa.

---

## 📚 Library yang Dibutuhkan

Sebelum melakukan upload, pastikan Anda telah menginstal beberapa library berikut melalui **Library Manager** di Arduino IDE:

1. **UniversalTelegramBot** (oleh Brian Lough)
2. **ArduinoJson** (oleh Benoit Blanchon) - *Sangat direkomendasikan menggunakan versi 6.x.x* (misalnya `v6.21.3`).
3. **DHT sensor library** (oleh Adafruit)
4. **Adafruit Unified Sensor** (oleh Adafruit - sebagai dependency DHT library)

---

## 🚀 Cara Penggunaan

### 1. Membuat Bot Telegram & Mendapatkan Token
1. Buka aplikasi Telegram, lalu cari **@BotFather**.
2. Kirim pesan `/newbot` untuk membuat bot baru.
3. Masukkan nama bot dan username bot sesuai instruksi (username harus diakhiri dengan kata `_bot`, contoh: `relay_iot_duwi_bot`).
4. Setelah berhasil, Anda akan menerima **API Token** (contoh: `1234567890:ABCdefGhIJKlmNoPQRsTUVwxyZ`). Simpan token ini.

### 2. Mendapatkan Chat ID Telegram Anda
Untuk keamanan agar orang asing tidak bisa mengontrol relay Anda, dapatkan ID akun Telegram Anda sendiri:
1. Di Telegram, cari bot **@userinfobot** atau **@IDBot**.
2. Kirim pesan apa saja (atau klik `/start`).
3. Bot akan membalas dengan menampilkan informasi **Id** Anda (berupa barisan angka, contoh: `123456789`). Simpan angka ID ini.

### 3. Konfigurasi Sketch Arduino
1. Buka file [Telegram-Bot-Universal.ino](Telegram-Bot-Universal.ino) di Arduino IDE.
2. Ubah konfigurasi jaringan WiFi pada baris kode berikut:
   ```cpp
   const char* ssid = "YOUR_SSID";          // Nama WiFi Anda
   const char* password = "YOUR_PASSWORD";  // Password WiFi Anda
   ```
3. Masukkan token bot dan Chat ID yang sudah Anda dapatkan sebelumnya:
   ```cpp
   #define BOT_TOKEN "1234567890:YOUR_BOT_TOKEN_HERE"
   #define CHAT_ID "YOUR_CHAT_ID_HERE"
   ```
4. Lakukan upload kode program ke board ESP8266 Anda (Wemos D1 Mini, NodeMCU, atau ESP-12E/F).

### 4. Menjalankan & Interaksi dengan Bot
1. Hubungkan modul ke sumber daya (USB / adaptor 5V).
2. Buka **Serial Monitor** pada baudrate `115200` untuk memantau status koneksi WiFi.
3. Buka bot Telegram yang Anda buat di langkah pertama, lalu klik **Start** atau kirim pesan `/start`.
4. Anda akan menerima pesan sambutan beserta daftar perintah yang bisa digunakan.
5. Kirim perintah berikut ke bot:
   - `/relay1_on` - Menyalakan Relay 1
   - `/relay1_off` - Mematikan Relay 1
   - `/relay2_on` - Menyalakan Relay 2
   - `/relay2_off` - Mematikan Relay 2
   - `/status` - Membaca data DHT11 (suhu & kelembaban) serta kondisi Relay 1 & 2 saat ini.

---

## 📌 Skema Sambungan Modul

| Komponen | Pin Modul | Pin GPIO ESP8266 |
| :--- | :--- | :--- |
| **Relay 1** | D2 | GPIO 4 |
| **Relay 2** | D1 | GPIO 5 |
| **Sensor DHT11** | D6 (JST Connector) | GPIO 12 |

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
