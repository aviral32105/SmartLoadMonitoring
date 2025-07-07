AIM

To design and implement an IoT-based load monitoring system that:

1.Continuously measures material weight using strain gauge technology

2.Provides real-time weight visualization through multiple interfaces

3.Automatically alerts warehouse staff about critical weight conditions

4.Prevents operational disruptions through threshold-based notifications

PROBLEM STATEMENT

Warehouses face challenges in:

1.Real-time monitoring of material inventory levels

2.Preventing structural damage from overloading

3.Avoiding production stoppages due to material depletion

4.Manual monitoring inefficiencies leading to human error

5.Lack of instant alerts for critical weight conditions

This system solves these problems by providing automated, continuous monitoring with instant visual alerts when weight exceeds safe operational thresholds.

SCOPE OF THE SOLUTION

Hardware Monitoring

1.Continuous weight measurement (0-500g range)

2.Real-time display on LCD

3.Precision sensing with ±0.1g resolution

Alert System

1.Visual warnings on web interface

2.Browser pop-up notifications

3.Threshold triggers: <200g (depletion) and >500g (overload)

Remote Access

  1.Real-time web dashboard
  
  2.Cross-device compatibility (mobile/desktop)
  
  3.Local network access (no internet required)

Operational Parameters

  1.Measurement frequency: 2 updates/second
  
  2.Alert persistence until condition resolves
  
  3.Self-calibration capability

REQUIRED COMPONENTS

Hardware

  1.ESP32 microcontroller
  
  2.HX711 load cell amplifier
  
  3.5kg strain gauge load cell
  
  4.16x2 I2C LCD
  
  5.Breadboard
  
  6.Jumper wires
  
  7.Micro-USB cable

Software

  1.Arduino IDE
  
  2.HX711 library
  
  3.LiquidCrystal_I2C library
  
  4.WebServer library
  
  5.WiFi library

Cloud/Network

  1.Wi-Fi hotspot (Any smartphone/access point)
  
  2.Local network infrastructure

Monitoring Tools

  1.Web browser (Chrome/Firefox/Safari)
  
  2.Serial monitor for debugging
