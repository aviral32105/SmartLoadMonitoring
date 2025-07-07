#include "HX711.h"

// Pin connections
#define DT_PIN  5    // HX711 DT → ESP32 GPIO 5
#define SCK_PIN 18   // HX711 SCK → ESP32 GPIO 18

HX711 scale;

void setup() {
  Serial.begin(115200);
  delay(500);
  scale.begin(DT_PIN, SCK_PIN);
  Serial.println("\nHX711 Connection Checker");
}

void loop() {
  // Check connection every second
  if (scale.is_ready()) {
    Serial.print(millis());
    Serial.println(" ms – alive");
  } else {
    Serial.print(millis());
    Serial.println(" ms – dead");
  }
  delay(1000);
}