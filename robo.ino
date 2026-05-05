// Definisi Pin Motor
int PWMA = 6;
int motorA1 = 10;
int motorA2 = 11;
int PWMB = 5;
int motorB1 = 9;
int motorB2 = 8;

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

// statement agar menghindari terjadinya debounce
bool sudahBerhenti = false;

void setup() {
    Serial.begin(9600);
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

    Serial.println("Sistem Enkoder Siap... Memulai Perjalanan.");
}

void loop() {
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

void maju(int pwma, int pwmb) {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    analogWrite(PWMA, pwma); 

    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
    analogWrite(PWMB, pwmb); 
}

void jarak_mobil(){
    volatile long hitung = (hitungA + hitungB)/2;
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