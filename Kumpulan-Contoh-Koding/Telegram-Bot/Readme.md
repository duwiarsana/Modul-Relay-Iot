# 🤖 Telegram Bot Controller & Alarm Peringatan Suhu

Contoh koding ini mengintegrasikan modul ESP8266 dengan **Telegram Bot** sehingga Anda bisa mengontrol 2-channel relay dan membaca data sensor suhu langsung dari aplikasi chat Telegram di mana saja Anda berada (kontrol global cloud).

Program ini juga dilengkapi dengan halaman web dashboard lokal untuk memudahkan Anda mengatur **Token Bot**, **Chat ID**, memilih jenis sensor (**DHT11/DHT22/DS18B20**), serta mengaktifkan **Alarm Notifikasi Suhu Otomatis** ke HP Anda jika mendeteksi suhu melewati ambang batas aman.

---

## 🛠️ Persiapan Library

Pastikan library berikut sudah terinstal di Arduino IDE Anda:

1. **ESPAsyncWebServer** (oleh me-no-dev)
2. **ESPAsyncTCP** (oleh me-no-dev)
3. **DHT sensor library** (oleh Adafruit)
4. **Adafruit Unified Sensor** (dependency DHT)
5. **OneWire** (oleh Paul Stoffregen)
6. **DallasTemperature** (oleh Miles Burton)

---

## 🧩 Konfigurasi Pinout

* **Relay 1**: Disambungkan ke **GPIO 4 (D2)**
* **Relay 2**: Disambungkan ke **GPIO 5 (D1)**
* **Sensor Temp/Hum**: Disambungkan ke **GPIO 12 (D6)**

---

## ⚙️ Cara Menggunakan

### 1. Persiapan Kredensial WiFi & Upload
1. Buka file `Telegram-Bot.ino` di Arduino IDE.
2. Edit baris berikut dengan SSID & Password WiFi rumah Anda:
   ```cpp
   const char* ssid = "your-SSID";
   const char* password = "your-PASSWORD";
   ```
3. Upload program ke modul ESP8266 Anda.
4. Buka Serial Monitor, catat IP Address lokal modul Anda (misalnya `192.168.1.10`).
5. Modul juga mendukung mDNS, Anda bisa mengakses konfigurasinya lewat alamat browser lokal: **`http://telegram-bot.local`**.

### 2. Dapatkan Token Telegram Bot & Chat ID
1. Buat bot baru di Telegram melalui chat dengan [@BotFather](https://t.me/botfather). Kirim perintah `/newbot` dan ikuti petunjuknya hingga mendapatkan **API Token**.
2. Dapatkan Chat ID akun Telegram Anda (atau grup/channel tempat Anda ingin menerima notifikasi alarm) dengan mengirim chat sembarang ke bot Anda, lalu buka browser dan akses URL ini (ganti `<TOKEN>` dengan API Token bot Anda):
   `https://api.telegram.org/bot<TOKEN>/getUpdates`
3. Cari properti `"chat":{"id":XXXXXXXXX ...}` pada hasil JSON dan catat angka ID tersebut.

### 3. Masukkan Konfigurasi di Web Panel Modul
1. Hubungkan HP/Laptop Anda ke WiFi yang sama dengan modul.
2. Buka browser dan ketik alamat IP modul Anda atau **`http://telegram-bot.local`**.
3. Isi kolom **Bot Token Telegram** dan **Chat ID Telegram**.
4. Pilih jenis sensor (**DHT11**, **DHT22**, atau **DS18B20**) yang Anda gunakan pada modul.
5. Jika ingin menggunakan alarm otomatis, tentukan **Batas Alarm Suhu (dalam °C)** dan centang opsi **Kirim Notifikasi Alarm Suhu**.
6. Klik **Simpan Konfigurasi**.
7. Anda dapat mencoba menekan tombol **Kirim Pesan Tes Bot** untuk memastikan bot Anda dapat mengirim pesan ke chat Telegram Anda.

---

## 🗣️ Daftar Perintah Telegram Bot

Buka chat bot Anda di Telegram, lalu ketik perintah suara/teks berikut untuk kontrol modul:

* `/start` - Menampilkan panduan dan daftar perintah bot.
* `/status` - Membaca status suhu, kelembaban, serta kondisi kedua relay saat ini.
* `/relay1_on` - Menyalakan Relay 1.
* `/relay1_off` - Mematikan Relay 1.
* `/relay2_on` - Menyalakan Relay 2.
* `/relay2_off` - Mematikan Relay 2.
