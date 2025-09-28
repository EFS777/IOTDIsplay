# IOTDIsplay
A simple yet powerful IoT project that lets you send custom text from a Flutter mobile app to a 16x2 LCD display and LED display using NodeMCU (ESP8266) and Arduino. This project showcases how mobile apps can interact with hardware in real-time over Wi-Fi.

Hello Fellow Developers 👋
This repository contains the following 7 files:

1. mobileApp.dart – Flutter code for the mobile application UI and logic.

2. pubspec.yaml – Flutter configuration file containing dependencies and version information.
👉 Please try to use the same versions to avoid compatibility issues.

3. arduino_lcd.ino – Arduino sketch to receive serial data and display it on the 16x2 LCD.

4. nodemcu_mqtt.ino – NodeMCU (ESP8266) sketch to connect to the MQTT broker and subscribe to incoming messages.

5. schematic.png – Circuit diagram showing how the hardware components are connected for LCD display.
   
6. led_display_schematic.png – Circuit diagram showing how the hardware components are connected for LCD display.

7. ledDisplay.ino – Arduino sketch to receive serial data and display it on the LED display.
