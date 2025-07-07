#include <WiFi.h>
#include <WebServer.h>
#include "HX711.h"

// HX711 pins
#define DOUT_PIN 5
#define SCK_PIN  18

HX711 scale;
long   raw_zero     = 0;
float  scale_factor = 1.0;

// Wi‑Fi credentials
const char* ssid     = "Galaxy A70AD8B";
const char* password = "skrs5399";

// HTTP server
WebServer server(80);

String buildPage(float weight) {
  // Basic HTML + JS alert if out of range
  String html = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="refresh" content="1">
  <title>ESP32 Scale</title>
</head>
<body>
  <h2>Live Weight: <span id="wt">)=====")
    + String(weight,2) +
  R"=====( g</span></h2>
<script>
  const w = parseFloat(document.getElementById('wt').innerText);
  if (w < 200) {
    alert('⚠️ Weight depletion: ' + w + ' g');
  }
  if (w > 700) {
    alert('⚠️ Weight overload: ' + w + ' g');
  }
</script>
</body>
</html>
)=====";
  return html;
}

void handleRoot() {
  long raw = scale.read_average(10);
  float w   = (raw - raw_zero) / scale_factor;
  server.send(200, "text/html", buildPage(w));
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // — HX711 calibration
  scale.begin(DOUT_PIN, SCK_PIN);
  while (!scale.is_ready()) { delay(200); }
  scale.tare(); raw_zero = scale.read_average();
  delay(100);
  // place your known weight (e.g. 550g)
  delay(5000);
  long withW = scale.read_average();
  scale_factor = float(withW - raw_zero) / 550.0;
  Serial.println("Calibrated.");

  // — Wi‑Fi (DHCP)
  WiFi.begin(ssid, password);
  Serial.print("Connecting Wi‑Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print('.');
  }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  // — Start server
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  // Serial output
  long raw = scale.read_average(10);
  float w   = (raw - raw_zero) / scale_factor;
  Serial.printf("Weight: %.2f g\n", w);

  // Web server
  server.handleClient();
  delay(1000);
}