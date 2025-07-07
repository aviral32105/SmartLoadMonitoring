void setup() {
  // Start Serial at 115200 baud
  Serial.begin(115200);
  // Wait up to 2 seconds for the serial port to open
  unsigned long start = millis();
  while (!Serial && millis() - start < 2000) { /* wait */ }

  // Print a test message
  Serial.println("🚀 Serial is up and running!");

  // Configure built‑in LED (usually GPIO2) for a simple blink
  pinMode(2, OUTPUT);
}

void loop() {
  // Toggle the onboard LED every half‑second
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);

  // Also print a heartbeat
  Serial.println("❤️ Alive");
}