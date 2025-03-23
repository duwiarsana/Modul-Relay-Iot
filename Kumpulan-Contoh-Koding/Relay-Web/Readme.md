# 🌐 Kontrol Relay lewat Web (ESP8266)

Contoh program ini menunjukkan cara mengontrol 2 buah relay menggunakan ESP8266 lewat antarmuka web. Tinggal buka IP dari ESP-nya lewat browser, terus klik tombol ON/OFF buat nyalain atau matiin relay.

Project ini dibuat khusus untuk **modul Relay IoT 2 channel berbasis ESP8266** karya [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana).

---

## 🔧 Fitur

- Kontrol 2 relay lewat WiFi
- Tampilan web responsif dan ringan
- Tanpa refresh halaman (pakai `fetch()` untuk kontrol)
- Print IP ke Serial Monitor
- Bisa diakses dari HP atau laptop di jaringan yang sama

---

## 📚 Library yang Dibutuhkan

- `ESPAsyncWebServer`
- `ESPAsyncTCP`

> Pastikan udah install kedua library itu via Library Manager atau lewat link GitHub-nya.

---

## 🚀 Cara Pakai

1. Ganti `ssid` dan `password` di sketch sesuai jaringan WiFi kamu.
2. Upload ke board ESP8266 (Wemos D1 Mini, NodeMCU, dll).
3. Buka **Serial Monitor**, catat IP address-nya.
4. Akses IP itu lewat browser di HP/laptop.
5. Klik tombol "Relay 1 ON", "Relay 1 OFF", dst.

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
