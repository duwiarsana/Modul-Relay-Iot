# 🗣️ Kontrol Suara Google Assistant (Google Home) dengan Sinric Pro

Contoh koding ini menunjukkan cara mengontrol 2-channel relay dan memantau sensor DHT11 menggunakan **Google Assistant / Google Home** pada modul IoT Relay. 

Karena Google Home memerlukan integrasi berbasis Cloud (tidak mendukung emulasi Hue Bridge lokal secara langsung seperti Alexa), kita menggunakan platform perantara gratis bernama **Sinric Pro**.

---

## 🛠️ Persiapan Library

Pastikan library berikut sudah terinstal di Arduino IDE Anda:

1. **SinricPro** (oleh sinricpro)
   - Buka Arduino IDE -> **Library Manager** -> Cari `SinricPro` -> Klik **Install**.
2. **DHT sensor library** (oleh Adafruit)
   - Cari `DHT sensor library` -> Klik **Install**.
3. **Adafruit Unified Sensor** (dependency untuk DHT library)

---

## ⚙️ Langkah Setup Cloud (Sinric Pro)

Sebelum meng-upload kode, Anda perlu membuat akun dan mendaftarkan perangkat di portal Sinric Pro secara gratis:

1. Buka situs [sinric.pro](https://sinric.pro/) dan daftar akun baru.
2. Setelah login, masuk ke menu **Credentials** untuk mendapatkan:
   - `APP KEY`
   - `APP SECRET`
3. Masuk ke menu **Devices** -> Klik **Add Device**:
   - **Device 1 (Relay Satu)**:
     - Nama: `Relay Satu`
     - Tipe Perangkat: `Switch`
     - Catat `DEVICE ID` yang dihasilkan.
   - **Device 2 (Relay Dua)**:
     - Nama: `Relay Dua`
     - Tipe Perangkat: `Switch`
     - Catat `DEVICE ID` yang dihasilkan.
   - **Device 3 (Sensor DHT11)**:
     - Nama: `Suhu Kamar` (atau sesuai keinginan)
     - Tipe Perangkat: `Temperature Sensor`
     - Catat `DEVICE ID` yang dihasilkan.

---

## 💻 Cara Menggunakan Kode

1. Buka file `Google-Assistant-Control.ino` di Arduino IDE.
2. Ubah konfigurasi WiFi Anda:
   ```cpp
   const char* ssid = "SSID-WIFI-ANDA";
   const char* password = "PASSWORD-WIFI-ANDA";
   ```
3. Masukkan kode credentials dari portal Sinric Pro yang sudah Anda buat tadi:
   ```cpp
   #define APP_KEY           "MASUKKAN-APP-KEY-DISINI"
   #define APP_SECRET        "MASUKKAN-APP-SECRET-DISINI"
   #define SWITCH_ID_1       "MASUKKAN-DEVICE-ID-RELAY-1"
   #define SWITCH_ID_2       "MASUKKAN-DEVICE-ID-RELAY-2"
   #define TEMP_SENSOR_ID    "MASUKKAN-DEVICE-ID-DHT11"
   ```
4. Upload sketch ke ESP8266 Anda.

---

## 📱 Menghubungkan ke Google Home / Google Assistant

1. Buka aplikasi **Google Home** di smartphone Anda.
2. Klik tombol **"+"** (Tambah) di pojok kiri atas -> Pilih **Set up device**.
3. Pilih **Works with Google**.
4. Cari layanan **Sinric Pro** pada daftar pencarian, lalu pilih.
5. Login menggunakan akun **Sinric Pro** yang sudah Anda buat.
6. Hubungkan akun (*Link account*). Google Home akan otomatis mengimpor ketiga perangkat virtual yang telah Anda buat di Sinric Pro (`Relay Satu`, `Relay Dua`, dan `Suhu Kamar`).

---

## 🗣️ Contoh Perintah Suara Google Assistant

Sekarang Anda bisa memberikan perintah suara menggunakan Google Assistant di HP atau Google Nest speaker Anda:

* *"Ok Google, nyalakan Relay Satu."*
* *"Ok Google, matikan Relay Dua."*
* *"Ok Google, berapa suhu di Suhu Kamar?"*
