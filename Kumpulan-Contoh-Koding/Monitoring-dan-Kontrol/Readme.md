# IoT Relay Dashboard + Monitoring Suhu (Modul Relay IoT Terintegrasi)

Project ini dibuat oleh **Anak Agung Duwi Arsana** dan dirancang khusus untuk dijalankan pada **modul Relay IoT terintegrasi buatan sendiri**, yang sudah memiliki:

- ✅ ESP8266 onboard
- ✅ CP2102 USB to Serial
- ✅ 2x Relay (dengan terminal screw NO-NC)
- ✅ Konektor JST untuk sensor DHT11
- ✅ Input 5V melalui USB Type-C **atau** jack DC barrel

---

## ✨ Fitur

- Kontrol 2 relay dari web (ON/OFF)
- Tampilan suhu secara real-time dengan grafik (Chart.js)
- Dark mode UI yang modern dan responsif
- Status relay tersimpan di EEPROM
- DHT11 plug & play lewat socket JST
- Web UI bisa diakses dari HP atau PC

---

## 🧰 Spesifikasi Modul Relay IoT

| Komponen         | Keterangan                                      |
|------------------|--------------------------------------------------|
| Microcontroller  | ESP8266 onboard                                  |
| USB Interface    | CP2102 (untuk upload dan debugging)              |
| Relay            | 2 channel onboard, aktif LOW, ada terminal screw |
| Sensor Suhu      | Konektor JST 3-pin untuk DHT11                   |
| Input Power      | 5V dari USB Type-C atau konektor DC barrel       |

---

## 🔌 Pin Internal

| Fungsi         | GPIO | Label di ESP8266 | Keterangan     |
|----------------|------|------------------|----------------|
| Relay 1        | 4    | D2               | Aktif LOW      |
| Relay 2        | 5    | D1               | Aktif LOW      |
| DHT11 (Data)   | 12   | D6               | Plug JST       |

---

## 🛠 Cara Pakai

1. Upload sketch menggunakan Arduino IDE
2. Ganti `ssid` dan `password` sesuai WiFi kamu
3. Pasang sensor DHT11 ke JST socket
4. Colok power 5V (USB Type-C atau DC)
5. Buka IP ESP di browser
6. Kontrol dan pantau suhu langsung dari web

---

## 🖼 Tampilan Web

- Tombol ON/OFF untuk masing-masing relay
- Indikator status ON/OFF
- Grafik suhu real-time menggunakan Chart.js
- UI responsive dan dark mode friendly

---

## 💾 EEPROM

Status relay akan tersimpan ke EEPROM setiap kali tombol diklik, dan otomatis diload ulang saat ESP dinyalakan ulang.

---

## 📚 Library

Wajib install dari Library Manager:

- ESP8266WiFi
- ESP8266WebServer
- DHT sensor library
- EEPROM

---

## 🙌 Dukung Saya

💖 Dukung karya ini di [Saweria](https://saweria.co/duwiarsana)  
📺 Subscribe juga ke channel YouTube [Anak Agung Duwi Arsana](https://youtube.com/@anakagungduwiarsana)

---

## 📝 Lisensi

MIT License  
(c) 2025 Anak Agung Duwi Arsana  
[https://duwiarsana.com](https://duwiarsana.com)
