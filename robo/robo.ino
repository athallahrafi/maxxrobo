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
const float jarakPerPulsa = 2.042;

// base perhitungan pulsa enkoder a dan b
volatile long hitungA = 0; 
volatile long hitungB = 0;
volatile long jarakMobil = 0;


volatile unsigned long terakhirA = 0;
volatile unsigned long terakhirB = 0;
const int jedaDebounce = 5;

// Kontrol status
bool sudahBerhenti = false;
bool sudahMulai = false; 

// Timer untuk Serial Monitor agar tidak lag
unsigned long waktuLaluSerial = 0;

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

        if (jarakA <= 580.0) {
            maju(180, 200);
        } else {
            if (!sudahBerhenti) {
                kanan();
                stop();
                delay(480);
                stop();
                maju(180,200);
                delay(5500);
                stop();
                Serial.println(">>> TARGET 2 METER TERCAPAI! <<<");
                sudahBerhenti = true;
            }
        }

        static unsigned long waktuLalu = 0;
        if (millis() - waktuLaluSerial >= 100) {
            waktuLaluSerial = millis();
            
            float jarakA = hitungA * jarakPerPulsa;
            float jarakB = hitungB * jarakPerPulsa;

            Serial.print("A: "); Serial.print(hitungA);
            Serial.print(" | B: "); Serial.print(hitungB);
            Serial.print(" | Jarak: "); Serial.print(jarakMobil);
            Serial.println(" cm");
        }
    }
}

void maju(int pwma, int pwmb) {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    analogWrite(PWMA, pwma); 

    digitalWrite(motorB2, HIGH);
    digitalWrite(motorB1, LOW);
    analogWrite(PWMB, pwmb); 
}

void kanan() {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    analogWrite(PWMA, 200); 

    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
    analogWrite(PWMB, 100); 
}

void stop() {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);
    analogWrite(PWMA, 0); 

    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
    analogWrite(PWMB, 0); 
}

void jarak_mobil(){
    noInterrupts();
    long copyA = hitungA;
    long copyB = hitungB;
    interrupts();
    
    long rataRataPulsa = (copyA + copyB) / 2;
    jarakMobil = rataRataPulsa * jarakPerPulsa; 
}

// --- INTERRUPT SERVICE ROUTINE (ISR) DENGAN DEBOUNCE ---

void tambahHitungA() {
    // Jika waktu sekarang - waktu terakhir lebih dari 5ms, hitung sebagai pulsa sah
    if (millis() - terakhirA > jedaDebounce) {
        hitungA++;
        terakhirA = millis();
    }
}

void tambahHitungB() {
    if (millis() - terakhirB > jedaDebounce) {
        hitungB++;
        terakhirB = millis();
    }
}
