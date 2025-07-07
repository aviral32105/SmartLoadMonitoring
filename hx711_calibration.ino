#include "HX711.h"

#define DOUT 5   // DT pin
#define SCK 18   // SCK pin

HX711 scale;

long raw_zero = 0;
float scale_factor = 1.0;

void setup() {
  Serial.begin(115200);
  delay(1000);  

  Serial.println("\nHX711 Calibration");

  scale.begin(DOUT, SCK);

  // Wait for HX711 to be ready
  Serial.print("Checking HX711...");
  while (!scale.is_ready()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nHX711 is ready");

  Serial.println("Taring... Make sure nothing is on the scale.");
  delay(2000);
  scale.tare();  // Zero the scale
  raw_zero = scale.read_average();
  Serial.print("Tare complete. Offset: ");
  Serial.println(raw_zero);

  Serial.println("\nNow place known weight (285g). Waiting 5 seconds...");
  delay(5000);

  long raw_with_weight = scale.read_average();
  Serial.print("Raw with weight: ");
  Serial.println(raw_with_weight);

  float known_weight = 280.00;  // in grams
  scale_factor = (raw_with_weight - raw_zero) / known_weight;

  Serial.print("Calculated scale factor: ");
  Serial.println(scale_factor, 5);

  Serial.println("\nStarting real-time weight display...");
}

void loop() {
  long raw = scale.read_average();
  float weight = (raw - raw_zero) / scale_factor;
  Serial.print("Weight: ");
  Serial.print(weight, 2);
  Serial.println(" g");
  delay(1000);
}