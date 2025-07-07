#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Your confirmed I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);
String inputText = "";

void setup() {
  Wire.begin(21, 22);         // SDA = 21, SCL = 22
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.println("Type text and press Enter to display it on the LCD.");
}

void loop() {
  if (Serial.available()) {
    inputText = Serial.readStringUntil('\n');
    lcd.clear();
    lcd.setCursor(0, 0);

    if (inputText.length() <= 16) {
      lcd.print(inputText);
    } else {
      lcd.print(inputText.substring(0, 16));
      lcd.setCursor(0, 1);
      lcd.print(inputText.substring(16, 32));
    }
  }
}
