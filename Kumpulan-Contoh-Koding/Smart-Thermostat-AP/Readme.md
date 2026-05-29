# 🌡️ Smart Thermostat ESP8266 dengan Web Interface Modern (AP Mode / Offline)

Contoh program ini menunjukkan cara membuat sistem **Smart Thermostat** berbasis **ESP8266** yang dapat beroperasi secara mandiri sebagai **Access Point (AP)** tanpa bergantung pada koneksi internet atau router hotspot. Pengguna dapat menghubungkan smartphone atau laptop langsung ke jaringan WiFi yang dipancarkan oleh ESP8266 untuk memantau suhu serta mengubah pengaturan thermostat melalui Web Dashboard lokal.

Project ini dibuat khusus untuk **modul Relay IoT 2 channel berbasis ESP8266** karya [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana).

---

## 🔧 Fitur Utama

- **Mandiri Tanpa Internet (AP Mode):** ESP8266 memancarkan WiFi sendiri, sangat cocok untuk area terpencil atau penggunaan lokal tanpa router tambahan.
- **Captive Portal Otomatis:** Menggunakan DNS Server internal sehingga pengguna dialihkan secara otomatis ke halaman login/dashboard saat terhubung ke jaringan WiFi ESP8266.
- **Web Dashboard Lokal Premium:** Tampilan antarmuka gelap (dark mode) dengan indikator dial melingkar yang dinamis dan kontrol responsif.
- **Mendukung Multi-Sensor:** Kompatibel dengan sensor **DHT11**, **DHT22**, dan **DS18B20** pada pin GPIO yang sama (dapat diubah langsung dari halaman web secara real-time).
- **Mode Kontrol Otomatis & Manual:**
  - **Auto:** Mengontrol cooler dan heater otomatis berdasarkan target suhu dan nilai histeresis yang Anda tetapkan.
  - **Cool:** Mengaktifkan Cooler secara manual.
  - **Heat:** Mengaktifkan Heater secara manual.
  - **Off:** Mematikan semua relay.
- **Penyimpanan EEPROM:** Menyimpan pengaturan terakhir (target suhu, histeresis, mode, jenis sensor) secara otomatis ke memori flash ESP8266 agar tidak hilang ketika daya dimatikan.
- **Dukungan mDNS:** Selain menggunakan IP default `192.168.4.1`, Anda juga dapat mengakses kontrol panel melalui `http://smart-thermostat.local` setelah terhubung.

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
1. Buka file [Smart-Thermostat-AP.ino](Smart-Thermostat-AP.ino) di Arduino IDE.
2. Secara default, nama SSID WiFi yang akan dipancarkan oleh ESP8266 adalah `Smart-Thermostat-AP` tanpa password. Jika Anda ingin mengubah nama SSID atau menambahkan password, cari baris berikut di bagian atas sketch:
   ```cpp
   const char* ap_ssid = "Smart-Thermostat-AP";
   const char* ap_password = ""; // Isi password (minimal 8 karakter) jika ingin dikunci
   ```
3. Lakukan upload kode program ke board ESP8266 Anda (Wemos D1 Mini, NodeMCU, atau ESP-12E/F).

### 2. Mengakses Halaman Control Panel
1. Setelah proses upload selesai, aktifkan WiFi di HP atau Laptop Anda.
2. Cari jaringan WiFi bernama **`Smart-Thermostat-AP`** dan hubungkan perangkat Anda ke WiFi tersebut.
3. Karena fitur **Captive Portal** aktif, sebagian besar perangkat HP akan memunculkan pop-up notifikasi untuk "Masuk ke jaringan (Sign in to network)" yang jika diklik akan langsung membuka Dashboard.
4. Jika tidak terbuka otomatis, buka browser (Chrome, Safari, Firefox), lalu akses alamat IP bawaan AP:  
   👉 **[http://192.168.4.1](http://192.168.4.1)** atau **[http://smart-thermostat.local](http://smart-thermostat.local)**
5. Selamat! Anda sekarang dapat memantau suhu lokal dan mengatur parameter thermostat secara nirkabel secara offline.

> [!NOTE]
> Karena berjalan secara offline/tanpa internet, ikon FontAwesome dan font kustom Plus Jakarta Sans dari CDN internet tidak akan ter-render dengan sempurna kecuali perangkat HP Anda terhubung ke internet seluler secara bersamaan saat mengakses WiFi lokal ini, atau aset-aset tersebut sudah pernah tersimpan di cache memori browser Anda sebelumnya.

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
