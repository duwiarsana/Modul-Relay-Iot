# 📊 ESP8266 DHT11 Data Logger ke Google Sheets (Real-time & 24-Hour Filter)

Contoh program ini menunjukkan cara mencatat (logging) data sensor **DHT11** (Suhu & Kelembaban) ke **Google Sheets** menggunakan **ESP8266** secara berkala. Melalui bantuan **Google Apps Script**, data dikirimkan menggunakan protokol HTTPS yang aman dan otomatis tercatat di baris baru Google Sheets.

Selain itu, project ini juga menyertakan formula khusus di Google Sheets untuk membuat dashboard terpisah yang **hanya menampilkan data 24 jam terakhir secara real-time**, sementara data history lama tetap tersimpan dengan aman.

Project ini dibuat khusus untuk **modul Relay IoT 2 channel berbasis ESP8266** karya [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana).

---

## 🔧 Fitur

- **Data Logging Otomatis:** Mencatat suhu (°C) dan kelembaban (%) ke Google Sheets dengan stempel waktu (timestamp) otomatis.
- **Filter Data 24 Jam Terakhir:** Menampilkan visualisasi data 24 jam terakhir secara dinamis tanpa menghapus data log utama.
- **Koneksi HTTPS Aman & Bebas Perawatan:** Menggunakan HTTPS melalui `WiFiClientSecure` mode `insecure` sehingga Anda tidak perlu memelihara sertifikat SSL root yang bisa kedaluwarsa.
- **Penanganan Redirect (302 Redirect):** Menggunakan fitur `setFollowRedirects` bawaan ESP8266 HTTP Client untuk menangani pengalihan link Google Apps Script dengan sempurna.

---

## 📚 Library yang Dibutuhkan

Sebelum melakukan upload, pastikan Anda telah menginstal library berikut melalui **Library Manager** di Arduino IDE:

1. **DHT sensor library** (oleh Adafruit)
2. **Adafruit Unified Sensor** (oleh Adafruit - sebagai dependensi library DHT)

---

## 🚀 Panduan Setup Langkah Demi Langkah

### Langkah 1: Setup Google Sheets & Google Apps Script
1. Buat spreadsheet baru di Google Drive Anda. Beri nama, misalnya `Data Logger IoT`.
2. Pada sheet pertama (default namanya `Sheet1`), buatlah header pada baris pertama:
   - Kolom **A**: `Timestamp`
   - Kolom **B**: `Suhu (*C)`
   - Kolom **C**: `Kelembaban (%)`
3. Klik menu **Ekstensi (Extensions)** -> **Apps Script**.
4. Hapus semua kode default di editor, lalu salin dan tempel kode Google Apps Script di bawah ini:

```javascript
function doGet(e) {
  // Membuka spreadsheet aktif dan mengambil sheet pertama (index 0)
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheets()[0];
  
  // Membaca parameter query GET (?temp=XX.X&humi=YY.Y)
  var temp = e.parameter.temp;
  var humi = e.parameter.humi;
  
  // Validasi parameter agar tidak menulis baris kosong
  if (temp !== undefined && humi !== undefined) {
    // Mendapatkan waktu saat ini
    var now = new Date();
    
    // Menambahkan baris baru berisi: Waktu, Suhu, Kelembaban
    sheet.appendRow([now, temp, humi]);
    
    return ContentService.createTextOutput("SUCCESS");
  } else {
    return ContentService.createTextOutput("ERROR: Missing parameters");
  }
}
```

5. Klik tombol **Simpan** (ikon disket).
6. Klik tombol **Terapkan (Deploy)** di pojok kanan atas -> pilih **Terapkan baru (New deployment)**.
7. Klik ikon gerigi (Pilih jenis/Select type) -> pilih **Aplikasi web (Web app)**.
8. Konfigurasikan sebagai berikut:
   - **Deskripsi:** `Logging DHT11`
   - **Jalankan sebagai (Execute as):** `Saya (Your Email)`
   - **Siapa yang memiliki akses (Who has access):** `Siapa saja (Anyone)`  *(Ini wajib agar ESP8266 bisa mengirim data tanpa login akun Google)*.
9. Klik **Terapkan (Deploy)**.
10. Jika muncul permintaan izin otorisasi (Authorize Access), klik dan setujui menggunakan akun Google Anda (klik *Advanced* -> *Go to ... (unsafe)* jika ada peringatan keamanan).
11. Setelah berhasil, Anda akan mendapatkan **URL Aplikasi Web (Web App URL)**.  
    Contoh: `https://script.google.com/macros/s/AKfycbz...XXXX/exec`  
    **Salin URL ini**, Anda akan membutuhkannya untuk dimasukkan ke koding Arduino.

---

### Langkah 2: Setup Filter 24 Jam Terakhir di Google Sheets
Untuk memisahkan data lengkap dengan data 24 jam terakhir secara dinamis:
1. Kembali ke Google Sheets Anda.
2. Buat sheet/tab baru dengan menekan tombol **+ (Tambah Sheet)** di bagian bawah kiri. Beri nama sheet baru ini **Dashboard**.
3. Di sheet **Dashboard** baru tersebut, klik pada sel **A1**, lalu masukkan formula berikut:

```excel
=SORT(FILTER(Sheet1!A:C, Sheet1!A:A >= NOW() - 1), 1, FALSE)
```

> [!TIP]
> **Cara Kerja Formula:**
> - `Sheet1!A:C` mengambil semua data dari sheet utama.
> - `Sheet1!A:A >= NOW() - 1` memfilter data di mana kolom waktu lebih besar atau sama dengan waktu saat ini (`NOW()`) dikurangi 1 hari (`1` unit hari dalam format serial Google Sheets = 24 jam).
> - `SORT(..., 1, FALSE)` mengurutkan hasil filter berdasarkan kolom pertama (Timestamp) secara menurun (`FALSE` = Descending/Terbaru di atas), sehingga data paling baru selalu muncul paling atas.

---

### Langkah 3: Konfigurasi Sketch Arduino
1. Buka file [Data-Logger-Spreadsheet.ino](Data-Logger-Spreadsheet.ino) di Arduino IDE.
2. Konfigurasikan detail WiFi Anda:
   ```cpp
   const char* ssid = "YOUR_SSID";          // Nama WiFi Anda
   const char* password = "YOUR_PASSWORD";  // Password WiFi Anda
   ```
3. Ganti konstanta `gasWebUrl` dengan URL Aplikasi Web (Web App URL) dari Google Apps Script yang sudah Anda salin di Langkah 1:
   ```cpp
   const char* gasWebUrl = "https://script.google.com/macros/s/AKfycbz...XXXX/exec";
   ```
4. Lakukan upload kode program ke board ESP8266 Anda.
5. Buka **Serial Monitor** pada baudrate `115200` untuk memantau status koneksi dan proses pengiriman data.

---

## 📌 Skema Sambungan Modul

| Komponen | Pin Modul | Pin GPIO ESP8266 |
| :--- | :--- | :--- |
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
