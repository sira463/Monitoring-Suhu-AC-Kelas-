
//versi esp32 2.0.17


#include <Arduino.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <ArduinoJson.h>
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif


#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
HardwareSerial SerialPort2(2);  // gunakan UART1
#include "DHT.h"
#define DHTPIN 18      // Pin data DHT22 terhubung ke GPIO4 (ubah sesuai kebutuhan)
#define DHTTYPE DHT22  // Tipe sensor
DHT dht(DHTPIN, DHTTYPE);

#define CONFIG_FILE "/config.json"
size_t config_count = 0;
int status_orang = 0;

//pin tombol
int pin_t_putih = 15;
int pin_t_biru = 23;
int pin_t_kuning = 4;



#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>
#include <ir_Daikin.h>
#include <ir_Sharp.h>
#include <ir_LG.h>
#include <ir_Samsung.h>
#include <ir_Toshiba.h>
#include <ir_Fujitsu.h>
#include <ir_Mitsubishi.h>
#include <ir_Gree.h>
#include <ir_Whirlpool.h>
#include <ir_Kelvinator.h>
#include <ir_Electra.h>
#include <ir_Coolix.h>
// Pin IR
#define IR_PIN 26
// Buat objek per merek
IRPanasonicAc acPanasonic(IR_PIN);
IRDaikinESP acDaikin(IR_PIN);
IRSharpAc acSharp(IR_PIN);
IRLgAc acLG(IR_PIN);
IRSamsungAc acSamsung(IR_PIN);
IRToshibaAC acToshiba(IR_PIN);
IRFujitsuAC acFujitsu(IR_PIN);
IRMitsubishiAC acMitsubishi(IR_PIN);
IRGreeAC acGree(IR_PIN);
IRWhirlpoolAc acWhirlpool(IR_PIN);
IRKelvinatorAC acKelvinator(IR_PIN);
IRElectraAc acElectra(IR_PIN);
IRCoolixAC acCoolix(IR_PIN);



//variabel
float suhu = 0;
float kelembaban = 0;
String pilihan_model_AC = "";
float batas_min_fan = 0;

String mode_now = "";
String last_mode = "";

// Brand global
String Brand = "PANASONIC";
/* ---- brand table ---- */
String all_brand[] = {
  "PANASONIC",
  "SHARP",
  "DAIKIN",
  "LG",
  "SAMSUNG",
  "TOSHIBA",
  "FUJITSU",
  "MITSUBISHI",
  "GREE",
  "WHIRLPOOL",
  "KELVINATOR",
  "ELECTRA",
  "COOLIX",
};
int jumlah_brand = sizeof(all_brand) / sizeof(all_brand[0]);

int timer = 0;
int timer1 = 0;
int timer2 = 0;
int conf = 0;

// WiFi dan server
const char* ssid = "wifi-iot";
const char* password = "password-iot";
const char* server_url = "";
const char* apikey = "";
unsigned long delay_iot = 10000;
unsigned long last_request = 0;
DynamicJsonDocument data(2048);

int port_buzzer = 1;


void lcd_i2c(String text = "", int baris = 0, int clear = 1) {
  byte bar[8] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  };
  if (text == "") {
    lcd.init();  //jika error pakai lcd.init();
    lcd.backlight();
    lcd.createChar(0, bar);
    lcd.setCursor(0, 0);
    lcd.print("Loading..");
    for (int i = 0; i < 16; i++) {
      lcd.setCursor(i, 1);
      lcd.write(byte(0));
      delay(100);
    }
    delay(50);
    lcd.clear();
  } else {
    if (clear == 1) {
      lcd.clear();
    }
    lcd.setCursor(0, baris);
    lcd.print(text + "                ");
    Serial.println(text);
  }
}

// Fungsi kendali generik
void Control_AC(int power = 0, String mode = "AUTO", String mode_fan = "AUTO" , int mode_kalib = 0) {
  mode_now = String(power) + "/" + mode + "/" + mode_fan;

  if (mode_now != last_mode || mode_kalib == 1) {
    lcd_i2c("COMMMAND");
    delay(1000);
    if (Brand == "PANASONIC") {
      acPanasonic.begin();
      if (power) acPanasonic.on();
      else acPanasonic.off();
      // acPanasonic.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acPanasonic.setFan(kPanasonicAcFanAuto);
      else if (mode_fan == "HIGH") acPanasonic.setFan(kPanasonicAcFanHigh);
      else if (mode_fan == "LOW") acPanasonic.setFan(kPanasonicAcFanLow);
      // Mode
      if (mode == "AUTO") acPanasonic.setMode(kPanasonicAcAuto);
      else if (mode == "COOL") acPanasonic.setMode(kPanasonicAcCool);
      else if (mode == "DRY") acPanasonic.setMode(kPanasonicAcDry);
      else if (mode == "HEAT") acPanasonic.setMode(kPanasonicAcHeat);
      else if (mode == "FAN") acPanasonic.setMode(kPanasonicAcFan);
      acPanasonic.send();
    } else if (Brand == "DAIKIN") {
      acDaikin.begin();
      if (power) acDaikin.on();
      else acDaikin.off();
      // acDaikin.setTemp(suhu);
      if (mode_fan == "AUTO") acDaikin.setFan(kDaikin64FanAuto);
      else if (mode_fan == "HIGH") acDaikin.setFan(kDaikin64FanHigh);
      else if (mode_fan == "LOW") acDaikin.setFan(kDaikin64FanQuiet);  // Ubah ke Quiet (level terendah)
      if (mode == "AUTO") acDaikin.setMode(kDaikinAuto);
      else if (mode == "COOL") acDaikin.setMode(kDaikinCool);
      else if (mode == "DRY") acDaikin.setMode(kDaikinDry);
      else if (mode == "HEAT") acDaikin.setMode(kDaikinHeat);
      else if (mode == "FAN") acDaikin.setMode(kDaikin64Fan);
      acDaikin.send();
    } else if (Brand == "SHARP") {
      acSharp.begin();
      if (power) acSharp.on();
      else acSharp.off();
      // acSharp.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acSharp.setFan(kSharpAcFanAuto);
      else if (mode_fan == "HIGH") acSharp.setFan(kSharpAcFanHigh);
      else if (mode_fan == "LOW") acSharp.setFan(kSharpAcFanMin);
      // Mode
      if (mode == "AUTO") acSharp.setMode(kSharpAcAuto);
      else if (mode == "COOL") acSharp.setMode(kSharpAcCool);
      else if (mode == "DRY") acSharp.setMode(kSharpAcDry);
      else if (mode == "HEAT") acSharp.setMode(kSharpAcHeat);
      // else if (mode == "FAN") acSharp.setMode(kSharpAcFan);  // Tidak didukung di library
      acSharp.send();
    } else if (Brand == "LG") {
      acLG.begin();
      if (power) acLG.on();
      else acLG.off();
      //acLG.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acLG.setFan(kLgAcFanAuto);
      else if (mode_fan == "HIGH") acLG.setFan(kLgAcFanHigh);
      else if (mode_fan == "LOW") acLG.setFan(kLgAcFanLow);
      // Mode
      if (mode == "AUTO") acLG.setMode(kLgAcAuto);
      else if (mode == "COOL") acLG.setMode(kLgAcCool);
      else if (mode == "DRY") acLG.setMode(kLgAcDry);
      else if (mode == "HEAT") acLG.setMode(kLgAcHeat);
      else if (mode == "FAN") acLG.setMode(kLgAcFan);
      acLG.send();
    } else if (Brand == "SAMSUNG") {
      acSamsung.begin();
      if (power) acSamsung.on();
      else acSamsung.off();
      // acSamsung.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acSamsung.setFan(kSamsungAcFanAuto);
      else if (mode_fan == "HIGH") acSamsung.setFan(kSamsungAcFanHigh);
      else if (mode_fan == "LOW") acSamsung.setFan(kSamsungAcFanLow);
      // Mode
      if (mode == "AUTO") acSamsung.setMode(kSamsungAcAuto);
      else if (mode == "COOL") acSamsung.setMode(kSamsungAcCool);
      else if (mode == "DRY") acSamsung.setMode(kSamsungAcDry);
      else if (mode == "HEAT") acSamsung.setMode(kSamsungAcHeat);
      else if (mode == "FAN") acSamsung.setMode(kSamsungAcFan);
      acSamsung.send();
    } else if (Brand == "TOSHIBA") {
      acToshiba.begin();
      if (power) acToshiba.on();
      else acToshiba.off();
      //  acToshiba.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acToshiba.setFan(kToshibaAcFanAuto);
      else if (mode_fan == "HIGH") acToshiba.setFan(kToshibaAcFanMax);
      else if (mode_fan == "LOW") acToshiba.setFan(kToshibaAcFanMin);
      // Mode
      if (mode == "AUTO") acToshiba.setMode(kToshibaAcAuto);
      else if (mode == "COOL") acToshiba.setMode(kToshibaAcCool);
      else if (mode == "DRY") acToshiba.setMode(kToshibaAcDry);
      else if (mode == "HEAT") acToshiba.setMode(kToshibaAcHeat);
      else if (mode == "FAN") acToshiba.setMode(kToshibaAcFan);
      acToshiba.send();
    } else if (Brand == "FUJITSU") {
      acFujitsu.begin();
      if (power) acFujitsu.on();
      else acFujitsu.off();
      //  acFujitsu.setTemp(suhu);
      // Fan (komentari karena method tidak ada; tambah model di constructor jika perlu)
      // if (mode_fan == "AUTO") acFujitsu.setFan(kFujitsuAcFanAuto);
      // else if (mode_fan == "HIGH") acFujitsu.setFan(kFujitsuAcFanHigh);
      // else if (mode_fan == "LOW") acFujitsu.setFan(kFujitsuAcFanQuiet);
      // Mode (tambah "Mode" pada konstanta)
      if (mode == "AUTO") acFujitsu.setMode(kFujitsuAcModeAuto);
      else if (mode == "COOL") acFujitsu.setMode(kFujitsuAcModeCool);
      else if (mode == "DRY") acFujitsu.setMode(kFujitsuAcModeDry);
      else if (mode == "HEAT") acFujitsu.setMode(kFujitsuAcModeHeat);
      else if (mode == "FAN") acFujitsu.setMode(kFujitsuAcModeFan);
      acFujitsu.send();
    } else if (Brand == "MITSUBISHI") {
      acMitsubishi.begin();
      if (power) acMitsubishi.on();
      else acMitsubishi.off();
      //  acMitsubishi.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acMitsubishi.setFan(kMitsubishiAcFanAuto);
      else if (mode_fan == "HIGH") acMitsubishi.setFan(kMitsubishiAcFanMax);
      else if (mode_fan == "LOW") acMitsubishi.setFan(kMitsubishiAcFanQuiet);
      // Mode
      if (mode == "AUTO") acMitsubishi.setMode(kMitsubishiAcAuto);
      else if (mode == "COOL") acMitsubishi.setMode(kMitsubishiAcCool);
      else if (mode == "DRY") acMitsubishi.setMode(kMitsubishiAcDry);
      else if (mode == "HEAT") acMitsubishi.setMode(kMitsubishiAcHeat);
      else if (mode == "FAN") acMitsubishi.setMode(kMitsubishiAcFan);
      acMitsubishi.send();
    } else if (Brand == "GREE") {
      acGree.begin();
      if (power) acGree.on();
      else acGree.off();
      //  acGree.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acGree.setFan(kGreeFanAuto);
      else if (mode_fan == "HIGH") acGree.setFan(kGreeFanMax);  // Ubah dari High ke Max
      else if (mode_fan == "LOW") acGree.setFan(kGreeFanMin);
      // Mode
      if (mode == "AUTO") acGree.setMode(kGreeAuto);
      else if (mode == "COOL") acGree.setMode(kGreeCool);
      else if (mode == "DRY") acGree.setMode(kGreeDry);
      else if (mode == "HEAT") acGree.setMode(kGreeHeat);
      else if (mode == "FAN") acGree.setMode(kGreeFan);
      acGree.send();
    } else if (Brand == "WHIRLPOOL") {
      acWhirlpool.begin();
      if (power) acWhirlpool.setPowerToggle(true);  // Ubah ke setPowerToggle(true) untuk on
      else acWhirlpool.setPowerToggle(false);       // Ubah ke setPowerToggle(false) untuk off
      //  acWhirlpool.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acWhirlpool.setFan(kWhirlpoolAcFanAuto);
      else if (mode_fan == "HIGH") acWhirlpool.setFan(kWhirlpoolAcFanHigh);
      else if (mode_fan == "LOW") acWhirlpool.setFan(kWhirlpoolAcFanLow);
      // Mode
      if (mode == "AUTO") acWhirlpool.setMode(kWhirlpoolAcAuto);
      else if (mode == "COOL") acWhirlpool.setMode(kWhirlpoolAcCool);
      else if (mode == "DRY") acWhirlpool.setMode(kWhirlpoolAcDry);
      else if (mode == "HEAT") acWhirlpool.setMode(kWhirlpoolAcHeat);
      else if (mode == "FAN") acWhirlpool.setMode(kWhirlpoolAcFan);
      acWhirlpool.send();
    } else if (Brand == "KELVINATOR") {
      acKelvinator.begin();
      if (power) acKelvinator.on();
      else acKelvinator.off();
      //  acKelvinator.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acKelvinator.setFan(kKelvinatorFanAuto);
      else if (mode_fan == "HIGH") acKelvinator.setFan(kKelvinatorFanMax);
      else if (mode_fan == "LOW") acKelvinator.setFan(kKelvinatorFanMin);
      // Mode
      if (mode == "AUTO") acKelvinator.setMode(kKelvinatorAuto);
      else if (mode == "COOL") acKelvinator.setMode(kKelvinatorCool);
      else if (mode == "DRY") acKelvinator.setMode(kKelvinatorDry);
      else if (mode == "HEAT") acKelvinator.setMode(kKelvinatorHeat);
      else if (mode == "FAN") acKelvinator.setMode(kKelvinatorFan);
      acKelvinator.send();
    } else if (Brand == "ELECTRA") {
      acElectra.begin();
      if (power) acElectra.on();
      else acElectra.off();
      //  acElectra.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acElectra.setFan(kElectraAcFanAuto);
      else if (mode_fan == "HIGH") acElectra.setFan(kElectraAcFanHigh);
      else if (mode_fan == "LOW") acElectra.setFan(kElectraAcFanLow);
      // Mode
      if (mode == "AUTO") acElectra.setMode(kElectraAcAuto);
      else if (mode == "COOL") acElectra.setMode(kElectraAcCool);
      else if (mode == "DRY") acElectra.setMode(kElectraAcDry);
      else if (mode == "HEAT") acElectra.setMode(kElectraAcHeat);
      else if (mode == "FAN") acElectra.setMode(kElectraAcFan);
      acElectra.send();
    } else if (Brand == "COOLIX") {
      acCoolix.begin();
      if (power) acCoolix.on();
      else acCoolix.off();
      //  acCoolix.setTemp(suhu);
      // Fan
      if (mode_fan == "AUTO") acCoolix.setFan(kCoolixFanAuto);
      else if (mode_fan == "HIGH") acCoolix.setFan(kCoolixFanMax);
      else if (mode_fan == "LOW") acCoolix.setFan(kCoolixFanMin);
      // Mode
      if (mode == "AUTO") acCoolix.setMode(kCoolixAuto);
      else if (mode == "COOL") acCoolix.setMode(kCoolixCool);
      else if (mode == "DRY") acCoolix.setMode(kCoolixDry);
      else if (mode == "HEAT") acCoolix.setMode(kCoolixHeat);
      else if (mode == "FAN") acCoolix.setMode(kCoolixFan);
      acCoolix.send();
    }
    // Jika brand tidak ditemukan, bisa tambahkan handling error di sini
    last_mode = mode_now;
  }
}



void setupWiFi() {
  WiFi.begin(ssid, password);
  lcd_i2c("Menghubungkan WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(1000);
    lcd_i2c(".", 1, 0);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    lcd_i2c(" WiFi Terhubung!");
    //lcd_i2c("IP: ");
    //lcd_i2c(WiFi.localIP().toString());
  } else {
    lcd_i2c(" Gagal terhubung ke WiFi.");
  }
}
// Fungsi utama untuk request data ke server (bisa kirim parameter)
void prosesData(String queryParams = "") {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  String url = String(server_url) + String(apikey) + queryParams;
  url.replace(" ", "%20");
  Serial.println("Request ke: " + url);
  http.begin(url);
  http.addHeader("Accept", "application/json");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    DeserializationError err = deserializeJson(data, response);
    if (err) {
      Serial.println("Gagal parsing JSON: " + String(err.c_str()));
    } else {
      Serial.println("Data JSON diterima.");
    }
  } else {
    Serial.println("HTTP Error: " + String(httpCode));
  }
  http.end();
}
void buzzer_on() {
  digitalWrite(port_buzzer, HIGH);
}
void buzzer_off() {
  digitalWrite(port_buzzer, LOW);
}
void buzzer_bip() {
  digitalWrite(port_buzzer, HIGH);
  delay(500);
  digitalWrite(port_buzzer, LOW);
}
void buzzer_bipbip() {
  digitalWrite(port_buzzer, HIGH);
  delay(500);
  digitalWrite(port_buzzer, LOW);
  delay(500);
  digitalWrite(port_buzzer, HIGH);
  delay(500);
  digitalWrite(port_buzzer, LOW);
  delay(500);
}
void led_ir_on() {
  digitalWrite(IR_PIN, HIGH);
}
void led_ir_off() {
  digitalWrite(IR_PIN, LOW);
}
void led_ir_bip() {
  digitalWrite(IR_PIN, HIGH);
  delay(500);
  digitalWrite(IR_PIN, LOW);
}
void led_ir_bipbip() {
  digitalWrite(IR_PIN, HIGH);
  delay(500);
  digitalWrite(IR_PIN, LOW);
  delay(500);
  digitalWrite(IR_PIN, HIGH);
  delay(500);
  digitalWrite(IR_PIN, LOW);
  delay(500);
}

// Fungsi utama untuk request data ke server (bisa kirim parameter)
void proses_iot(String queryParams = "") {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String url = String(server_url) + String(apikey) + queryParams;
  url.replace(" ", "%20");
  Serial.println("Request ke: " + url);
  http.begin(url);
  http.addHeader("Accept", "application/json");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println(response);

    DynamicJsonDocument jsonDoc(1024);


    DeserializationError err = deserializeJson(jsonDoc, response);
    if (err) {
      Serial.println("Gagal parsing JSON: " + String(err.c_str()));
      //  lcd_i2c("gagal");
    } else {
      Serial.println("Data JSON diterima.");
      // lcd_i2c("berhasil");

      batas_min_fan = jsonDoc["out_1"].as<float>();

      //  lcd_i2c(String(batas_min_fan));
    }
  } else {
    lcd_i2c(String(httpCode));
    Serial.println("HTTP Error: " + String(httpCode));
  }

  http.end();
}


void setup() {
  Serial.begin(115200);
  SerialPort2.begin(9600, SERIAL_8N1, 16, 17);
  lcd_i2c();
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Gagal mount SPIFFS");
    while (1)
      ;
  }
  dht.begin();
  Serial.println("Mulai baca DHT22...");


  delay(3000);
  setupWiFi();
  last_request = millis();
  pinMode(port_buzzer, OUTPUT);
  buzzer_bipbip();
  pinMode(pin_t_putih, INPUT_PULLUP);
  pinMode(pin_t_biru, INPUT_PULLUP);
  pinMode(pin_t_kuning, INPUT_PULLUP);

  pinMode(IR_PIN, OUTPUT);
  led_ir_bipbip();
  loadConfig();
  proses_iot("");
}





void loop() {
  suhu = dht.readTemperature();
  kelembaban = dht.readHumidity();

  if (SerialPort2.available()) {
    String data = SerialPort2.readStringUntil('\n');

    if (millis() - timer2 >= 10000) {
      status_orang = data.toInt();
      timer2 = millis();
    }
  }

  if (digitalRead(pin_t_putih) == 0) {
    pilih_model();
  }

  //logika mode

  if (conf == 1) {
    lcd_i2c("S:" + String(suhu) + "C K:" + String(kelembaban) + "%");
  }
  if (conf == 2) {
    lcd_i2c(Brand);
  }

  if (conf == 3) {
    if (status_orang == 1) {
      lcd_i2c("Ada Orang ");
    } else {
      lcd_i2c("Tidak Ada Orang ");
    }
  }

  if (suhu <= 26 && status_orang == 1) {
    //fan
    Control_AC(1, "AUTO", "HIGH");  // mode auto //fan high
    Control_AC(1, "FAN", "HIGH");   // mode fan//fan high
    lcd_i2c("FAN ON ", 1, 0);
  } else {

    if (kelembaban > 70 && status_orang == 1) {
      //dry
      Control_AC(1, "DRY", "LOW");  // mode dry // fan low
      lcd_i2c("MODE DRY ", 1, 0);
    }

    else if (kelembaban <= 70 && status_orang == 1) {
      //cool
      Control_AC(1, "COOL", "LOW");  // mode COOL // fan low
      lcd_i2c("MODE COOL ", 1, 0);
    }
  }

  if (status_orang == 0) {
    Control_AC(0, "AUTO", "LOW");  // AC MATI
    lcd_i2c("AC MATI ... ", 1, 0);
  }
  //  else if (status_orang == 1) {
  //   Control_AC(1, "AUTO", "LOW");  // AC HIDUP
  // }

  if (millis() - timer > 3000) {

    conf += 1;
    if (conf > 3) {
      conf = 0;
    }

    timer = millis();
  }

  if (millis() - timer1 >= 10000) {
    proses_iot("&suhu=" + String(suhu) + "&kelembaban=" + String(kelembaban) + "&orang=" + String(status_orang));
    lcd_i2c("Mengirim .. ");
    timer1 = millis();
  }
  delay(500);
}
