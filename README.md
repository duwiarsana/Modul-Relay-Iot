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

| Fungsi       | GPIO ESP8266 |
|--------------|---------------|
| Relay 1      | GPIO 4 (D2)   |
| Relay 2      | GPIO 5 (D1)   |
| Sensor DHT11 | GPIO 14 (D5)  |

> Pastikan GPIO yang digunakan di sketch sesuai dengan pinout ini agar modul berjalan dengan benar.

## 🔧 Instalasi Firmware

1. Pasang ESP8266 ke board.
2. Hubungkan ke komputer via USB to Serial.
3. Buka Arduino IDE atau PlatformIO.
4. Upload firmware sesuai kebutuhan kamu.

Contoh firmware akan disediakan di folder `/Contoh coding`.

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

## 📁 Folder `Kumpulan Contoh Koding`

Berisi beberapa sketch contoh yang bisa kamu langsung flash ke ESP8266 untuk:
- Mengontrol relay via WiFi
- Membaca data DHT11
- Kombinasi relay + suhu
- dan lainnya (akan terus bertambah)

---

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
