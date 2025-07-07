#include "HX711.h"

// Pin definitions
#define DOUT_PIN 5    // HX711 DT → ESP32 GPIO 5
#define CLK_PIN  18   // HX711 SCK → ESP32 GPIO 18

HX711 scale;

void setup() {
  Serial.begin(115200);
  delay(500);  
  Serial.println("\nHX711 Raw Data Reader");

  // Initialize the HX711
  scale.begin(DOUT_PIN, CLK_PIN);

  // Check device presence
  if (scale.is_ready()) {
    Serial.println("HX711 detected. Reading raw values...");
  } else {
    Serial.println("HX711 not detected! Check wiring.");
    while (1); // halt
  }
}

void loop() {
  // Read a single raw value
  long raw = scale.read();
  
  // Print it
  Serial.print("Raw reading: ");
  Serial.println(raw);
  
  delay(1000); // one reading per second
}
