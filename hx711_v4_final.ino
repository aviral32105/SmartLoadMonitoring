#include "HX711.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DOUT 5   // DT pin
#define SCK 18   // SCK pin

HX711 scale;

// Wi-Fi credentials
const char* ssid = "Galaxy A70AD8B";
const char* password = "skrs5399";

WebServer server(80);

// LCD Configuration (0x27 is common I2C address)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 display

// Predefined calibration values
long raw_zero = 0;
const float scale_factor = 445.70020;
float current_weight = 0.0;
unsigned long last_lcd_update = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize LCD
  Wire.begin();  // Initialize I2C
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  
  // Initialize HX711
  Serial.println("Initializing HX711...");
  scale.begin(DOUT, SCK);
  
  int attempts = 0;
  while (!scale.is_ready()) {
    Serial.print(".");
    lcd.print(".");
    delay(100);
    if (++attempts > 50) {
      lcd.clear();
      lcd.print("HX711 ERROR!");
      Serial.println("\nERROR: HX711 not responding!");
      while (1) delay(1000);
    }
  }
  
  lcd.clear();
  lcd.print("HX711 Ready!");
  Serial.println("\nHX711 ready!");

  // Tare the scale
  lcd.clear();
  lcd.print("Taring...");
  Serial.println("Taring...");
  scale.tare();
  raw_zero = scale.read_average();
  Serial.print("Tare offset: ");
  Serial.println(raw_zero);
  delay(1000);

  // Connect to Wi-Fi
  lcd.clear();
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
    if (++attempts > 30) {
      lcd.clear();
      lcd.print("WiFi FAILED!");
      Serial.println("\nERROR: WiFi connection failed!");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    lcd.clear();
    lcd.print("WiFi Failed!");
  }
  delay(2000);

  // Set up web server
  server.on("/", handleRoot);
  server.on("/weight", handleWeight);
  server.begin();
  
  lcd.clear();
  lcd.print("Ready!");
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  updateWeight();
  
  // Update LCD every 500ms (prevents flickering)
  if (millis() - last_lcd_update > 500) {
    updateLCD();
    last_lcd_update = millis();
  }
  
  delay(50);
}

void updateWeight() {
  if (scale.is_ready()) {
    long raw = scale.read();
    current_weight = (raw - raw_zero) / scale_factor;
  }
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight:");
  
  lcd.setCursor(0, 1);
  if (current_weight < 1000) {
    lcd.print(current_weight, 1);
    lcd.print(" g ");
  } else {
    lcd.print(current_weight/1000.0, 2);
    lcd.print(" kg");
  }
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Smart Load Monitoring System</title>
    <style>
      body { 
        font-family: Arial, sans-serif; 
        text-align: center; 
        background-color: #f5f5f5;
      }
      .container {
        max-width: 500px;
        margin: 0 auto;
        padding: 20px;
        background-color: white;
        border-radius: 10px;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        margin-top: 30px;
      }
      .weight { 
        font-size: 3em; 
        font-weight: bold; 
        margin: 20px 0;
        color: #333;
      }
      .status { 
        color: #888; 
        margin: 20px 0;
        font-size: 1.2em;
      }
      .alert {
        font-size: 1.8em;
        font-weight: bold;
        margin: 20px 0;
        padding: 15px;
        border-radius: 8px;
        display: none; /* Hidden by default */
      }
      .depletion {
        color: #d32f2f; /* Red */
        background-color: #ffcdd2;
        border: 2px solid #d32f2f;
      }
      .overload {
        color: #f57c00; /* Orange */
        background-color: #ffe0b2;
        border: 2px solid #f57c00;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Smart Load Monitoring System</h1>
      <div class="weight" id="weight">0.00 g</div>
      
      <!-- Weight status alerts -->
      <div id="depletion-alert" class="alert depletion">
        WEIGHT DEPLETION
      </div>
      <div id="overload-alert" class="alert overload">
        WEIGHT EXCEEDED
      </div>
      
      <div class="status" id="status">Live updating every second</div>
    </div>

    <script>
      let lastAlert = null;
      
      function updateWeight() {
        fetch("/weight")
          .then(response => response.json())
          .then(data => {
            const w = data.weight;
            document.getElementById("weight").innerText = w.toFixed(2) + " g";
            
            // Get alert elements
            const depletionAlert = document.getElementById('depletion-alert');
            const overloadAlert = document.getElementById('overload-alert');
            
            // Reset both alerts first
            depletionAlert.style.display = 'none';
            overloadAlert.style.display = 'none';
            
            // Show appropriate alert
            if (w < 200) {
              depletionAlert.style.display = 'block';
            } 
            else if (w > 500) {
              overloadAlert.style.display = 'block';
            }
            
            // Check for popup alerts (with state tracking)
            if (w < 200 && lastAlert !== "low") {
              alert('Weight depletion: ' + w.toFixed(2) + ' g');
              lastAlert = "low";
            } 
            else if (w > 500 && lastAlert !== "high") {
              alert('Weight overload: ' + w.toFixed(2) + ' g');
              lastAlert = "high";
            }
            else if (w >= 200 && w <= 500) {
              lastAlert = null; // Reset alert state
            }
          })
          .catch(err => console.error('Error:', err));
      }
      
      // Initial update and set interval
      updateWeight();
      setInterval(updateWeight, 1000);
    </script>
  </body>
  </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleWeight() {
  String json = "{\"weight\":" + String(current_weight, 2) + "}";
  server.send(200, "application/json", json);
}