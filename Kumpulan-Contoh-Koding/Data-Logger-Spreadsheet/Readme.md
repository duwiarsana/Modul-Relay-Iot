# 📊 ESP8266 DHT11 Data Logger & Kontrol 2 Relay via Google Sheets

Contoh program ini menunjukkan cara membuat sistem IoT dua arah menggunakan **ESP8266** dengan **Google Sheets** sebagai database dan panel kendalinya. Sistem ini melakukan dua fungsi utama secara bersamaan:
1. **Data Logging (ESP8266 ➡️ Google Sheets):** Mengirimkan data suhu (°C) dan kelembaban (%) dari sensor **DHT11** untuk dicatat di spreadsheet secara berkala.
2. **Relay Control (Google Sheets ➡️ ESP8266):** Membaca status tombol kendali di spreadsheet untuk menyalakan/mematikan **Relay 1** dan **Relay 2** secara real-time.

Project ini dibuat khusus untuk **modul Relay IoT 2 channel berbasis ESP8266** karya [Duwi Arsana](https://youtube.com/@AnakAgungDuwiArsana).

---

## 🔧 Fitur Utama

- **Sistem Dua Arah (Two-Way IoT):** Mengirim data sensor sekaligus menerima perintah kontrol relay dalam satu request HTTP GET yang sangat efisien.
- **Filter Data 24 Jam Terakhir:** Rumus khusus untuk membuat dashboard real-time yang hanya menampilkan riwayat suhu 24 jam terakhir agar spreadsheet tetap rapi.
- **Kontrol Relay dari Sel Spreadsheet:** Kendalikan perangkat listrik rumah Anda cukup dengan mengetik angka `1` (ON) atau `0` (OFF) pada kolom spreadsheet.
- **Koneksi HTTPS Aman & Bebas Perawatan:** Menggunakan `WiFiClientSecure` mode `insecure` sehingga tidak memerlukan pemeliharaan sertifikat root SSL Google yang bisa kedaluwarsa.
- **Penanganan Redirect (302 Redirect):** Menggunakan fitur `setFollowRedirects` bawaan ESP8266 HTTP Client untuk menangani pengalihan tautan Apps Script secara sempurna.

---

## 📚 Library yang Dibutuhkan

Sebelum melakukan upload, pastikan Anda telah menginstal library berikut melalui **Library Manager** di Arduino IDE:

1. **DHT sensor library** (oleh Adafruit)
2. **Adafruit Unified Sensor** (oleh Adafruit - sebagai dependensi library DHT)

---

## 🚀 Panduan Setup Langkah Demi Langkah

### Langkah 1: Setup Google Sheets & Google Apps Script
1. Buat spreadsheet baru di Google Drive Anda. Beri nama, misalnya `Data Logger & Kontrol IoT`.
2. **Tab Pertama (Logging Data):**
   - Pastikan nama tab pertama adalah `Sheet1`.
   - Buat header pada baris pertama:
     - Kolom **A**: `Timestamp`
     - Kolom **B**: `Suhu (*C)`
     - Kolom **C**: `Kelembaban (%)`
3. **Tab Kedua (Kontrol Relay):**
   - Buat tab/sheet baru dengan menekan tombol **+ (Tambah Sheet)** di bagian kiri bawah, beri nama sheet tersebut **Control** (case-sensitive).
   - Di tab **Control**, buat header berikut:
     - Kolom **A1**: `Relay 1`
     - Kolom **B1**: `Relay 2`
   - Isi baris kedua dengan nilai awal kontrol (mati):
     - Kolom **A2**: `0` (atau isi `1` untuk ON)
     - Kolom **B2**: `0` (atau isi `1` untuk ON)
4. Klik menu **Ekstensi (Extensions)** -> **Apps Script**.
5. Hapus semua kode default di editor, lalu salin dan tempel kode Google Apps Script di bawah ini:

```javascript
function doGet(e) {
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var logSheet = ss.getSheets()[0]; // Mengambil sheet pertama (Sheet1) untuk menyimpan log sensor
  
  // Membaca parameter query GET (?temp=XX.X&humi=YY.Y) dari ESP8266
  var temp = e.parameter.temp;
  var humi = e.parameter.humi;
  
  // Jika ESP8266 mengirimkan data sensor, catat ke sheet log
  if (temp !== undefined && humi !== undefined) {
    var now = new Date();
    logSheet.appendRow([now, temp, humi]);
  }
  
  // Membaca status relay dari sheet kedua bernama "Control"
  var controlSheet = ss.getSheetByName("Control");
  var relay1Status = controlSheet.getRange("A2").getValue(); // Membaca sel A2
  var relay2Status = controlSheet.getRange("B2").getValue(); // Membaca sel B2
  
  // Jika sel kosong, default-kan ke 0
  if (relay1Status === "") relay1Status = 0;
  if (relay2Status === "") relay2Status = 0;
  
  // Mengirim balasan teks sederhana "relay1,relay2" (contoh: "1,0") ke ESP8266
  var responseText = relay1Status + "," + relay2Status;
  return ContentService.createTextOutput(responseText);
}
```

6. Klik tombol **Simpan** (ikon disket).
7. Klik tombol **Terapkan (Deploy)** di pojok kanan atas -> pilih **Terapkan baru (New deployment)**.
8. Klik ikon gerigi (Pilih jenis/Select type) -> pilih **Aplikasi web (Web app)**.
9. Konfigurasikan sebagai berikut:
   - **Deskripsi:** `Logging & Kontrol Relay`
   - **Jalankan sebagai (Execute as):** `Saya (Your Email)`
   - **Siapa yang memiliki akses (Who has access):** `Siapa saja (Anyone)`  *(Ini wajib agar ESP8266 bisa terhubung tanpa login akun Google)*.
10. Klik **Terapkan (Deploy)**. Setujui permintaan otorisasi akun Google Anda.
11. Salin **URL Aplikasi Web (Web App URL)** yang Anda dapatkan (Contoh: `https://script.google.com/macros/s/AKfycbz...XXXX/exec`).

---

### Langkah 2: Setup Filter 24 Jam Terakhir di Google Sheets
Untuk memvisualisasikan data suhu terbaru tanpa menghapus history lama:
1. Di Google Sheets Anda, tambahkan sheet baru lagi dengan menekan tombol **+**, beri nama sheet tersebut **Dashboard**.
2. Klik sel **A1** di sheet **Dashboard**, lalu masukkan formula berikut:

```excel
=SORT(FILTER(Sheet1!A:C, Sheet1!A:A >= NOW() - 1), 1, FALSE)
```

> [!TIP]
> Formula ini akan secara otomatis memfilter data dari `Sheet1` yang berumur kurang dari 24 jam (`NOW() - 1`) dan mengurutkannya dari yang paling baru di baris teratas.

---

### Langkah 3: Konfigurasi & Upload Sketch Arduino
1. Buka file [Data-Logger-Spreadsheet.ino](Data-Logger-Spreadsheet.ino) di Arduino IDE.
2. Konfigurasikan SSID dan Password WiFi Anda:
   ```cpp
   const char* ssid = "YOUR_SSID";          // Nama WiFi Anda
   const char* password = "YOUR_PASSWORD";  // Password WiFi Anda
   ```
3. Masukkan URL Apps Script Web App yang sudah disalin ke variabel `gasWebUrl`:
   ```cpp
   const char* gasWebUrl = "https://script.google.com/macros/s/AKfycbz...XXXX/exec";
   ```
4. Lakukan upload kode program ke board ESP8266 Anda.
5. Buka **Serial Monitor** pada baudrate `115200` untuk memantau aktivitas.

---

## 📌 Skema Sambungan Modul

| Komponen | Pin Modul | Pin GPIO ESP8266 |
| :--- | :--- | :--- |
| **Relay 1 (Cooler/Fan)** | D2 | GPIO 4 |
| **Relay 2 (Heater/Pemanas)** | D1 | GPIO 5 |
| **Sensor DHT11** | D6 (JST Connector) | GPIO 12 |

---

## 🎮 Cara Mengontrol Relay dari Spreadsheet
- Buka spreadsheet Anda, lalu buka tab **Control**.
- Untuk menyalakan **Relay 1**, ubah angka di sel **A2** menjadi `1`. Untuk mematikan, ubah menjadi `0`.
- Untuk menyalakan **Relay 2**, ubah angka di sel **B2** menjadi `1`. Untuk mematikan, ubah menjadi `0`.
- ESP8266 melakukan query setiap **15 detik**, sehingga dalam maksimal 15 detik setelah Anda menekan enter di spreadsheet, status relay fisik pada modul IoT Anda akan langsung merespon dan berubah.

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
