# MQTT Control & Monitoring Suhu DHT11 via JSON (Modul Relay IoT Terintegrasi)

Project ini dirancang untuk mengontrol 2 channel Relay dan memantau suhu/kelembaban dari sensor DHT11 menggunakan protokol **MQTT** dalam format **JSON** tunggal melalui broker MQTT publik gratis (`broker.hivemq.com`).

---

## ✨ Fitur

- ✅ Membaca data suhu dan kelembaban dari sensor DHT11 secara berkala (setiap 10 detik).
- ✅ Mengemas seluruh data (suhu, kelembaban, status relay 1, status relay 2) ke dalam **satu payload JSON**.
- ✅ Mengirim (publish) payload JSON tersebut ke satu topic MQTT unik berbasiskan Chip ID ESP8266.
- ✅ Menerima (subscribe) perintah kontrol relay dari MQTT untuk menghidupkan (`1` / `ON`) atau mematikan (`0` / `OFF`) relay.
- ✅ Mengirimkan ulang payload data JSON terbaru secara real-time sesaat setelah status relay berubah.

---

## 🔌 Pin Out Modul

| Komponen | GPIO | Label Wemos/NodeMCU | Keterangan |
| :--- | :---: | :---: | :--- |
| **Relay 1** | **4** | D2 | Aktif HIGH |
| **Relay 2** | **5** | D1 | Aktif HIGH |
| **Sensor DHT11** | **12** | D6 | Konektor JST |

---

## 🛠 Cara Penggunaan

### 1. Persiapan Library Arduino IDE
Pastikan Anda sudah menginstal library berikut melalui **Library Manager**:
- **PubSubClient** (oleh Nick O'Leary)
- **DHT sensor library** (oleh Adafruit)
- **Adafruit Unified Sensor** (dependency untuk DHT library)

### 2. Konfigurasi Sketch
Buka file `MQTT-Control.ino` dan sesuaikan parameter WiFi Anda:
```cpp
const char* ssid = "SSID_WIFI_ANDA";       // Ganti dengan SSID WiFi Anda
const char* password = "PASSWORD_WIFI_ANDA"; // Ganti dengan Password WiFi Anda
```

> 💡 **Tip Sensor:** Jika Anda menggunakan **DHT22** (putih) alih-alih **DHT11** (biru), ganti baris ini di kode program:
> ```cpp
> #define DHTTYPE DHT22
> ```

### 3. Upload Program
Hubungkan modul ke komputer via USB, pilih board yang sesuai (misal `NodeMCU 1.0 (ESP-12E Module)`), tentukan port serial, lalu klik **Upload**.

### 4. Monitor Serial
Buka Serial Monitor dengan baud rate **115200**. Setelah terhubung, modul akan menampilkan Client ID unik dan daftar Topic yang digunakan.

Contoh output Serial Monitor:
```text
--- MQTT Relay & DHT11 Module (JSON) ---
Unique MQTT Client ID: ESP8266Client-5671922
Daftar topic yang digunakan:
  - Output Data (JSON): modul-relay-iot/5671922/data
  - Kontrol Relay 1 (Kirim ON/OFF atau 1/0): modul-relay-iot/5671922/relay1/cmd
  - Kontrol Relay 2 (Kirim ON/OFF atau 1/0): modul-relay-iot/5671922/relay2/cmd
----------------------------------------
```

---

## 📡 Panduan Komunikasi MQTT (Subscribe & Kontrol)

> ⚠️ **PENTING:** Ganti `5671922` pada contoh di bawah dengan Chip ID unik modul Anda.

### 1. Struktur Topic & Format JSON

| Aksi | Nama Topic | Payload / Isi Pesan | Keterangan |
| :--- | :--- | :--- | :--- |
| **Subscribe** (Menerima Data) | `modul-relay-iot/5671922/data` | JSON (Lihat format di bawah) | Dikirim berkala setiap 10 detik atau instan saat relay berubah status |
| **Publish** (Kontrol Relay 1) | `modul-relay-iot/5671922/relay1/cmd` | `1` / `ON` (Menyala) atau `0` / `OFF` (Mati) | Perintah untuk mengaktifkan/mematikan Relay 1 |
| **Publish** (Kontrol Relay 2) | `modul-relay-iot/5671922/relay2/cmd` | `1` / `ON` (Menyala) or `0` / `OFF` (Mati) | Perintah untuk mengaktifkan/mematikan Relay 2 |

#### Format Payload JSON yang Diterima (`.../data`):
```json
{
  "suhu": 29.50,
  "kelembaban": 65.00,
  "relay1": 0,
  "relay2": 1
}
```
*(Keterangan nilai `relay1` dan `relay2`: `0` artinya MATI, `1` artinya HIDUP).*

---

### 2. Cara Subscribe Data menggunakan Mosquitto Client
Jalankan perintah ini di Terminal / Command Prompt untuk memantau data JSON dari modul Anda:

```bash
mosquitto_sub -h broker.hivemq.com -p 1883 -t "modul-relay-iot/5671922/data" -v
```

### 3. Cara Mengontrol Relay menggunakan Mosquitto Client
Jalankan perintah publish ini pada jendela terminal baru:

* **Menyalakan Relay 1:**
  ```bash
  mosquitto_pub -h broker.hivemq.com -p 1883 -t "modul-relay-iot/5671922/relay1/cmd" -m "1"
  ```
* **Mematikan Relay 1:**
  ```bash
  mosquitto_pub -h broker.hivemq.com -p 1883 -t "modul-relay-iot/5671922/relay1/cmd" -m "0"
  ```
* **Menyalakan Relay 2:**
  ```bash
  mosquitto_pub -h broker.hivemq.com -p 1883 -t "modul-relay-iot/5671922/relay2/cmd" -m "1"
  ```
* **Mematikan Relay 2:**
  ```bash
  mosquitto_pub -h broker.hivemq.com -p 1883 -t "modul-relay-iot/5671922/relay2/cmd" -m "0"
  ```

---

## 📲 Cara Menguji menggunakan MQTT Explorer (Aplikasi GUI PC)
1. Buka [MQTT Explorer](http://mqtt-explorer.com/).
2. Hubungkan ke Broker: **Host:** `broker.hivemq.com`, **Port:** `1883`.
3. Di panel sebelah kiri, temukan topik `modul-relay-iot` -> `5671922` -> `data`.
4. Anda akan melihat payload JSON berisi data suhu, kelembaban, dan status relay.
5. Untuk kontrol, gunakan kolom **Publish** di sebelah kanan bawah:
   * **Topic:** `modul-relay-iot/5671922/relay1/cmd`
   * **Payload:** Pilih `Raw` lalu isi `1` atau `0`.
   * Klik **Publish**.

---

## 🙌 Dukung Saya

💖 Dukung karya ini di [Saweria](https://saweria.co/duwiarsana)  
📺 Subscribe juga ke channel YouTube [Anak Agung Duwi Arsana](https://youtube.com/@anakagungduwiarsana)

---

## 📝 Lisensi

MIT License  
(c) 2026 Anak Agung Duwi Arsana  
[https://duwiarsana.com](https://duwiarsana.com)
