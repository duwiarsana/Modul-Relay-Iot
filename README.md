# 🔌 IoT Relay Module ESP8266 - 2 Channel + DHT11

Modul relay IoT berbasis ESP8266 ini dirancang untuk mengontrol 2 buah relay melalui jaringan WiFi. Modul ini juga dilengkapi konektor JST untuk sensor DHT11, sehingga memungkinkan pengambilan data suhu dan kelembaban secara real-time.

---

This is a simple 2-channel IoT relay module based on ESP8266, designed for smart automation projects. It includes a JST connector for DHT11 sensor, making it perfect for temperature and humidity-based control.

## ✨ Fitur / Features

- 2 Channel Relay (NO/NC)
- ESP8266 onboard (bisa pakai ESP-12E/F)
- JST Connector untuk DHT11 sensor
- Tegangan kerja 5V
- Ukuran compact, siap dipasang di box
- Bisa di-flash dengan Arduino IDE atau PlatformIO

## ⚙️ Spesifikasi / Specifications

| Komponen       | Spesifikasi              |
|----------------|--------------------------|
| MCU            | ESP8266 (ESP-12E/F)      |
| Relay          | 5V, max 10A              |
| Sensor Support | DHT11 via JST Connector  |
| Tegangan Input | 5V DC                    |

## 🧩 GPIO yang Digunakan

| Fungsi       | GPIO ESP8266 | Keterangan |
|--------------|--------------|------------|
| Relay 1      | GPIO 4 (D2)  | Aktif HIGH |
| Relay 2      | GPIO 5 (D1)  | Aktif HIGH |
| Sensor DHT11 | GPIO 12 (D6) | Plug JST   |

> Pastikan GPIO yang digunakan di sketch sesuai dengan pinout ini agar modul berjalan dengan benar.

## 🔧 Instalasi Firmware

1. Pasang ESP8266 ke board.
2. Hubungkan ke komputer via USB to Serial.
3. Buka Arduino IDE atau PlatformIO.
4. Upload firmware sesuai kebutuhan kamu.

Contoh firmware disediakan di folder [`/Kumpulan-Contoh-Koding`](Kumpulan-Contoh-Koding).

---

## 🔌 Penggunaan dengan Wemos D1 Mini / NodeMCU

Jika Anda ingin merakit proyek ini menggunakan development board **Wemos D1 Mini** atau **NodeMCU ESP8266** (misalnya menggunakan breadboard / kabel jumper), Anda dapat menggunakan kode program yang sama tanpa perlu mengubah isi kodenya.

### 📌 Skema Sambungan Kabel (Wiring)

Hubungkan kaki komponen ke pin board Wemos D1 Mini / NodeMCU sebagai berikut:

| Komponen | Pin Board (Wemos D1 Mini / NodeMCU) | Pin GPIO ESP8266 | Keterangan |
| :--- | :--- | :--- | :--- |
| **Relay 1** (IN / Trigger) | **D2** | GPIO 4 | Aktif HIGH |
| **Relay 2** (IN / Trigger) | **D1** | GPIO 5 | Aktif HIGH |
| **Sensor DHT11** (Data) | **D6** | GPIO 12 | Hubungkan ke pin data DHT11 |
| **VCC** (Relay & DHT11) | **5V / VIN / 3.3V** | - | Sesuaikan dengan tegangan kerja relay/sensor Anda |
| **GND** (Relay & DHT11) | **GND** | - | Hubungkan semua ground menjadi satu |

> 💡 **Info:** Di dalam kode program, pin didefinisikan menggunakan nomor GPIO langsung (`4` untuk Relay 1, `5` untuk Relay 2, dan `12` untuk DHT11). Pin ini secara default terpetakan ke **D2**, **D1**, dan **D6** pada board NodeMCU / Wemos D1 Mini.

### 💻 Cara Upload di Arduino IDE

1. Buka Arduino IDE dan buka salah satu contoh sketch dari folder `Kumpulan-Contoh-Koding`.
2. Masuk ke **Tools > Board > ESP8266 Boards** lalu pilih board sesuai yang Anda gunakan (misal: `LOLIN(WEMOS) D1 mini (clone)` atau `NodeMCU 1.0 (ESP-12E Module)`).
3. Pilih port COM yang sesuai.
4. Klik **Upload**.

---

## 💡 Contoh Penggunaan

- Saklar lampu pintar
- Kontrol pompa air otomatis berdasarkan suhu/kelembaban
- Otomasi kipas ruangan

---

## 📺 Tonton Video Demo

Klik gambar di bawah ini untuk menonton demo penggunaan modul ini:

🎥 [Tonton di YouTube](https://www.youtube.com/watch?v=41Ir59VN2lw)

[![Tonton Video Demo](https://img.youtube.com/vi/41Ir59VN2lw/0.jpg)](https://www.youtube.com/watch?v=41Ir59VN2lw)

<p align="left">
  <img src="https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=https://www.youtube.com/watch?v=41Ir59VN2lw" alt="QR YouTube">
</p>

---

## 🛒 Tempat Pembelian

Modul ini bisa kamu beli di Tokopedia:  
👉 [https://tokopedia.link/BubYYGYAYRb](https://tokopedia.link/BubYYGYAYRb)

---

## ☕ Dukung Saya

Kalau kamu suka dengan proyek ini dan ingin support agar saya bisa terus berkarya:

👉 **Saweria:** [https://saweria.co/duwiarsana](https://saweria.co/duwiarsana)

<p align="left">
  <img src="https://api.qrserver.com/v1/create-qr-code/?size=160x160&data=https://saweria.co/duwiarsana" alt="Saweria QR">
</p>

---

## 📁 Folder `Kumpulan-Contoh-Koding`

Folder ini berisi kumpulan contoh koding siap pakai yang bisa langsung di-flash ke modul:

1. **[Relay-Web](Kumpulan-Contoh-Koding/Relay-Web)**
   - Mode Client (konek ke WiFi rumah).
   - Tampilan web control panel modern (Glassmorphism) untuk menghidupkan dan mematikan 2 relay secara nirkabel.
   - Sinkronisasi status relay secara real-time pada halaman web saat pertama kali dimuat.
   - **Dukungan mDNS:** Bisa diakses langsung menggunakan domain lokal `http://relay-web.local` tanpa perlu tahu IP address-nya.

2. **[Relay-Web-AP](Kumpulan-Contoh-Koding/Relay-Web-AP) (Baru! ⚡)**
   - Mode Access Point (AP) dengan **Captive Portal**.
   - ESP8266 memancarkan WiFi sendiri (`IoT-WiFi-Relay`).
   - Begitu HP/PC tersambung ke WiFi tersebut, halaman kontrol web akan **otomatis muncul** tanpa perlu mengetik alamat IP secara manual.

3. **[Monitoring-dan-Kontrol](Kumpulan-Contoh-Koding/Monitoring-dan-Kontrol)**
   - Mode Client (konek ke WiFi rumah).
   - Dilengkapi grafik tren suhu real-time (menggunakan Chart.js) dari sensor DHT11.
   - Status switch relay otomatis kembali ke posisi semula jika koneksi terputus.
   - **Dukungan mDNS:** Bisa diakses langsung menggunakan domain lokal `http://monitoring-kontrol.local` tanpa perlu tahu IP address-nya.

4. **[Monitoring-dan-Kontrol-AP](Kumpulan-Contoh-Koding/Monitoring-dan-Kontrol-AP) (Baru! ⚡)**
   - Mode Access Point (AP) dengan **Captive Portal** + sensor suhu DHT11.
   - ESP8266 memancarkan WiFi (`IoT-Smart-Relay`).
   - Menampilkan kontrol relay interaktif dan grafik suhu secara offline (langsung konek HP ke modul).

5. **[Timer-Relay](Kumpulan-Contoh-Koding/Timer-Relay)**
   - Mode Client (konek ke WiFi rumah untuk sinkronisasi waktu NTP).
   - Panel penjadwalan waktu (ON/OFF) otomatis berbasis jam digital real-time untuk masing-masing relay.
   - Dilengkapi sistem proteksi agar aman dari error saat EEPROM kosong.
   - **Dukungan mDNS:** Bisa diakses langsung menggunakan domain lokal `http://timer-relay.local` tanpa perlu tahu IP address-nya.

6. **[Smart-Thermostat](Kumpulan-Contoh-Koding/Smart-Thermostat) (Baru! ⚡)**
   - Mengubah modul menjadi pengatur suhu otomatis (Klimatisasi).
   - Relay 1 sebagai pendingin (Cooler/Kipas), Relay 2 sebagai pemanas (Heater).
   - Menggunakan dial melingkar (*circular thermostat gauge*) yang interaktif untuk menyetel target suhu dan histeresis.
   - Dilengkapi dengan 4 mode operasi: **Auto**, **Manual Cool**, **Manual Heat**, dan **System Off**.
   - **Dukungan mDNS:** Bisa diakses langsung menggunakan domain lokal `http://smart-thermostat.local` tanpa perlu tahu IP address-nya.

7. **[Telegram-Bot](Kumpulan-Contoh-Koding/Telegram-Bot) (Baru! ⚡)**
   - Mengontrol 2 relay dan membaca sensor DHT11 langsung lewat chat Telegram Bot.
   - Token Bot Telegram dan Chat ID dapat diubah dan disimpan langsung dari halaman web modul secara asinkron.
   - Dilengkapi dengan fitur **Alarm Notifikasi Suhu Otomatis** ke HP Anda jika mendeteksi suhu melebihi ambang batas aman.
   - Tanpa library eksternal Telegram pihak ketiga (koneksi HTTPS super ringan & anti-fingerprint-expired).
   - **Dukungan mDNS:** Halaman konfigurasi lokal bisa diakses langsung menggunakan domain lokal `http://telegram-bot.local` tanpa perlu tahu IP address-nya.

8. **[Telegram-Bot-Universal](Kumpulan-Contoh-Koding/Telegram-Bot-Universal)**
   - Mengontrol 2 relay dan membaca sensor DHT11 lewat chat Telegram menggunakan library populer **UniversalTelegramBot** (oleh Brian Lough) & **ArduinoJson**.
   - Dilengkapi dengan fitur keamanan verifikasi Chat ID agar hanya pemilik bot terdaftar yang memiliki akses kontrol relay.
   - Menggunakan client secure dengan mode bypass sertifikat SSL (`client.setInsecure()`) agar modul berjalan awet tanpa khawatir perubahan sertifikat HTTPS api.telegram.org.

## 📄 Lisensi

Proyek ini dirilis di bawah lisensi MIT. Silakan gunakan, modifikasi, dan bagikan sesuai kebutuhan, selama tetap menyertakan kredit ke pembuat asli.

---

## 🧠 Kontribusi

Pull request sangat welcome. Kalau ada saran, bug, atau ide pengembangan, langsung saja buka issue ya!

---

## 📱 Sosial Media

Dibuat oleh [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana)  
Website: [https://duwiarsana.com](https://duwiarsana.com)  
Email: anakagungduwiarsana@gmail.com
