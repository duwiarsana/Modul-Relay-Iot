# ⚡ ESP-NOW Remote Control (Kontrol Nirkabel Offline Tanpa Router)

Contoh koding ini menunjukkan cara mengontrol 2-channel relay secara nirkabel langsung dari modul ESP8266 lain (Transmitter/Remote) ke modul IoT Relay (Receiver) menggunakan protokol bawaan ESP-NOW.

Protokol **ESP-NOW** dikembangkan oleh Espressif, memungkinkan komunikasi cepat, berlatensi rendah (<10ms), dan hemat daya antar modul tanpa memerlukan router WiFi.

---

## 🛠️ Persiapan Perangkat

Untuk proyek ini, Anda memerlukan **2 unit modul ESP8266**:

1. **Modul Receiver (IoT Relay Board)**: Modul relay utama yang akan Anda kontrol.
2. **Modul Transmitter (Remote)**: Modul ESP8266 tambahan (seperti Wemos D1 Mini / NodeMCU) sebagai pemancar sinyal kontrol.

---

## 🧩 Skema Kabel Tombol (Transmitter)

Sambungkan tombol tipe push-button pada modul **Transmitter** ke pin sebagai berikut:

* **Tombol 1 (Kontrol Relay 1)**: Hubungkan kaki tombol satu ke pin **D3 (GPIO 0)** dan kaki lainnya ke **GND**.
* **Tombol 2 (Kontrol Relay 2)**: Hubungkan kaki tombol dua ke pin **D4 (GPIO 2)** dan kaki lainnya ke **GND**.

*(Karena kita mengaktifkan internal pull-up pada pin tombol, tidak diperlukan resistor eksternal).*

---

## 💻 Langkah Penggunaan

### 1. Dapatkan MAC Address Modul Receiver (Modul IoT Relay)
1. Buka file `ESP-NOW-Receiver/ESP-NOW-Receiver.ino` menggunakan Arduino IDE.
2. Upload sketch tersebut ke modul **IoT Relay (Receiver)** Anda.
3. Buka **Serial Monitor** pada baudrate **115200**.
4. Cari baris output:
   `PENTING! MAC Address Receiver Anda: XX:XX:XX:XX:XX:XX`
5. Salin 6 pasang kode hex tersebut.

### 2. Konfigurasi Modul Transmitter (Remote)
1. Buka file `ESP-NOW-Transmitter/ESP-NOW-Transmitter.ino` menggunakan Arduino IDE.
2. Temukan baris kode berikut di bagian atas program:
   ```cpp
   uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
   ```
3. Ganti nilai di dalam kurung kurawal `{ ... }` dengan kode hex MAC Address milik Receiver yang Anda dapatkan di langkah pertama.
   * *Contoh jika MAC Address receiver adalah `4A:3F:8B:11:22:33`, ubah kodenya menjadi:*
   ```cpp
   uint8_t receiverAddress[] = {0x4A, 0x3F, 0x8B, 0x11, 0x22, 0x33};
   ```
4. Upload sketch ke modul **Transmitter (Remote)** Anda.

---

## 🔬 Cara Pengujian

1. Nyalakan kedua modul ESP8266 (bisa ditenagai pakai kabel USB charger / power bank biasa).
2. Tekan tombol 1 pada modul **Transmitter**. Relay 1 pada modul **Receiver** akan berubah keadaan (ON -> OFF atau OFF -> ON) secara instan.
3. Tekan tombol 2 untuk mengontrol Relay 2.
4. Anda dapat melihat log debug pada Serial Monitor masing-masing modul untuk memantau status pengiriman data dan status penerimaan perintah.
