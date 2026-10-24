# Catatan Perubahan Firmware MRC_LF

Dokumen ini mencatat seluruh perubahan dari kode awal sampai versi sekarang:
apa yang diubah, kenapa diubah, masalah apa yang diselesaikan, dan apa fungsi
setiap bagian kode.

- Kode awal: `config.h` 623 baris, `main.ino` 92 baris, `track.h` 724 baris
  (total 1.439 baris), nama variabel campur Indonesia/Inggris, banyak dead code.
- Versi sekarang: `config.h` 519, `control_math.h` 84, `main.ino` 63,
  `track.h` 359 (total 1.025 baris firmware) + 87 baris test.
- Angka jarak/kecepatan di lintasan (`runCourse()`) **tidak diubah**.

---

## 1. Struktur file

| File | Isi |
|---|---|
| `main.ino` | `setup()`, `runMenu()` (menu tombol), `loop()` |
| `config.h` | pin, konstanta, kalibrasi, pembacaan sensor, kontrol motor, line follower, EEPROM |
| `control_math.h` | fungsi matematika murni (normalisasi, centroid, deadband, filter), bisa dites di PC |
| `track.h` | `runCourse()` — urutan gerakan lintasan |
| `tests/test_control_math.cpp` | test host untuk `control_math.h` |

Catatan riwayat: commit `fa28c33 init` di repo git adalah versi **setelah**
refactor tahap 1. Versi awal (nama Indonesia) tidak tersimpan di git; rujukan
di dokumen ini berasal dari kode awal sesi.

---

## 2. Tiga tahap perubahan

1. **Audit** — membaca seluruh kode, memetakan alur, menemukan 13 masalah.
2. **Refactor + perbaikan** — rename ke bahasa Inggris, hapus dead code,
   perbaiki bug kalibrasi/EEPROM/menu/kontroler.
3. **Peningkatan stabilitas** — centroid analog, filter suku D, deadband motor,
   recovery garis hilang, debounce persimpangan, koreksi arah steering.

---

## 3. Masalah yang ditemukan dan diselesaikan

| # | Masalah | Gejala di lapangan | Solusi |
|---|---|---|---|
| 1 | Threshold kalibrasi bukan titik tengah | `(bg+garis)/2 + garis` membuat ambang terlalu dekat background; garis pudar terbaca sebagai garis, robot menempel ke satu sisi | `threshold = (min + max) / 2` |
| 2 | Kalibrasi disimpan tapi tidak pernah dibaca | Setiap power cycle kembali ke tabel default; kalibrasi hilang saat baterai dicabut | `loadCalibration()` dipanggil di `initHardware()` + marker validasi |
| 3 | Switch 13 pola, `pv` basi | Pola lain (garis hilang, persimpangan penuh, 3 sensor) tidak cocok, robot memakai error lama | Centroid dari semua sensor |
| 4 | Motor tetap berjalan saat kalibrasi | Tekan T1 saat mode test masih jalan, robot melaju selama sampling | `stopMotors()` sebelum `calibrate()` |
| 5 | Empat helper drive bertentangan dengan `driveMotors()` | `forward()` memutar roda kanan ke arah sebaliknya; `right()`/`left()` justru lurus | Helper dihapus, satu konvensi di `driveMotors()` |
| 6 | `BT()` tidak pernah dipanggil | Perintah serial/Bluetooth `F/B/U/D/P` tidak bereaksi | Dihapus |
| 7 | Counter tanpa timeout | Garis terlewat → robot berputar sampai baterai habis | Timeout 10 detik di `waitCrossings()` |
| 8 | Tidak ada test | Perubahan kontroler hanya bisa dicek dengan menjalankan robot | Test host untuk matematika kontrol |
| 9 | Dead code ±500 baris | Sulit membedakan kode hidup dan mati; bug polaritas tersembunyi | Dihapus |
| 10 | Alur menu mati, tombol Copy tidak bekerja | `while(1)` yang hanya sekali, `if` setelah `goto`, Copy hanya menampilkan teks | Alur dirapikan, Copy mengembalikan dan menyimpan default |
| 11 | `copy()` menunda 400 ms saat boot | `delay(50)` di dalam loop 8 sensor | Delay dihapus |
| 12 | `convertLogic()` menyimpan bit lama saat nilai sama persis | Satu sensor bisa basi satu siklus | Diganti perbandingan langsung `raw > threshold` |
| 13 | Kode "motor mati" ditulis tiga kali | `Stop()`, `rem()`, dan cabang nol `motor()` harus selalu sinkron | Semua lewat `driveMotors(0, 0)` |
| 14 | Input sensor hanya 8 bit (biner) | Posisi garis melompat 5 satuan per sensor; tidak ada resolusi sub-sensor | Kekuatan analog 0–1000 per sensor + centroid |
| 15 | Suku D terlalu keras | `KD/4` = 29; satu transisi sensor mengubah koreksi 145 PWM → zig-zag | Suku D difilter, `KD` per sampel = 10 |
| 16 | Deadband motor tidak dikompensasi | PWM kecil hanya menggerakkan satu roda karena gesekan berbeda | `applyDeadband()` di `driveMotors()` |
| 17 | Garis hilang → berputar PWM penuh | Robot berputar di tempat, tidak pernah menemukan garis lagi | Cari pelan ke sisi terakhir garis terlihat |
| 18 | Crossing dihitung dari satu sampel | Garis tebal/reflektif dihitung dua kali atau terlewat | Debounce 3 sampel berturut |
| 19 | Arah steering sempat terbalik saat refactor | Roda kiri/kanan tertukar dibanding `motor(ki, ka)` asli | Dikembalikan ke `left = base − PD`, `right = base + PD` |
| 20 | Nama variabel tidak konsisten | Sulit dibaca dan dirawat | Rename menyeluruh ke bahasa Inggris |
| 21 | Garis putus-putus memicu pivot tiap celah | Robot berputar dan keluar jalur di setiap celah | `followLineDashed()` terpisah: coast, sweep, reacquire debounce |

### Penjelasan masalah utama

**Kalibrasi (1, 2, 14).** Nilai mentah tiap sensor berbeda karena komponen dan
pencahayaan. Kode awal menyimpan ambang yang salah posisi dan tidak pernah
membacanya lagi. Sekarang kalibrasi menyimpan tiga hal per sensor: ambang
(`threshold`), nilai minimum (`sensorMin`, background), dan maksimum
(`sensorMax`, garis). Ketiganya dipakai untuk menormalisasi bacaan menjadi
0–1000, sehingga perbedaan antar-sensor hilang sebelum dihitung.

**Error garis (3, 15, 17).** Kode awal hanya mengenali 13 dari 256 pola sensor.
Pola lain memakai `pv` dari siklus sebelumnya. Ditambah suku D yang besar,
setiap transisi sensor menghasilkan lompatan PWM besar. Sekarang posisi garis
dihitung kontinu dari semua sensor, suku D difilter, dan saat garis hilang
robot mencari pelan ke sisi terakhir, bukan berputar kencang.

**Arah steering (19).** Kode asli: `ki = base − PD`, `ka = base + PD`, lalu
`motor(ki, ka)`. Saat refactor, sempat menjadi `left = base + correction`,
`right = base − correction`, yang membalik arah koreksi. Sudah dikembalikan
dan diverifikasi terhadap kode asli.

**Garis putus-putus (21).** `followLine()` menganggap setiap celah sebagai garis
hilang total dan langsung pivot. `followLineDashed()` mempertahankan koreksi
terakhir selama `DASHED_COAST_MS`, lalu menyapu kiri/kanan; `followLine()` dan
`runCourse()` tidak diubah.

---

## 4. Penjelasan setiap fungsi

### 4.1 `control_math.h` (matematika murni, tanpa Arduino)

**`clampInt(value, low, high)`** (`control_math.h:8`)
- Tujuan: membatasi nilai ke rentang `low..high`.
- Alasan: menggantikan `if` berulang dan mencegah nilai PWM/strength keluar batas.

**`normalizedStrength(raw, minReading, maxReading)`** (`control_math.h:19`)
- Tujuan: mengubah bacaan mentah 0–255 menjadi kekuatan garis 0–1000.
- Cara kerja: `(raw − min) * 1000 / (max − min)`, lalu di-clamp 0–1000.
- Alasan: menyeragamkan respons tiap sensor. Jika kontras `max − min` kurang
  dari `MIN_CONTRAST` (30), hasilnya 0 supaya kalibrasi buruk tidak menghasilkan
  nilai acak.
- Masalah yang diselesaikan: perbedaan sensitivitas antar-sensor dan pembagian
  dengan nol saat kalibrasi gagal.

**`centroidPosition(strength, &total)`** (`control_math.h:28`)
- Tujuan: menghitung posisi garis dari kekuatan semua sensor.
- Cara kerja: rata-rata berbobot indeks sensor (0–7), diskalakan ke −30…+30
  (sensor 0 = +30, sensor 7 = −30). `total` adalah jumlah seluruh kekuatan,
  dipakai untuk mendeteksi garis hilang.
- Alasan: menggantikan switch 13 pola. Rumus ini menangani semua 256 pola dan
  memberi resolusi sub-sensor.
- Masalah yang diselesaikan: `pv` basi, lompatan 5 satuan, pola persimpangan.

**`applyDeadband(pwm, deadband)`** (`control_math.h:42`)
- Tujuan: menaikkan PWM kecil ke nilai minimum agar motor benar-benar bergerak.
- Cara kerja: nilai 1…`deadband−1` menjadi `deadband`; negatif simetris; 0 tetap 0.
- Alasan: dua motor punya gesekan berbeda; PWM kecil bisa menggerakkan satu roda
  saja dan membuat robot menikung tanpa perintah.
- Masalah yang diselesaikan: koreksi kecil yang tidak simetris.

**`lowPassStep(previous, current, alphaPercent)`** (`control_math.h:52`)
- Tujuan: filter low-pass orde satu untuk sinyal turunan.
- Cara kerja: `(alpha * current + (100 − alpha) * previous) / 100`.
- Alasan: input sensor berubah diskrit; tanpa filter, suku D melonjak.
- Masalah yang diselesaikan: overshoot dan getaran akibat derivative spike.

**`lineSeen(strength, wasVisible, enterStrength, releaseStrength)`** (`control_math.h:60`)
- Tujuan: hysteresis status garis.
- Cara kerja: jika sebelumnya terlihat, cukup `>= releaseStrength`; jika belum,
  harus `>= enterStrength`.
- Alasan: mencegah status garis berkedip di sekitar satu ambang.
- Masalah yang diselesaikan: coast/reacquire berulang saat kekuatan garis marginal.

**`gapPhase(hiddenMs, coastMs, searchMs)`** (`control_math.h:65`)
- Tujuan: menentukan fase celah: coast, search, atau lost.
- Cara kerja: `hiddenMs < coastMs` → coast; `< coastMs + searchMs` → search;
  selebihnya lost.
- Alasan: batas waktu eksplisit, bukan jumlah sampel yang bergantung timing.
- Masalah yang diselesaikan: keputusan garis hilang yang tidak konsisten.

**`sweepDirection(hiddenMs, coastMs, sweepMs)`** (`control_math.h:77`)
- Tujuan: arah sapuan +1/−1 bergantian selama fase search.
- Cara kerja: indeks `(hiddenMs − coastMs) / sweepMs`, genap +1, ganjil −1.
- Alasan: sapuan bolak-balik tanpa state tambahan.
- Masalah yang diselesaikan: robot hanya menyapu ke satu arah.

### 4.2 `config.h` — hardware dan tampilan

**`selectMuxChannel(a, b, c)`** (`config.h:96`)
- Tujuan: memilih kanal 0–7 pada multiplexer 4051 lewat pin A0/A1/A2.
- Perubahan: nama dari `selector()`, pin dari `A/B/C` menjadi `MUX_SELECT_A/B/C`.

**`readSensors()`** (`config.h:102`)
- Tujuan: membaca 8 sensor ke `rawSensor[0..7]` (kanal 7 dibaca lebih dulu).
- Perubahan: nama dari `readSelector()`, tetap `analogRead(...) / 4` (0–255).
- Catatan: urutan pembacaan dipertahankan agar timing tidak berubah.

**`displayText(size, x, y, text, refresh, clear)`** (`config.h:121`)
- Tujuan: menulis teks ke OLED, dengan opsi hapus layar dan refresh.
- Perubahan: nama dari `lcd_char()`, parameter `String` diganti `const char *`
  supaya tidak membuat alokasi String di AVR.

**`drawProgressBar(x, y, w, h, percent)`** (`config.h:135`)
- Tujuan: progress bar kalibrasi.
- Perubahan: hanya nama (dari `drawProgressbar`).

### 4.3 `config.h` — kalibrasi dan EEPROM

**`saveCalibration()`** (`config.h:141`)
- Tujuan: menyimpan `threshold[8]`, `sensorMin[8]`, `sensorMax[8]`, lalu marker
  `0x5A5B` ke EEPROM.
- Perubahan: dulu hanya menyimpan 3 array tanpa marker, dan `sensorMin/Max`
  tidak pernah disimpan karena tidak dipakai.
- Layout: 0–15 threshold, 16–31 min, 32–47 max, 48–49 marker (50 byte).

**`loadCalibration()`** (`config.h:158`)
- Tujuan: membaca kalibrasi dari EEPROM saat boot.
- Cara kerja: baca ke variabel lokal, cek marker, baru salin ke variabel global.
- Alasan: EEPROM kosong bernilai `0xFF`. Tanpa marker, ambang menjadi −1 dan
  semua sensor dianggap melihat garis. Marker baru juga menolak layout lama.
- Masalah yang diselesaikan: kalibrasi hilang setelah power cycle; data sampah
  saat pertama kali flashing.

**`loadDefaultCalibration()`** (`config.h:191`)
- Tujuan: memakai tabel `DEFAULT_THRESHOLDS`, `sensorMin = 0`, `sensorMax = 255`.
- Alasan: fallback yang aman sebelum pengguna melakukan kalibrasi.
- Dipakai oleh `initHardware()` dan tombol Copy.

**`calibrate(samples)`** (`config.h:199`)
- Tujuan: mengambil sampel beberapa kali, mencatat min/max tiap sensor,
  menghitung ambang, lalu menyimpan.
- Perubahan: rumus ambang diperbaiki menjadi `(min + max) / 2`.
- Guard: jika kontras satu sensor < `MIN_CONTRAST`, nilai lama dipertahankan,
  sehingga satu sensor yang tidak sempat melewati garis tidak merusak kalibrasi.
- Masalah yang diselesaikan: ambang off-centre dan kalibrasi setengah jadi.

### 4.4 `config.h` — motor dan aktuator

**`driveMotors(leftPwm, rightPwm)`** (`config.h:234`)
- Tujuan: satu-satunya tempat yang menulis ke pin motor.
- Cara kerja: terapkan deadband, lalu tentukan arah berdasarkan tanda PWM.
- Perubahan: menggabungkan `motor()`, `right()`, `left()`, `forward()`,
  `backward()` dan `Stop()`/`rem()`.
- Alasan: kode lama punya dua konvensi tanda yang berbeda; `forward()` memutar
  roda kanan ke arah sebaliknya. Satu fungsi menghilangkan kemungkinan itu.
- Masalah yang diselesaikan: polaritas tidak konsisten dan duplikasi.

**`brake(durationMs)`** (`config.h:261`)
- Tujuan: matikan motor lalu tunggu. Nama dari `rem()`. Memakai `driveMotors(0,0)`.

**`stopMotors()`** (`config.h:266`)
- Tujuan: matikan motor dan tutup gripper. Nama dari `Stop()`.
- Alasan: dipakai menu, timeout, dan sebelum kalibrasi.

**`beep(times, durationMs)`** (`config.h:271`)
- Tujuan: pola bunyi buzzer. Nama dari `bip()`.

**`liftUp(stepDelay)` / `liftDown(stepDelay)`** (`config.h:444`, `451`)
- Tujuan: menggerakkan servo lift dari `LIFT_DOWN` ke `LIFT_UP` atau sebaliknya.
- Perubahan: nama dari `naik()` / `turun()`, parameter diberi nama jelas.
- Catatan: tetap blocking; angka servo tidak berubah.

### 4.5 `config.h` — line following

**`updateLineState()`** (`config.h:280`)
- Tujuan: satu kali baca sensor menghasilkan semua state kontrol:
  `rawSensor`, `sensorOnLine`, `sensorMask`, `linePosition`, `lineStrength`.
- Perubahan: menggabungkan `convertLogic()` + `logic()`.
- Cara kerja: kekuatan analog dihitung per sensor; mask biner tetap memakai
  `raw > threshold` supaya arti crossing tidak berubah; posisi dari centroid.
- Masalah yang diselesaikan: informasi sensor hilang karena threshold terlalu
  awal, dan error basi pada pola yang tidak dikenal.

**`followLine(baseSpeed)`** (`config.h:293`)
- Tujuan: kontroler utama saat mengikuti garis.
- Urutan:
  1. `updateLineState()`.
  2. Jika `lineStrength < MIN_LINE_STRENGTH`: cari pelan ke sisi terakhir.
  3. Hitung `error = SET_POINT − linePosition`.
  4. Hitung turunan, filter, lalu `correction = KP*error + KD*filtered`.
  5. Batasi koreksi dan PWM tiap roda.
  6. Jika posisi mentok di ujung, pivot dengan `PIVOT_SPEED` (bukan PWM penuh).
- Perubahan dari `runn()`:
  - `left = base − correction`, `right = base + correction` (sama dengan
    `motor(ki, ka)` asli).
  - Suku D difilter; `KD` tidak lagi dibagi `SAMPLE_MS`.
  - Tidak ada lagi spin PWM penuh saat garis hilang.
- Masalah yang diselesaikan: zig-zag, overshoot, dan putaran liar.
- Catatan: untuk garis putus-putus gunakan `followLineDashed()`.

**`followLineDashed(baseSpeed)`** (`config.h:330`)
- Tujuan: line follower terpisah untuk garis putus-putus.
- Urutan:
  1. `updateLineState()` dan hysteresis `lineSeen()`.
  2. Jika garis terlihat (3 sampel setelah gap): PD seperti `followLine()`,
     simpan `dashedLastCorrection`.
  3. Jika hilang `< DASHED_COAST_MS`: maju dengan koreksi terakhir (coast).
  4. Jika hilang sampai `DASHED_COAST_MS + DASHED_SEARCH_MS`: maju pelan sambil
     menyapu kiri/kanan tiap `DASHED_SWEEP_MS`.
  5. Jika lebih lama: `driveMotors(0, 0)` sampai garis terlihat lagi.
- Perbedaan dari `followLine()`: tidak ada pivot `EDGE_POSITION`; reacquire
  me-reset suku D; state kontrol terpisah (`dashed*`).
- Catatan: belum dipanggil dari `runCourse()`.

**`driveForward(leftPwm, rightPwm, seconds)`** (`config.h:408`)
- Tujuan: maju dengan PWM tetap selama kira-kira `seconds` detik.
- Perubahan: nama dari `tForward()`, `j = i` dihapus.
- Catatan: open-loop; jarak bergantung pada baterai dan beban.

**`driveBackward(leftPwm, rightPwm, seconds)`** (`config.h:414`)
- Tujuan: mundur dengan PWM tetap. Nama dari `tBackward()`.

**`followLineForTicks(speed, ticks)`** (`config.h:420`)
- Tujuan: mengikuti garis selama `ticks * 10` siklus kontrol.
- Perubahan: nama dari `tik()`.

**`followLineForTicksDashed(speed, ticks)`** (`config.h:426`)
- Tujuan: wrapper `ticks * 10` siklus `followLineDashed()`; drop-in untuk
  `followLineForTicks()` saat integrasi nanti.

**`turnRight(leftPwm, rightPwm, ticks)`** (`config.h:432`)
- Tujuan: pivot kanan selama `ticks * 500` iterasi. Nama dari `tRight()`.

**`turnLeft(leftPwm, rightPwm, ticks)`** (`config.h:438`)
- Tujuan: pivot kiri selama `ticks * 500` iterasi. Nama dari `tLeft()`.

**`waitCrossings(mask, crossings, speed)`** (`config.h:458`)
- Tujuan: berjalan sampai pola sensor `mask` terdeteksi sebanyak `crossings`.
- Cara kerja: fase "menunggu set" dan "menunggu clear", masing-masing harus
  bertahan `DEBOUNCE_SAMPLES` sampel; ada timeout `SEARCH_TIMEOUT_MS`.
- Perubahan: menggabungkan `LCounter()` dan `CCounter()`, `RCounter()` dihapus.
- Masalah yang diselesaikan: hitungan ganda, hitungan terlewat, dan loop tanpa
  akhir.

**`initHardware()`** (`config.h:488`)
- Tujuan: inisialisasi pin, servo, OLED, posisi awal, dan memuat kalibrasi.
- Perubahan: nama dari `io_init()`, `Serial.begin()` dihapus (tidak ada lagi
  output serial hidup), dan `copy()` diganti `loadCalibration()` dengan
  fallback `loadDefaultCalibration()`.

### 4.6 `track.h`

**`runCourse()`** (`track.h:5`)
- Tujuan: urutan gerakan lintasan: reload → 5 basket → fortune ball.
- Perubahan: hanya nama dari `biru()`. Semua angka dan urutan
  `driveForward`/`followLineForTicks`/`turnRight`/`turnLeft`/
  `waitCrossings`/`lift`/`gripper`/`delay` dipertahankan.
- Catatan: bagian besar tetap open-loop. Error jarak akibat tegangan baterai
  tidak bisa diperbaiki oleh kontroler garis; perlu encoder atau IMU.

### 4.7 `main.ino`

**`setup()`** (`main.ino:3`)
- Tujuan: memanggil `initHardware()`.

**`runMenu()`** (`main.ino:7`)
- Tujuan: menu tombol.
  - T1 (`BUTTON_CALIBRATE`): `stopMotors()` lalu `calibrate(100)`.
  - T2 (`BUTTON_COURSE`): `runCourse()`, lalu berhenti dan kembali ke menu.
  - T3 (`BUTTON_TEST`): `followLine(250)` terus sampai T3 ditekan lagi.
  - T4 (`BUTTON_COPY`): `loadDefaultCalibration()` + `saveCalibration()`.
- Perubahan: `while(1)` dan `if` yang tidak terjangkau dihapus; Copy sekarang
  benar-benar bekerja dan tersimpan.
- Catatan: T3 sengaja tidak diberi timeout karena mode uji dipegang operator.

**`loop()`** (`main.ino:61`)
- Tujuan: memanggil `runMenu()`; menu tidak pernah kembali.

---

## 5. Tabel rename

### 5.1 Variabel dan konstanta

| Lama | Baru | Arti |
|---|---|---|
| `nTengah[]` | `DEFAULT_THRESHOLDS[]` | ambang default pabrik |
| `eTengah[]` | `threshold[]` | ambang per sensor |
| `sen[]` | `rawSensor[]` | bacaan mentah 0–255 |
| `biner[]` | `sensorOnLine[]` | 1 jika sensor melihat garis |
| `sensor` | `sensorMask` | 8 bit pola sensor untuk crossing |
| `pv` | `linePosition` | posisi garis −30…+30 |
| `Error` | `error` | error kontroler |
| `PD` | `correction` | keluaran P + D |
| `pwm1`, `pwm2` | `leftPwm`, `rightPwm` | PWM roda (lokal) |
| `kec` | `baseSpeed` | kecepatan dasar |
| `kecl` | `speed` | kecepatan saat mencari crossing |
| `hitung` | `count` | jumlah crossing (lokal) |
| `brpkali` | `crossings` | target jumlah crossing |
| `openn` | `GRIPPER_OPEN` | sudut gripper buka |
| `closed` | `GRIPPER_CLOSED` | sudut gripper tutup |
| `up` | `LIFT_UP` | sudut lift atas |
| `down` | `LIFT_DOWN` | sudut lift bawah |
| `setPoint` | `SET_POINT` | target posisi garis |
| `Kp`, `Kd`, `Ts` | `KP`, `KD`, `SAMPLE_MS` | konstanta kontroler |
| `minim` | `MIN_PWM` | PWM minimum roda |
| `buzzer` | `BUZZER_PIN` | pin buzzer |
| `mLA`, `mLB`, `mRA`, `mRB` | `MOTOR_LEFT_FWD`, `MOTOR_LEFT_REV`, `MOTOR_RIGHT_FWD`, `MOTOR_RIGHT_REV` | pin motor |
| `t1`…`t4` | `BUTTON_CALIBRATE`, `BUTTON_COURSE`, `BUTTON_TEST`, `BUTTON_COPY` | tombol |
| `A`, `B`, `C`, `muxIn` | `MUX_SELECT_A`, `MUX_SELECT_B`, `MUX_SELECT_C`, `MUX_INPUT` | pin multiplexer |
| `x`, `s`, `pos`, `addr`, `j` | dihapus | tidak pernah dibaca |

### 5.2 Fungsi

| Lama | Baru |
|---|---|
| `io_init()` | `initHardware()` |
| `readSelector()` | `readSensors()` |
| `selector()` | `selectMuxChannel()` |
| `lcd_char()` | `displayText()` |
| `drawProgressbar()` | `drawProgressBar()` |
| `motor()` | `driveMotors()` |
| `runn()` | `followLine()` |
| `logic()` + `convertLogic()` | `updateLineState()` |
| `rem()` | `brake()` |
| `Stop()` | `stopMotors()` |
| `bip()` | `beep()` |
| `naik()` | `liftUp()` |
| `turun()` | `liftDown()` |
| `tForward()` | `driveForward()` |
| `tBackward()` | `driveBackward()` |
| `tik()` | `followLineForTicks()` |
| `tRight()` | `turnRight()` |
| `tLeft()` | `turnLeft()` |
| `LCounter()` + `CCounter()` | `waitCrossings()` |
| `biru()` | `runCourse()` |
| `go()` | `runMenu()` |
| `copy()` | `loadDefaultCalibration()` |

---

## 6. Fungsi dan kode yang dihapus

| Yang dihapus | Alasan |
|---|---|
| `merah()` (`track.h`, ±360 baris) | Tidak pernah dipanggil; hampir semuanya komentar |
| `BT()` (`main.ino`) | Tidak pernah dipanggil; perintah serial/Bluetooth mati |
| `RCounter()` | Tidak pernah dipanggil |
| `scanL()` | Tidak pernah dipanggil |
| `gripperTest()` | Tidak pernah dipanggil |
| `test()` | Tidak pernah dipanggil |
| `tampilData()`, `tampilConvert()`, `tampilETengah()` | Debug serial/OLED, tidak dipakai |
| `drawBargraph()` | Tidak dipakai |
| `lcd_num()` | Hanya dipakai `tampilETengah()` yang dihapus |
| `right()`, `left()`, `forward()`, `backward()` | Duplikat `driveMotors()` dan polaritasnya salah |
| `eGaris[]`, `eBG[]` | Disimpan ke EEPROM tetapi tidak pernah dibaca |
| `garis[]`, `bg[]`, `tengah[]` global | Cukup lokal di `calibrate()` |
| Macro `btPlus`…`btCancel`, `NUMFLAKES` | Tidak dipakai |
| `j = i` di tiap loop gerak | Menulis global yang tidak pernah dibaca |

---

## 7. Perubahan perilaku yang perlu diketahui

1. **Kalibrasi ulang sekali setelah flashing.** Marker EEPROM berubah dari
   `0x5A5A` ke `0x5A5B`; data lama akan ditolak dan firmware memakai default.
2. **Tombol Copy** sekarang mengembalikan tabel default *dan* menyimpannya ke
   EEPROM. Sebelumnya tombol ini tidak melakukan apa-apa.
3. **Parameter kontroler berubah.** Input tidak lagi diskrit 5 satuan, jadi
   `KD` diturunkan dari efektif 29 menjadi 10 dan diberi filter. Ini perlu
   tuning di lintasan.
4. **Deadband motor aktif** (`MOTOR_DEADBAND = 40`). PWM kecil tidak lagi
   membuat satu roda diam.
5. **Garis hilang** tidak lagi membuat robot berputar PWM penuh; robot mencari
   pelan ke sisi terakhir dan `waitCrossings()` berhenti setelah 10 detik.
6. **Crossing** butuh 3 sampel berturut, sehingga garis tebal tidak dihitung
   dua kali.
7. **Angka lintasan tidak berubah.** Semua nilai di `runCourse()` diverifikasi
   identik dengan `biru()` lama.
8. **Fungsi dashed belum aktif.** `followLineDashed()` dan
   `followLineForTicksDashed()` ditambahkan terpisah; `followLine()` dan
   `runCourse()` tidak berubah sampai integrasi dilakukan.

---

## 8. Konstanta tuning

| Konstanta | Nilai awal | Efek |
|---|---|---|
| `KP` | 5 | reaksi terhadap offset garis; naikkan sampai sedikit osilasi lalu turunkan |
| `KD` | 10 | redaman; terlalu besar → getar, terlalu kecil → overshoot |
| `DERIVATIVE_ALPHA` | 30 | persen nilai turunan baru per sampel; kecilkan jika bising |
| `MOTOR_DEADBAND` | 40 | PWM minimum agar kedua roda mulai bergerak |
| `MIN_CONTRAST` | 30 | kontras minimum agar sensor dianggap valid |
| `MIN_LINE_STRENGTH` | 400 | ambang "garis masih terlihat" |
| `EDGE_POSITION` | 28 | kapan mulai pivot di ujung sensor |
| `PIVOT_SPEED` | 140 | PWM saat pivot |
| `SEARCH_SPEED` | 100 | PWM saat mencari garis hilang |
| `DEBOUNCE_SAMPLES` | 3 | jumlah sampel untuk satu crossing |
| `SEARCH_TIMEOUT_MS` | 10000 | batas waktu mencari crossing |
| `SAMPLE_MS` | 4 | periode loop kontrol |
| `DASHED_COAST_MS` | 250 | batas coast sebelum mencari garis |
| `DASHED_SEARCH_MS` | 1500 | lama sapuan sebelum berhenti |
| `DASHED_SWEEP_MS` | 180 | periode ganti arah sapuan |
| `DASHED_REACQUIRE_SAMPLES` | 3 | sampel berturut untuk menganggap garis kembali |
| `DASHED_RELEASE_STRENGTH` | 300 | ambang hysteresis saat garis sudah terlihat |
| `DASHED_SEARCH_SPEED` | 100 | PWM maju saat menyapu |
| `DASHED_SWEEP_BIAS` | 55 | selisih PWM kiri/kanan saat menyapu |

Urutan tuning yang disarankan: `KD = 0`, `MOTOR_DEADBAND = 0` → naikkan `KP`
sampai hampir osilasi → ukur deadband → naikkan `KD` → uji garis hilang →
uji garis putus-putus (`DASHED_COAST_MS`) → jalankan lintasan penuh.

---

## 9. Verifikasi

Yang sudah dilakukan:

- Test host `tests/test_control_math.cpp`: 36 assertion lulus, mencakup
  normalisasi, kontras rendah, centroid (sensor tunggal, pasangan tengah,
  pasangan asimetris, tepi simetris, kosong), deadband, filter, hysteresis
  garis, fase celah, dan arah sapuan.
- Syntax check seluruh sketch memakai stub header Arduino: lulus.
- Pengecekan angka: seluruh angka di `runCourse()` dibandingkan dengan
  `biru()` lama dan identik.
- Pengecekan arah steering: `left = base − PD`, `right = base + PD` sesuai
  `motor(ki, ka)` asli.

Yang belum dilakukan:

- Build dan flash dengan Arduino IDE/`arduino-cli` sungguhan (toolchain dan
  library tidak tersedia di mesin ini).
- Uji di lintasan: tuning `KP`/`KD`/deadband dan verifikasi recovery.
- Integrasi `followLineDashed()` ke segmen putus-putus di `runCourse()`.
- Uji `followLineDashed()` di lintasan bergaris putus-putus.

Cara menjalankan test:

```bash
g++ -std=c++11 -Wall -Wextra tests/test_control_math.cpp -o /tmp/test_control_math
/tmp/test_control_math
```

---

## 10. Batasan dan rencana lanjut

- **Lintasan masih open-loop.** `driveForward`, `turnRight`, `turnLeft`, dan
  `followLineForTicks` bergantung pada waktu dan tegangan baterai. Ini sumber
  error terbesar, bukan kontroler garis.
- **Garis putus-putus.** `followLineDashed()` menangani celah < 5 cm dengan
  coast. Celah lebih panjang atau kecepatan tinggi memerlukan encoder/IMU.
- **Roda dalam belum bisa berbalik.** `MIN_PWM = 0`; tikungan tajam ditangani
  oleh pivot. Membalik roda dalam adalah eksperimen terpisah.
- **Tidak ada encoder/IMU/kamera.** Untuk stabilitas lintasan, IMU (heading
  hold) atau encoder roda memberi peningkatan lebih besar daripada ML.
- **Machine learning belum dipakai.** Inference model kecil sebenarnya muat di
  ATmega328P, tetapi input hanya 8 sensor dan tidak ada ground truth; model
  akan belajar mengulang centroid. Bottleneck-nya ada di sensing dan gerak
  open-loop. ML baru masuk akal setelah ada data analog + ground truth.
- **Tuning wajib di lintasan.** Nilai `KP`, `KD`, dan `MOTOR_DEADBAND` di atas
  adalah titik awal, bukan hasil akhir.
