# 🔌 Web Relay Control - Mode Access Point (Captive Portal)

Contoh koding ini mengaktifkan modul ESP8266 sebagai **Access Point (AP)** mandiri yang memancarkan sinyal WiFi sendiri bernama `IoT-WiFi-Relay`. Anda dapat mengontrol 2-channel relay secara offline menggunakan tampilan web control panel modern (Glassmorphism) langsung dari browser HP/PC Anda tanpa membutuhkan router internet.

Proyek ini menggunakan **Captive Portal** sehingga ketika Anda menghubungkan perangkat ke WiFi modul, halaman kontrol akan otomatis pop-up di layar Anda.

---

## 🛠️ Persiapan Library

Sebelum meng-upload, pastikan Anda telah menginstal library berikut di Arduino IDE Anda:

1. **ESPAsyncWebServer** (oleh me-no-dev)
   * *Catatan:* Library ini mungkin perlu diunduh sebagai file `.zip` dari GitHub resmi dan diinstal manual via Arduino IDE (*Sketch -> Include Library -> Add .ZIP Library*).
2. **ESPAsyncTCP** (oleh me-no-dev) - Dependency untuk ESPAsyncWebServer pada platform ESP8266.

---

## 🧩 Fitur & Konfigurasi Pinout

* **Relay 1**: Disambungkan ke **GPIO 4 (D2)**
* **Relay 2**: Disambungkan ke **GPIO 5 (D1)**
* **Nama WiFi (SSID) Default**: `IoT-WiFi-Relay` (tanpa password).
* **Alamat IP Default**: `192.168.4.1`

---

## 💻 Cara Menggunakan

1. Buka file `Relay-Web-AP.ino` menggunakan Arduino IDE.
2. Upload sketch tersebut ke modul ESP8266 Anda.
3. Setelah selesai, cari jaringan WiFi baru di HP/Laptop Anda bernama **`IoT-WiFi-Relay`** dan lakukan koneksi.
4. Halaman portal kontrol web otomatis terbuka. Jika tidak terbuka, silakan buka web browser di HP Anda dan akses alamat **`http://192.168.4.1`**.
5. Gunakan tombol switch interaktif di layar untuk menghidupkan atau mematikan masing-masing relay.
