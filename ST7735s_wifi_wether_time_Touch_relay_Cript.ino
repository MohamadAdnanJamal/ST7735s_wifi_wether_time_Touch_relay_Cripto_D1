#include <ESP8266WiFi.h>          // WiFi
#include <Adafruit_GFX.h>         // LCD
#include <Adafruit_ST7735.h>      // LCD
#include <SPI.h>                  // LCD
#include <Fonts/FreeMono9pt7b.h>  // Include a custom font

#include <NTPClient.h>
#include <WiFiUdp.h>


#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>


#include <ESP8266WebServer.h>


//*********************************************//

#define TOUCH_SENSOR_1 D1
#define TOUCH_SENSOR_2 D2
#define RELAY_1 D4
#define RELAY_2 D3

bool relay1State = false;
bool relay2State = false;

unsigned long previousMillis = 0;
const long interval = 25;  // Check touch sensors every 50ms

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
const long debounceDelay = 50;  // Debounce delay of 200ms

bool lastTouch1State = HIGH;
bool lastTouch2State = HIGH;

ESP8266WebServer server(80);
//***********************************************//


#define TFT_CS D8   // Chip Select pin
#define TFT_RST -1  // Or set to D3 if you have connected to the reset pin
#define TFT_DC D0   // Data/Command pin AO
#define TFT_SCK D7  // Clock pin
#define TFT_SDA D5  // Data pin (MOSI)

bool getRelay1State() {
  return relay1State;
}

bool getRelay2State() {
  return relay2State;
}


// Initialize the display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Function to convert hex color to 16-bit color
uint16_t colorFromHex(uint32_t hexColor) {
  return tft.color565((hexColor >> 16) & 0xFF, (hexColor >> 8) & 0xFF, hexColor & 0xFF);
}

// WiFi icon pixel data
const unsigned char wifiIcon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x0f, 0xe0, 0x00,
  0x00, 0x7f, 0xfc, 0x00,
  0x00, 0xe0, 0x0e, 0x00,
  0x01, 0xc0, 0x07, 0x00,
  0x03, 0x80, 0x03, 0x80,
  0x07, 0x00, 0x01, 0xc0,
  0x0e, 0x0f, 0xe0, 0xe0,
  0x1c, 0x3f, 0xf8, 0x70,
  0x38, 0x7f, 0xfc, 0x38,
  0x70, 0xe0, 0x0e, 0x1c,
  0x61, 0xc0, 0x07, 0x0c,
  0x03, 0x80, 0x03, 0x80,
  0x07, 0x0f, 0xe1, 0xc0,
  0x0e, 0x1f, 0xf0, 0xe0,
  0x0c, 0x3c, 0x78, 0x60,
  0x00, 0x70, 0x1c, 0x00,
  0x00, 0xe0, 0x0e, 0x00,
  0x00, 0xc1, 0x06, 0x00,
  0x00, 0x03, 0x80, 0x00,
  0x00, 0x07, 0xc0, 0x00,
  0x00, 0x0f, 0xe0, 0x00,
  0x00, 0x1f, 0xf0, 0x00,
  0x00, 0x0f, 0xe0, 0x00,
  0x00, 0x07, 0xc0, 0x00,
  0x00, 0x03, 0x80, 0x00,
  0x00, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00

};


const unsigned char newIcon[] PROGMEM = {};

// Replace these with your network credentials
const char* ssid = "XXXXXXXX";
const char* password = "XXXXXXXX";


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 60000);  // 0 is the offset (seconds) for UTC, 60000 is the interval to update time

int previousHourTens = -1;
int previousHourUnits = -1;
int previousMinuteTens = -1;
int previousMinuteUnits = -1;
//int previousSecondTens = -1;
//int previousSecondUnits = -1;


// Variable to track last time separator was toggled
unsigned long lastSeparatorToggle = 0;
bool separatorVisible = true;

String openWeatherMapApiKey = "XXXXXXXXXX";
String city = "Maltepe";
String countryCode = "TR";
unsigned long weatherLastTime = -300000;   // Set weatherLastTime to a negative value to fetch data immediately
unsigned long weatherTimerDelay = 300000;  // 5 minutes in milliseconds

float temp_C = 0.0;
int humidity = 0;
float wind_speed_float = 0.0;
String sky_clearance = "";

float prev_temp = -1000;
int prev_humidity = -1;
float prev_wind_speed = -1;
String prev_sky = "";


int btcUsd = 0, ethUsd = 0, solUsd = 0;
int oldBtcUsd = 0, oldEthUsd = 0, oldSolUsd = 0;
unsigned long previous_cripto_update_Millis = 0;  // Store the last update time
const long updateInterval = 60000;  // Update every 60 seconds

int globalSize;  // Global variab



void handleToggleRelay1() {
  toggleRelay(RELAY_1, relay1State);
  handleRoot();
}

void handleToggleRelay2() {
  toggleRelay(RELAY_2, relay2State);
  handleRoot();
}

template <typename T>
void Clear__old_value(int x, int y, T value, int size, uint16_t bgColor);

template <typename T>
void Write__New_value(int x, int y, T value, int size, uint16_t color);





void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(colorFromHex(0xFF0000), ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.print("Connection lost!");
    tft.setCursor(10, 30);
    tft.print("Reconnecting...");

    Serial.println("Connection lost. Reconnecting...");

    // Attempt to reconnect
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    // Wait for reconnection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");

      // Blink screen or update display to indicate reconnection attempt
      tft.fillScreen(ST77XX_RED);
      delay(300);
      tft.fillScreen(ST77XX_BLACK);
      delay(300);
      tft.setTextColor(colorFromHex(0x00FF00), ST77XX_BLACK);
      tft.setCursor(10, 30);
      tft.print("Reconnecting...");
      delay(300);
    }

    // Connection re-established
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(colorFromHex(0x00FF00), ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.print("Reconnected!");
    tft.setCursor(10, 30);
    tft.print(WiFi.localIP());

    Serial.println("Reconnected.");
    delay(2000);
    tft.fillScreen(ST77XX_BLACK);
    previousHourTens = -1;
    previousHourUnits = -1;
    previousMinuteTens = -1;
    previousMinuteUnits = -1;
    weatherLastTime = -300000;   // Set weatherLastTime to a negative value to fetch data immediately
    weatherTimerDelay = 300000;  // 5 minutes in milliseconds

    temp_C = 0.0;
    humidity = 0;
    wind_speed_float = 0.0;
    sky_clearance = "";

    prev_temp = -1000;
    prev_humidity = -1;
    prev_wind_speed = -1;
    prev_sky = "";
  }
}


// Function to display time at specified position with specified color
void displayTime(int x, int y, uint16_t color) {
  tft.setTextSize(2);
  // Check if it's time to update the display
  static unsigned long lastUpdate = 0;  // Static variable to store last update time
  const long updateInterval = 1000;     // Update interval in milliseconds (1 second)

  unsigned long currentMillis = millis();
  // if (currentMillis - lastUpdate >= updateInterval) {
  // Update the time from the NTP server
  timeClient.update();

  // Get the current time
  int currentHour24 = timeClient.getHours();
  int currentHour = (currentHour24 % 12) ? (currentHour24 % 12) : 12;  // Convert 24h to 12h format
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  // Extract individual digits
  int currentHourTens = currentHour / 10;
  int currentHourUnits = currentHour % 10;
  int currentMinuteTens = currentMinute / 10;
  int currentMinuteUnits = currentMinute % 10;
  int currentSecondTens = currentSecond / 10;
  int currentSecondUnits = currentSecond % 10;

  // Display hours
  tft.setCursor(x, y);
  tft.setTextColor(color);
  if (currentHourTens != previousHourTens) {
    tft.fillRect(x, y, 12, 16, ST77XX_BLACK);
    tft.print(currentHourTens);
    previousHourTens = currentHourTens;
  }
  x += 12;  // Move cursor for next digit
  tft.setCursor(x, y);
  if (currentHourUnits != previousHourUnits) {
    tft.fillRect(x, y, 12, 16, ST77XX_BLACK);
    tft.print(currentHourUnits);
    previousHourUnits = currentHourUnits;
  }

  // Display separator " : "
  x += 1;  // Move cursor before separator
  tft.setCursor(x, y);

  // Toggle separator visibility every second
  if (currentMillis - lastSeparatorToggle >= 500) {  // 500ms on, 500ms off (blinking effect)
    if (separatorVisible) {
      tft.print(" : ");  // Print the separator
    } else {
      tft.fillRect(x + 15, y, 5, 16, ST77XX_BLACK);  // Clear the separator area +15 for the offset clearing
    }
    separatorVisible = !separatorVisible;  // Toggle visibility
    lastSeparatorToggle = currentMillis;   // Update last toggle time
  }

  // Display minutes
  x += 24;  // Adjust spacing for minutes
  tft.setCursor(x, y);
  if (currentMinuteTens != previousMinuteTens) {
    tft.fillRect(x, y, 12, 16, ST77XX_BLACK);
    tft.print(currentMinuteTens);
    previousMinuteTens = currentMinuteTens;
  }
  x += 12;
  tft.setCursor(x, y);
  if (currentMinuteUnits != previousMinuteUnits) {
    tft.fillRect(x, y, 12, 16, ST77XX_BLACK);
    tft.print(currentMinuteUnits);
    previousMinuteUnits = currentMinuteUnits;
  }

  /* // Display seconds
    x += 2; // Move cursor before seconds
    tft.setCursor(x, y);
    if (currentSecondTens != previousSecondTens) {
      tft.fillRect(x, y, 12, 16, ST77XX_BLACK);
      tft.print(currentSecondTens);
      previousSecondTens = currentSecondTens;
    }
    x += 12;
    tft.setCursor(x, y);
    if (currentSecondUnits != previousSecondUnits) {
      tft.fillRect(x, y, 12, 16, ST77XX_BLACK);
      tft.print(currentSecondUnits);
      previousSecondUnits = currentSecondUnits;
    }*/

  // Update last update time
  lastUpdate = currentMillis;
  // }
}


void updateWeather() {
  // Construct the API URL for fetching weather data
  String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
  String jsonBuffer = httpGETRequest(serverPath.c_str());  // Perform HTTP GET request
  DynamicJsonDocument doc(1024);                           // Create a JSON document
  deserializeJson(doc, jsonBuffer);                        // Parse the JSON response

  // Parse and update weather data
  temp_C = doc["main"]["temp"].as<float>() - 273.15;              // Get temperature in Celsius
  humidity = doc["main"]["humidity"].as<int>();                   // Get humidity
  wind_speed_float = doc["wind"]["speed"].as<float>();            // Get wind speed
  sky_clearance = doc["weather"][0]["description"].as<String>();  // Get sky condition description

  weatherLastTime = millis();
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  int httpResponseCode = http.GET();
  String payload = "{}";

  if (httpResponseCode > 0) {
    payload = http.getString();
  }

  http.end();
  return payload;
}

float update_temp() {
  return temp_C;
}

int update_humidity() {
  return humidity;
}

float update_wind_speed() {
  return wind_speed_float;
}

String update_sky_clearance() {
  return sky_clearance;
}

void clearOldValue(int x, int y, float value, int decimalPlaces, int textSize, uint16_t textColor) {
  tft.setTextSize(textSize);
  tft.setTextColor(textColor);
  tft.setCursor(x, y);
  tft.print(value, decimalPlaces);
}

void drawNewValue(int x, int y, float value, int decimalPlaces, int textSize, uint16_t textColor) {
  tft.setTextSize(textSize);
  tft.setTextColor(textColor);
  tft.setCursor(x, y);
  tft.print(value, decimalPlaces);
}

void clearOldValue(int x, int y, int value, int textSize, uint16_t textColor) {
  tft.setTextSize(textSize);
  tft.setTextColor(textColor);
  tft.setCursor(x, y);
  tft.print(value);
  }

void drawNewValue(int x, int y, int value, int textSize, uint16_t textColor) {
  tft.setTextSize(textSize);
  tft.setTextColor(textColor);
  tft.setCursor(x, y);
  tft.print(value);
    }




void checkTouchSensors() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read touch sensors
    bool touch1 = digitalRead(TOUCH_SENSOR_1);
    bool touch2 = digitalRead(TOUCH_SENSOR_2);

    // Check if the first touch sensor is activated
    if (touch1 == LOW && lastTouch1State == HIGH && (currentMillis - lastDebounceTime1 > debounceDelay)) {
      lastDebounceTime1 = currentMillis;
      toggleRelay(RELAY_1, relay1State);
    }
    lastTouch1State = touch1;

    // Check if the second touch sensor is activated
    if (touch2 == LOW && lastTouch2State == HIGH && (currentMillis - lastDebounceTime2 > debounceDelay)) {
      lastDebounceTime2 = currentMillis;
      toggleRelay(RELAY_2, relay2State);
    }
    lastTouch2State = touch2;
  }
}

void toggleRelay(int relayPin, bool& relayState) {
  relayState = !relayState;
  digitalWrite(relayPin, relayState ? HIGH : LOW);

  int x = 87; // X position for display
  int y = (relayPin == RELAY_1) ? 60 : 50; // Y position: different for RELAY_1 and RELAY_2
  uint16_t bgColor = ST7735_BLACK; // Background color to clear the old value
  int size = 1; // Text size

  // Calculate the maximum width to clear ("1 HIGH" or "2 HIGH")
  String maxText = "2 HIGH"; // The longest possible text
  int width = maxText.length() * 6 * size;  // Adjust width based on the longest string and text size
  int height = 8 * size;  // Height based on text size

  // Clear the entire area before updating the text
  tft.fillRect(x, y, width, height, bgColor);

  // Prepare the text to display
  String displayText = String(relayPin == RELAY_1 ? "2" : "1") + (relayState ? " LOW" : " HIGH");
  
  uint16_t color = relayState ? ST7735_GREEN : ST7735_RED;

  Write__New_value(x, y, displayText, size, color);
}

void handleRoot() {
  // Determine relay states
  bool relay1State = getRelay1State();
  bool relay2State = getRelay2State();

  // Set button classes based on relay states
  String relay1Class = relay1State ? "btn btn-outline-warning" : "btn btn-outline-secondary";
  String relay2Class = relay2State ? "btn btn-outline-warning" : "btn btn-outline-secondary";

  String html = R"rawliteral(








<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 Relay Control with Weather Info</title>
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <style>
        body {
            padding-top: 56px;
            background: url('https://static.vecteezy.com/system/resources/previews/025/733/493/large_2x/hexagonal-abstract-technology-background-electric-glow-hexagonal-background-illustration-vector.jpg') no-repeat center center fixed;
            background-size: cover;
            color: #ffffff;
        }

        footer {
            background-color: #003366;
            color: #ffffff;
            padding: 10px;
            text-align: center;
            position: fixed;
            bottom: 0;
            width: 100%;
        }

        .navbar {
            background-color: #003366;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }

        .container {
            margin-top: 20px;
            margin-bottom: 80px; /* Add bottom margin for footer space */
            width: 90%;
            max-width: 800px;
        }

        .list-group-item {
            color: #ffffff;
            background-color: #003366;
            border: 1px solid #0056b3;
        }

            .list-group-item.active {
                background-color: #0056b3;
                border-color: #0056b3;
            }

        @media (max-width: 575px) {
            .container {
                max-width: 100%;
            }
        }
    </style>
</head>
<body>
    <nav class="navbar navbar-expand-lg navbar-dark fixed-top">
        <a class="navbar-brand" href="#">ESP8266 Control</a>
        <ul class="navbar-nav ml-auto">
            <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="Dropdown" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                    XXXXX
                </a>
                <div class="dropdown-menu" aria-labelledby="Dropdown">
                    <a class="dropdown-item" href="#" id="current-weather">XXXXX1</a>
                </div>
            </li>
        </ul>
    </nav>

    <div class="container">
        <div class="list-group">
            <a href="#" class="list-group-item list-group-item-action active">Relay Control</a>
            <button type="button" class="btn btn-outline-warning" onclick="location.href='/toggleRelay1'">Light 1</button>
            <button type="button" class="btn btn-outline-warning" onclick="location.href='/toggleRelay2'">Light 2</button>
        </div>

        <div class="list-group mt-3">
            <a href="#" class="list-group-item list-group-item-action active">Current Weather</a>
            <a href="#" class="list-group-item list-group-item-action" id="current-weather-details">Loading...</a>
        </div>
    </div>

    <footer>
        <p>&copy; 2024 ESP8266 Project. All rights reserved Eng Mohamad Adnan Jamal.</p>
    </footer>

    <script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.3/dist/umd/popper.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
    <script>
        $(document).ready(function () {
            var apiKey = '98f8bb3e2d29c673b5b21208ddf9da9e';
            var city = 'Maltepe'; // You can change this to any city you want

            // Function to fetch and update weather information
            function updateWeather() {
                $.ajax({
                    url: `https://api.openweathermap.org/data/2.5/forecast?q=${city}&appid=${apiKey}&units=metric`,
                    method: 'GET',
                    success: function (data) {
                        console.log('Weather data:', data); // Log the entire data for debugging

                        // Today's weather (first entry)
                        var today = data.list[0];
                        var currentWeatherHtml = `
                                    <strong>Description:</strong> ${today.weather[0].description}<br>
                                    <strong>Temperature:</strong> ${today.main.temp} &deg;C<br>
                                    <strong>Feels Like:</strong> ${today.main.feels_like} &deg;C<br>
                                    <strong>Humidity:</strong> ${today.main.humidity} %<br>
                                    <strong>Wind Speed:</strong> ${today.wind.speed} m/s<br>
                                    <strong>Pressure:</strong> ${today.main.pressure} hPa
                                `;
                        $('#current-weather-details').html(currentWeatherHtml);
                    },
                    error: function (error) {
                        console.log('Error fetching weather data:', error);
                    }
                });
            }

            updateWeather(); // Call the function on page load
        });
    </script>
</body>
</html>








)rawliteral";

  // Send the HTML content
  server.send(200, "text/html", html);
}


void displayWether(int temp_pozition_X, int temp_pozition_y) {
  float temp = round(update_temp() * 10) / 10.0;  // Round to one decimal place
  int hum = update_humidity();
  float wind_speed = update_wind_speed();
  String sky = update_sky_clearance();

  // Update only if the value has changed
  if (temp != prev_temp) {

    clearOldValue(temp_pozition_X, temp_pozition_y, prev_temp, 1, 2, ST77XX_BLACK);
    drawNewValue(temp_pozition_X, temp_pozition_y, temp, 1, 2, ST77XX_GREEN);

    tft.setTextSize(1);
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(temp_pozition_X + 47, temp_pozition_y + 8);
    tft.print("c");

    prev_temp = temp;
  }

  if (hum != prev_humidity) {
    clearOldValue(100, 30, prev_humidity, 0, 2, ST77XX_BLACK);
    drawNewValue(100, 30, hum, 0, 2, ST77XX_GREEN);
    prev_humidity = hum;
  }

  if (wind_speed != prev_wind_speed) {
    clearOldValue(75, 0, prev_wind_speed, 1, 2, ST77XX_BLACK);
    drawNewValue(75, 0, wind_speed, 1, 2, ST77XX_GREEN);
    prev_wind_speed = wind_speed;
    /*
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_GREEN);
      tft.setCursor(110, 7 );
      tft.print("mc");
      */
  }
}

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}



template <typename T>
void Clear__old_value(int x, int y, T value, int size, uint16_t bgColor) {
  String strValue = String(value);  // Convert the value to a string
  int width = strValue.length() * 6 * size;  // Calculate width based on the length of the value and size
  int height = 8 * size;  // Height based on text size
  tft.fillRect(x, y, width, height, bgColor);  // Clear the area by filling it with the background color
}

template <typename T>
void Write__New_value(int x, int y, T value, int size, uint16_t color) {
  globalSize = size;  // Store the size in the global variable
  tft.setTextColor(color);
  tft.setTextSize(globalSize);  // Set text size
  tft.setCursor(x, y);
  tft.print(String(value));  // Print the value
}

String formatPrice(int price) {
  String priceStr = String(price);
  int len = priceStr.length();
  
  if (len > 3) {
    priceStr = priceStr.substring(0, len - 3) + "." + priceStr.substring(len - 3);
  }
  
  return priceStr;
}

void Display_cripto_BTC_Prices(int x, int y, int size, uint16_t valueColor, uint16_t bgColor) {
  String formattedOldBtcUsd = formatPrice(oldBtcUsd);
  String formattedBtcUsd = formatPrice(btcUsd);

  if (btcUsd != oldBtcUsd) {
    Clear__old_value(x, y, formattedOldBtcUsd, size, bgColor);  // Clear old BTC value with correct size
    Write__New_value(x, y, formattedBtcUsd, size, valueColor);  // Display new BTC value
    oldBtcUsd = btcUsd;
  }
}

void Display_cripto_ETH_Prices(int x, int y, int size, uint16_t valueColor, uint16_t bgColor) {
  String formattedOldEthUsd = formatPrice(oldEthUsd);
  String formattedEthUsd = formatPrice(ethUsd);

  if (ethUsd != oldEthUsd) {
    Clear__old_value(x, y, formattedOldEthUsd, size, bgColor);  // Clear old ETH value with correct size
    Write__New_value(x, y, formattedEthUsd, size, valueColor);  // Display new ETH value
    oldEthUsd = ethUsd;
  }
}

void Display_cripto_SOL_Prices(int x, int y, int size, uint16_t valueColor, uint16_t bgColor) {
  String formattedOldSolUsd = formatPrice(oldSolUsd);
  String formattedSolUsd = formatPrice(solUsd);

  if (solUsd != oldSolUsd) {
    Clear__old_value(x, y, formattedOldSolUsd, size, bgColor);  // Clear old SOL value with correct size
    Write__New_value(x, y, formattedSolUsd, size, valueColor);  // Display new SOL value
    oldSolUsd = solUsd;
  }
}

void updatePrices() {
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();

  http.begin(client, "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum,solana&vs_currencies=usd&include_24hr_change=true");
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      btcUsd = doc["bitcoin"]["usd"].as<int>();
      ethUsd = doc["ethereum"]["usd"].as<int>();
      solUsd = doc["solana"]["usd"].as<int>();
      Serial.println("Prices updated");
    } else {
      Serial.println("Failed to parse JSON");
    }
  } else {
    Serial.print("Failed to connect: ");
    Serial.println(httpCode);
  }

  http.end();
}



void setup() {

  pinMode(TOUCH_SENSOR_1, INPUT);
  pinMode(TOUCH_SENSOR_2, INPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  digitalWrite(RELAY_1, LOW);  // Ensure relays start in the OFF state
  digitalWrite(RELAY_2, LOW);

  Serial.begin(115200);
  timeClient.begin();
  delay(10);

  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  tft.initR(INITR_GREENTAB);  // Try INITR_BLACKTAB, INITR_GREENTAB, INITR_REDTAB to see which works
  tft.setRotation(2);         // Set the rotation of the display (0 to 3)
  tft.fillScreen(ST77XX_BLACK);

  WiFi.begin(ssid, password);

  tft.setFont(&FreeMono9pt7b);  // Set the custom font
  tft.setTextSize(1);

  // Seed the random number generator using a floating analog pin
  randomSeed(analogRead(0));

  const char* text = "Connecting..";
  int i = 0;
  int cursorX = 0;
  tft.setCursor(cursorX, 23);

  // Print each character in a random color
  while (text[i] != '\0') {
    uint32_t randomColor = random(0x000000, 0xFFFFFF);
    tft.setTextColor(colorFromHex(randomColor), ST77XX_BLACK);
    tft.print(text[i]);
    cursorX += 11;  // Move cursor to the right for the next character (adjust this value if needed)
    tft.setCursor(cursorX, 23);
    i++;
  }

  // Draw WiFi icon at (50, 50)
  int iconX = 50;
  int iconY = 50;
  int iconWidth = 0;
  int iconHeight = 19;  // Fixed height for the provided icon

  // Calculate icon width dynamically
  for (int y = 0; y < iconHeight; y++) {
    for (int x = 0; x < 3; x++) {
      if (pgm_read_byte(&wifiIcon[y * 3 + x]) != 0xff) {
        iconWidth = (y * 3 + x + 1) * 8;  // Each byte represents 8 pixels wide
      }
    }
    if (iconWidth > 0) break;  // Break if width is found
  }

  // Draw WiFi icon dynamically
  for (int y = 0; y < iconHeight; y++) {
    for (int x = 0; x < iconWidth / 8; x++) {
      uint8_t pixelRow = pgm_read_byte(&wifiIcon[y * (iconWidth / 8) + x]);
      for (int i = 0; i < 8; i++) {
        if (pixelRow & (1 << (7 - i))) {
          tft.drawPixel(iconX + x * 8 + i, iconY + y, ST77XX_WHITE);
        } else {
          tft.drawPixel(iconX + x * 8 + i, iconY + y, ST77XX_BLACK);
        }
      }
    }
  }

  // Display SSID
  tft.setFont(NULL);
  tft.setTextColor(colorFromHex(0xFF0000), ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(25, 80);  // Adjust Y position as needed
  tft.print(ssid);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    tft.setFont(NULL);
    tft.setTextColor(colorFromHex(0x00FF4D), ST77XX_BLACK);
    tft.setTextSize(1);
    for (int i = 25; i < 85; i++) {
      tft.setCursor(i, 100);
      tft.print("=>");
    }
  }

  // Display connection status with color changes
  tft.fillScreen(ST77XX_RED);
  delay(300);
  tft.fillScreen(ST77XX_GREEN);
  delay(300);
  tft.fillScreen(ST77XX_BLUE);
  delay(300);
  tft.fillScreen(ST77XX_BLACK);
  delay(300);

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggleRelay1", handleToggleRelay1);
  server.on("/toggleRelay2", handleToggleRelay2);
  server.begin();
  // Display WiFi connection status and IP address
  tft.setFont(NULL);
  tft.setTextColor(colorFromHex(0xFF0000), ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(25, 10);  // Adjust Y position as needed
  tft.print("WiFi connected");
  tft.setCursor(25, 30);  // Adjust Y position as needed
  tft.print(WiFi.localIP());

  // Draw the new icon at (50, 50)
  int SmilyiconX = 50;
  int SmilyiconY = 50;
  int SmilyiconWidth = 5;    // Each row has 5 bytes (5 * 8 = 40 pixels)
  int SmilyiconHeight = 33;  // Total of 33 rows

  // Draw the new icon dynamically
  for (int y = 0; y < SmilyiconHeight; y++) {
    for (int x = 0; x < SmilyiconWidth; x++) {
      uint8_t pixelRow = pgm_read_byte(&newIcon[y * SmilyiconWidth + x]);
      for (int i = 0; i < 8; i++) {
        if (pixelRow & (1 << (7 - i))) {
          tft.drawPixel(SmilyiconX + x * 8 + i, SmilyiconY + y, ST77XX_YELLOW);
        } else {
          tft.drawPixel(SmilyiconX + x * 8 + i, SmilyiconY + y, ST77XX_BLACK);
        }
      }
    }
  }
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  updateWeather();
  updatePrices();

}



void loop() {
  checkWiFiConnection();
  // tft.fillRect(5, 22, 60, 2, ST77XX_RED);
  displayTime(0, 0, ST77XX_GREEN);
  if ((millis() - weatherLastTime) > weatherTimerDelay) {
    updateWeather();
  }
  displayWether(2, 30);
  server.handleClient();
  checkTouchSensors();

  unsigned long current_Cripto_Millis = millis(); 
    if (current_Cripto_Millis - previous_cripto_update_Millis >= updateInterval) {
    previous_cripto_update_Millis = current_Cripto_Millis;  // Save the time of the last update
    
    updatePrices();
  }
  Display_cripto_BTC_Prices(0, 55, 2, ST77XX_GREEN, ST77XX_BLACK);
  Display_cripto_ETH_Prices(0, 75, 2, ST77XX_GREEN, ST77XX_BLACK);
  Display_cripto_SOL_Prices(0, 95, 2, ST77XX_GREEN, ST77XX_BLACK);

}
