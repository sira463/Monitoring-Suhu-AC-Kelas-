#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "Monitoring Kelas"
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// ===== WIFI =====
char ssid[] = "Lab Robotika";
char pass[] = "lab_robotika";

// ===== PIN ESP32-S3 (FIXED RECOMMENDED) =====
#define DHTPIN 4
#define DHTTYPE DHT22

#define PIR_PIN 5
#define LDR_PIN 6        // ADC
#define SOUND_PIN 7      // ADC

// BUTTON
#define BTN_KUNING 10
#define BTN_MERAH 11
#define BTN_HIJAU 12

// ===== OBJECT =====
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

// ===== BACA SUARA =====
int bacaSuara() {
  int minVal = 4095;
  int maxVal = 0;

  for (int i = 0; i < 50; i++) {
    int val = analogRead(SOUND_PIN);
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
    delay(2);
  }
  return maxVal - minVal;
}

// ===== KIRIM DATA =====
void kirimData() {

  float suhu = dht.readTemperature();
  float hum = dht.readHumidity();

  int gerak = digitalRead(PIR_PIN);
  int cahaya = analogRead(LDR_PIN);
  int suara = bacaSuara();

  if (isnan(suhu) || isnan(hum)) {
    suhu = 0;
    hum = 0;
  }

  // ===== BUTTON =====
  if (digitalRead(BTN_KUNING) == LOW) {
    Serial.println("KUNING DITEKAN");
    Blynk.virtualWrite(V5, 1);
    delay(200);
  } else {
    Blynk.virtualWrite(V5, 0);
  }

  if (digitalRead(BTN_MERAH) == LOW) {
    Serial.println("MERAH DITEKAN");
    Blynk.virtualWrite(V6, 1);
    delay(200);
  } else {
    Blynk.virtualWrite(V6, 0);
  }

  if (digitalRead(BTN_HIJAU) == LOW) {
    Serial.println("HIJAU DITEKAN");
    Blynk.virtualWrite(V7, 1);
    delay(200);
  } else {
    Blynk.virtualWrite(V7, 0);
  }

  // ===== SERIAL =====
  Serial.print("T: "); Serial.print(suhu);
  Serial.print(" | H: "); Serial.print(hum);
  Serial.print(" | G: "); Serial.print(gerak);
  Serial.print(" | L: "); Serial.print(cahaya);
  Serial.print(" | S: "); Serial.println(suara);

  // ===== BLYNK =====
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, gerak);
  int statusCahaya = (cahaya > 2000) ? 0 : 1;
  Blynk.virtualWrite(V3, statusCahaya);
  Blynk.virtualWrite(V4, suara);

  // ===== LCD =====
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print((int)suhu);
  lcd.print("C H:");
  lcd.print((int)hum);
  lcd.print("%   ");

  lcd.setCursor(0, 1);

  // GERAK
  lcd.print("G:");
  lcd.print(gerak ? "ON " : "OFF");

  // LDR
  lcd.print(" L:");
  lcd.print(cahaya > 2000 ? "OFF" : "ON");

  // SUARA
  lcd.print(" S:");
  lcd.print(suara);

  lcd.print("   ");
}

void setup() {
  Serial.begin(115200);

  // I2C LCD (ESP32-S3)
  Wire.begin(17, 18);
  lcd.begin(16, 2);
  lcd.backlight();

  // SENSOR
  dht.begin();

  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(SOUND_PIN, INPUT);

  // BUTTON
  pinMode(BTN_KUNING, INPUT_PULLUP);
  pinMode(BTN_MERAH, INPUT_PULLUP);
  pinMode(BTN_HIJAU, INPUT_PULLUP);

  // LCD START
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // BLYNK
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.clear();
  lcd.print("Connected!");

  timer.setInterval(2000L, kirimData);
}

void loop() {
  Blynk.run();
  timer.run();
}
