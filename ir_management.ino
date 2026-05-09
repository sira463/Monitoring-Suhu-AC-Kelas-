void saveConfig(String nama_brand) {
  File file = SPIFFS.open(CONFIG_FILE, "w");void
  if (!file) {
    Serial.println("❌ Gagal buka file untuk simpan");
    return;
  }
  DynamicJsonDocument doc(4096);
  doc["nama_brand"] = nama_brand;

  if (serializeJson(doc, file) == 0) {
    Serial.println("❌ Gagal tulis JSON");
  } else {
    Serial.println("✅ Config tersimpan");
  }
  file.close();
}

void loadConfig() {
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("❌ Gagal buka file untuk load");
    return;
  }
  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, file);
  if (err) {
    Serial.println("❌ Gagal parse JSON");
    file.close();
    return;
  }
  file.close();
  Brand = doc["nama_brand"].as<String>();
  Serial.println("✅ Config berhasil diload");
}




// ------------------ Fungsi pilih_model ------------------
void pilih_model() {
  int i = 0;
  Serial.println("Asumsikan AC OFF. Akan kirim ON/tes lalu OFF jika berhasil.");
  lcd_i2c("PASTIKAN AC MATI");
  delay(3000);
  lcd_i2c("MEMULAI TES .. ");
  lcd_i2c("Tekan Tombol ", 1, 0);
  delay(2000);
  lcd_i2c("'Biru' jika YA");
  lcd_i2c("'Kuning' jika Tidak", 1, 0);
  delay(3000);

  while (1) {
    Brand = all_brand[i];
    Control_AC(1, "AUTO", "AUTO",1);  // power on
    
    lcd_i2c(Brand);
    lcd_i2c("Apakah AC menyala?", 1, 0);
    int tombol = bacaInput();

    if (tombol == 1) {
      lcd_i2c("Terpilih .. ", 1, 0);
      // Simpan ke SPIFFS
      saveConfig(Brand);
      delay(1000);
      break;
    }

    if (tombol == 2) {
      if (i >= jumlah_brand - 1) i = 0;
      else i++;
      delay(500);
    }
  }
}

int bacaInput() {
  int i = 0;
  // Flush sisa buffer
  while (1) {
    if (digitalRead(pin_t_biru) == 0) {
      i = 1;
      break;
    } else if (digitalRead(pin_t_kuning) == 0) {
      i = 2;
      break;
    }
  }
  return i;  // Konversi ke lowercase untuk kemudahan
}

