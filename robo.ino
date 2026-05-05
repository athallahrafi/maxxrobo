// Definisi Pin Motor
int PWMA = 6;
int motorA1 = 10;
int motorA2 = 11;
int PWMB = 5;
int motorB1 = 9;
int motorB2 = 8;

// Definisi Pin Tombol Start
int pinStart = 12; // Pin untuk tombol start

// Encoder
int pinEnkoderA = 2; 
int pinEnkoderB = 3;

// Perhitungan jarak per pulsa
// dengan rumus 3,14*6,5*100/8
const float jarakPerPulsa = 2.55;

// base perhitungan pulsa enkoder a dan b
volatile long hitungA = 0; 
volatile long hitungB = 0;

volatile long jarakMobil = 0;

// statement agar menghindari terjadinya debounce & kontrol status
bool sudahBerhenti = false;
bool sudahMulai = false; // Tambahan: Status apakah mobil sudah boleh jalan

void setup() {
    Serial.begin(9600);
    
    // Konfigurasi Tombol Start menggunakan PULLUP internal
    pinMode(pinStart, INPUT_PULLUP);
    
    pinMode(pinEnkoderA, INPUT_PULLUP);
    pinMode(pinEnkoderB, INPUT_PULLUP);
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(motorA1, OUTPUT);
    pinMode(motorA2, OUTPUT);
    pinMode(motorB1, OUTPUT);
    pinMode(motorB2, OUTPUT);

    // sensor akan menambah ketika sensor mulai membaca lubang 
    attachInterrupt(digitalPinToInterrupt(pinEnkoderA), tambahHitungA, RISING);
    attachInterrupt(digitalPinToInterrupt(pinEnkoderB), tambahHitungB, RISING);

    Serial.println("Sistem Siap...");
    Serial.println("Tekan tombol Start untuk memulai perjalanan.");
}

void loop() {
    // 1. Logika untuk mengecek tombol START
    if (!sudahMulai) {
        // Tombol bernilai LOW saat ditekan karena menggunakan INPUT_PULLUP
        if (digitalRead(pinStart) == LOW) {
            delay(50); // Anti-debounce sederhana agar tidak mendeteksi 2 kali
            if (digitalRead(pinStart) == LOW) {
                sudahMulai = true; // Mengizinkan mobil berjalan
                
                // Reset hitungan agar jarak dihitung murni dari saat tombol ditekan
                hitungA = 0;
                hitungB = 0;
                jarakMobil = 0;
                
                Serial.println(">>> TOMBOL DITEKAN! MOBIL MULAI BERJALAN <<<");
            }
        }
    }

    // 2. Logika utama pergerakan mobil (hanya berjalan jika sudahMulai == true)
    if (sudahMulai) {
        jarak_mobil();
        float jarakA = hitungA * jarakPerPulsa;
        float jarakB = hitungB * jarakPerPulsa;

        if (jarakMobil <= 100.0) {
            maju(200, 200);
        } else {
            if (!sudahBerhenti) {
                stop();
                Serial.println(">>> TARGET 1 METER TERCAPAI! <<<");
                sudahBerhenti = true;
            }
        }

        static unsigned long waktuLalu = 0;
        if (millis() - waktuLalu >= 50) {
            waktuLalu = millis();

            Serial.print("Jarak A: "); Serial.print(jarakA); Serial.print(" cm | ");
            Serial.print("Jarak B: "); Serial.print(jarakB); Serial.print(" cm |");
            Serial.print("Jarak Mobil: "); Serial.println(jarakMobil);
        }
    }
}

void maju(int pwma, int pwmb) {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    analogWrite(PWMA, pwma); 

    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
    analogWrite(PWMB, pwmb); 
}

void jarak_mobil(){
    // Menghindari masalah saat membaca variabel volatile yang bisa berubah tiba-tiba
    long copyHitungA = hitungA;
    long copyHitungB = hitungB;
    
    long hitung = (copyHitungA + copyHitungB) / 2;
    jarakMobil = hitung * jarakPerPulsa; 
}

void stop() {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
    analogWrite(PWMA, 0); 

    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
    analogWrite(PWMB, 0); 
}

void tambahHitungA() {
    hitungA++;
}

void tambahHitungB() {
    hitungB++;
}
