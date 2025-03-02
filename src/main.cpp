#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "HX711.h"

#define WIFI_SSID "Raf"
#define WIFI_PASSWORD "741rafly"

#define API_KEY "AIzaSyAoH-Q8LJml747bl-lqkUVjuwTTXdftVoc"
#define DATABASE_URL "https://cps-kelompok-2-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Email & Password buat login ke Firebase Auth
#define USER_EMAIL "raflyfasha30@gmail.com"
#define USER_PASSWORD "rafly12345"

FirebaseData fbdata;
FirebaseAuth auth;
FirebaseConfig config;

#define REED_SWITCH 14   
#define TILT_SWITCH 18  
#define LOADCELL_DOUT 4
#define LOADCELL_SCK  5

HX711 scale;
float calibration_factor = 2280.0;  // Sesuaikan berdasarkan hasil kalibrasi

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(115200);

    // Koneksi Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(300);
    } 
    Serial.println("\nConnected to Wi-Fi!");
    
    // Setup Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    Serial.println("Firebase Initialized");

    pinMode(REED_SWITCH, INPUT_PULLUP);
    pinMode(TILT_SWITCH, INPUT_PULLUP);

    lcd.init();
    lcd.backlight();

    // Setup HX711
    scale.begin(LOADCELL_DOUT, LOADCELL_SCK);
    scale.set_scale(calibration_factor);
    scale.tare();  // Reset scale ke nol

    Serial.println("HX711 Scale Initialized");
}

void loop() {
    if (!Firebase.ready()) {
        Serial.println("Firebase not ready");
        delay(1000);
        return;
    }

    bool isLocked = digitalRead(REED_SWITCH) == HIGH;  // Reed switch aktif
    bool isTilted = digitalRead(TILT_SWITCH) == LOW;  // Tilt switch aktif

    // Baca berat dari load cell
    float weight = scale.get_units(10);
    if (abs(weight) < 5) weight = 0;  // Abaikan noise kecil

    // Tetapkan ambang batas berat buku
    const float MIN_BOOK_WEIGHT = 20.0;   // Berat minimum buku (dalam gram)
    const float MAX_BOOK_WEIGHT = 3000.0; // Berat maksimum buku normal (dalam gram)

    String bukuStatus;
    String tiltStatus;

    lcd.clear();

    if (isLocked && isTilted && weight >= MIN_BOOK_WEIGHT && weight <= MAX_BOOK_WEIGHT) {
        // Buku ada jika semua syarat terpenuhi
        lcd.setCursor(0, 0);
        lcd.print("Buku ada");
        bukuStatus = "Buku ada";
        tiltStatus = "Hidup";
    } else {
        // Jika salah satu syarat tidak terpenuhi
        lcd.setCursor(0, 0);
        if (!isLocked) {
            lcd.print("Buku tidak terkunci");
            bukuStatus = "Buku tidak terkunci";
            tiltStatus = "hidup";
        } 
        else if (!isTilted) {
            lcd.print("Buku miring");
            bukuStatus = "Buku tidak tepat";
            tiltStatus = "Tidak hidup";
        } 
        else if (weight < MIN_BOOK_WEIGHT) {
            lcd.print("Buku tidak ada");
            bukuStatus = "Buku tidak ada";
        } 

    }

    // Tampilkan berat di LCD
    lcd.setCursor(0, 1);
    lcd.print("Berat: ");
    lcd.print(weight);
    lcd.print("g");

    // Cek perubahan sebelum update Firebase
    static String lastBukuStatus = "";
    static String lastTiltStatus = "";
    static float lastWeight = 0;

    if (bukuStatus != lastBukuStatus) {
        Firebase.RTDB.setString(&fbdata, "/rak/buku_status", bukuStatus);
        lastBukuStatus = bukuStatus;
    }

    if (tiltStatus != lastTiltStatus) {
        Firebase.RTDB.setString(&fbdata, "/rak/status", tiltStatus);
        lastTiltStatus = tiltStatus;
    }

    if (abs(weight - lastWeight) > 5) {  // Update hanya jika perubahan > 5g
        Firebase.RTDB.setFloat(&fbdata, "/rak/berat", weight);
        lastWeight = weight;
    }

    delay(500); 
}