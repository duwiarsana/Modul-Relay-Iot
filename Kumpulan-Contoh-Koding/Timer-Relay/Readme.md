# ⏱️ Timer Relay ESP8266 - Web Based

Contoh program ini dibuat untuk modul **Relay IoT 2 Channel berbasis ESP8266** karya [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana). Dengan project ini, kamu bisa mengatur waktu ON dan OFF relay lewat browser menggunakan antarmuka web sederhana yang responsif.

---

## 🔧 Fitur

- Kontrol 2 relay berdasarkan waktu ON/OFF (jam dan menit)
- Pengaturan waktu dilakukan via browser
- Waktu tersimpan di EEPROM
- Sinkronisasi waktu otomatis via NTP (zona waktu WIB / GMT+7)
- Tampilan web minimalis, pakai CSS Milligram
- Menampilkan jam real-time di web & Serial Monitor
- Print IP address ESP8266 ke Serial saat startup

---

## 📚 Library yang Dibutuhkan

Pastikan kamu install library berikut di Arduino IDE:

- `ESPAsyncWebServer`
- `ESPAsyncTCP`
- `NTPClient`
- `EEPROM`

---

## 🚀 Cara Pakai

1. Ganti SSID dan password WiFi di bagian `const char* ssid` dan `password`.
2. Upload ke board ESP8266 (misalnya Wemos D1 Mini atau NodeMCU).
3. Buka **Serial Monitor**, catat alamat IP dari ESP8266.
4. Akses modul via browser menggunakan alamat IP tersebut atau langsung menggunakan nama domain lokal mDNS: `http://timer-relay.local`.
5. Atur waktu ON/OFF masing-masing relay.
6. Klik **Simpan**, dan relay akan menyala/mati sesuai jadwal.

---

## 🛒 Tempat Beli Modul

Kamu bisa beli modul Relay IoT ini di Tokopedia (buatan Duwi Arsana):  
👉 [Tokopedia](https://tokopedia.link/BubYYGYAYRb)

---

## ☕ Dukung Proyek Ini

Kalau kamu terbantu dengan project ini dan ingin mendukung saya untuk terus berkarya, bisa traktir saya kopi digital lewat Saweria:

👉 [https://saweria.co/duwiarsana](https://saweria.co/duwiarsana)

<p align="left">
  <img src="https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=https://saweria.co/duwiarsana" alt="QR Saweria">
</p>

---

## 📄 Lisensi

Kode ini dirilis di bawah lisensi [MIT](https://opensource.org/licenses/MIT).  
Silakan digunakan, dimodifikasi, dan disebarluaskan — cukup cantumkan kredit ke pembuat asli:

**👤 Duwi Arsana**  
🔗 [https://duwiarsana.com](https://duwiarsana.com)  
📺 [YouTube: Anak Agung Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana)  
📩 Email: anakagungduwiarsana@gmail.com
