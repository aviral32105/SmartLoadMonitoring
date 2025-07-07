#include "HX711.h"
#include <WiFi.h>
#include <WebServer.h>

#define DOUT 5   // DT pin
#define SCK 18   // SCK pin

HX711 scale;

// Wi-Fi credentials (Mobile 2's hotspot)
const char* ssid = "Galaxy A70AD8B";
const char* password = "skrs5399";

WebServer server(80);  // Web server on port 80

// Predefined calibration (no need for calibration steps)
long raw_zero = 0;  // You can adjust if needed
float scale_factor = 513.61523;  // Your known calibration factor
float current_weight = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize HX711
  Serial.println("Initializing HX711...");
  scale.begin(DOUT, SCK);
  while (!scale.is_ready()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nHX711 ready!");

  // Tare the scale (optional, if you want to reset zero)
  Serial.println("Taring...");
  scale.tare();
  raw_zero = scale.read_average();
  Serial.print("Tare offset: ");
  Serial.println(raw_zero);

  // Connect to Wi-Fi (Mobile 2's hotspot)
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());  // Mobile 1 will use this IP

  // Set up web server routes
  server.on("/", handleRoot);  // Main page
  server.on("/weight", handleWeight);  // JSON endpoint for weight
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();  // Handle web requests
  updateWeight();  // Continuously read weight
  delay(100);  // Small delay to prevent overload
}

void updateWeight() {
  long raw = scale.read_average();
  current_weight = (raw - raw_zero) / scale_factor;
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Weight Scale</title>
    <style>
      body { font-family: Arial, sans-serif; text-align: center; }
      .weight { font-size: 3em; font-weight: bold; margin: 20px; }
    </style>
    <script>
      function updateWeight() {
        fetch("/weight")
          .then(response => response.json())
          .then(data => {
            document.getElementById("weight").innerText = data.weight.toFixed(2) + " g";
          });
      }
      setInterval(updateWeight, 1000);  // Update every second
    </script>
  </head>
  <body>
    <h1>ESP32 Weight Scale</h1>
    <div class="weight" id="weight">0.00 g</div>
  </body>
  </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleWeight() {
  String json = "{\"weight\":" + String(current_weight, 2) + "}";
  server.send(200, "application/json", json);
}