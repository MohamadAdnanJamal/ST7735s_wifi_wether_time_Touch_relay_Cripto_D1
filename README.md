
Certainly! Below is a more detailed and extended README.md file for your project:

ESP8266-based Smart Weather Station & Relay Control with 1.8" TFT Display
Overview
This project is an advanced, feature-rich IoT weather station and relay control system, developed using the ESP8266 microcontroller. The system integrates multiple functionalities, including real-time weather data retrieval, cryptocurrency price monitoring, NTP-based time synchronization, and relay control, all displayed on a 1.8" ST7735 TFT LCD screen. Additionally, the project offers a simple web interface for remote relay control, making it an ideal solution for smart home automation.

Table of Contents
Features
Hardware Requirements
Software Requirements
Circuit Diagram
Installation and Setup
Library Installation
Hardware Connections
Wi-Fi Configuration
Weather API Configuration
Uploading the Code
Usage Instructions
Display Features
Relay Control
Web Interface
Customization Options
Display Adjustments
Adding More Data
Relay Logic Customization
Troubleshooting
Contributing
License
Features
Weather Station



Real-time Weather Data: Fetches and displays the current temperature, humidity, wind speed, and sky conditions from the OpenWeatherMap API.
Automatic Updates: Weather information is updated every 5 minutes to ensure that the displayed data is current.
Cryptocurrency Price Monitoring
Supported Cryptocurrencies: Displays the latest prices for Bitcoin (BTC), Ethereum (ETH), and Solana (SOL).
Frequent Updates: Cryptocurrency prices are updated every 60 seconds.
Time Display
NTP Time Synchronization: Retrieves the current time from an NTP server, ensuring accurate timekeeping.
Real-time Clock Display: The time is displayed prominently on the TFT screen, updated every second.
Relay Control
Touch Sensor Control: Two touch sensors are connected to the ESP8266, allowing for manual control of two relays. These can be used to switch connected devices on or off.
Web Interface Control: A simple web interface allows remote control of the relays via any web browser on the same network.
TFT Display
1.8" ST7735 TFT LCD: All data is displayed on a vibrant, full-color TFT screen.
Flicker-Free Updates: The display is optimized to update only the changing portions of the screen, minimizing flicker and improving visual clarity.
Hardware Requirements
ESP8266 Module: NodeMCU or Wemos D1 Mini recommended.
1.8" ST7735 TFT LCD Display: For displaying weather, cryptocurrency prices, time, and relay status.
Two Touch Sensors: Used for manual relay control.
Two Relay Modules: For controlling connected devices.
Power Supply: 5V/2A power supply for the ESP8266 and relays.
Connecting Wires: Jumper wires for connections.
Software Requirements
Arduino IDE: Download from here.
Libraries: The following libraries must be installed in the Arduino IDE:
ESP8266WiFi.h: Manages Wi-Fi connectivity.
Adafruit_GFX.h: Core graphics library for the TFT display.
Adafruit_ST7735.h: Driver library for the ST7735 TFT display.
NTPClient.h: For time synchronization with an NTP server.
ArduinoJson.h: For parsing JSON responses from APIs.
ESP8266WebServer.h: To create a web server on the ESP8266.
ESP8266HTTPClient.h: For making HTTP requests.
Circuit Diagram
To be added: A detailed circuit diagram showing connections between the ESP8266, TFT display, touch sensors, and relays.

Installation and Setup
Library Installation
Open the Arduino IDE and go to Sketch > Include Library > Manage Libraries.
In the Library Manager, search for and install the following libraries:
ESP8266WiFi
Adafruit GFX
Adafruit ST7735
NTPClient
ArduinoJson
ESP8266WebServer
ESP8266HTTPClient
Hardware Connections
TFT Display:

SCK to GPIO 14 (D5)
SDA to GPIO 13 (D7)
A0 to GPIO 12 (D6)
RST to GPIO 15 (D8)
CS to GPIO 0 (D3)
VCC to 3.3V
GND to GND
Touch Sensors:

Sensor 1: Signal to GPIO 4 (D2)
Sensor 2: Signal to GPIO 5 (D1)
VCC to 3.3V
GND to GND
Relays:

Relay 1: IN to GPIO 16 (D0)
Relay 2: IN to GPIO 2 (D4)
VCC to 5V
GND to GND
Wi-Fi Configuration
Update the following lines in your code with your Wi-Fi credentials:

cpp
Copy code
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";
Weather API Configuration
Obtain an API key from OpenWeatherMap.
Update the following lines in your code with your API key, city name, and country code:
cpp
Copy code
const String apiKey = "Your_OpenWeatherMap_API_Key";
const String city = "Your_City";
const String countryCode = "Your_Country_Code";
Uploading the Code
Connect the ESP8266 to your computer via USB.
Select the correct board (e.g., NodeMCU 1.0) and port in the Arduino IDE.
Click the Upload button to upload the code to the ESP8266.
Usage Instructions
Display Features
Time: Displayed on the top left of the TFT screen, updated every second.
Weather: Displays temperature, humidity, wind speed, and sky conditions. Updated every 5 minutes.
Cryptocurrency Prices: Displays BTC, ETH, and SOL prices. Updated every 60 seconds.
Relay Control
Touch Sensors: Tap the touch sensors to toggle the state of the connected relays. The display shows the current state (e.g., "1 HIGH" or "1 LOW").
Web Interface: Access the web interface by entering the ESP8266's IP address in your browser. Toggle the relays using the on-screen buttons.
Web Interface
Accessing the Interface: Enter the ESP8266's local IP address in your web browser to access the relay control interface.
Relay Status: The interface displays the current state of both relays.
Control: Use the on-screen buttons to toggle the relays on or off.
Customization Options
Display Adjustments
Text Size & Color: Modify the text size and color for better visibility or aesthetic preference.
Positioning: Adjust the x and y coordinates of each display element to rearrange the layout.
Adding More Data
Additional Cryptocurrencies: Add more cryptocurrency prices by modifying the API request and display functions.
Extra Weather Data: Include more weather details, such as pressure or UV index, by parsing additional fields from the JSON response.
Relay Logic Customization
Multiple Relays: Expand the system to control more than two relays by adding more touch sensors and relay modules.
Conditional Logic: Implement more complex control logic, such as automated relay switching based on time or temperature.
Troubleshooting
Wi-Fi Connection Issues: Ensure that the correct SSID and password are entered. Check the serial monitor for connection status.
Display Issues: Verify the wiring connections between the ESP8266 and the TFT display. Ensure that the correct pins are defined in the code.
API Key Problems: Double-check the API key and URL syntax if weather data is not being retrieved.
Contributing
Contributions are welcome! If you have any suggestions or improvements, feel free to submit a pull request. For major changes, please open an issue first to discuss what you would like to change.

License
This project is licensed under the MIT License. You are free to use, modify, and distribute this project as per the terms of the license.
