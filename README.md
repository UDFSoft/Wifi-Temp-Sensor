# 🌡️ WiFi Temperature Sensor (ESP32 + DHT11)

[official page](https://udfsoft.com/) 

A simple IoT temperature and humidity sensor based on **ESP32-C3** and **DHT11**.  
The device measures temperature and humidity, sends the data to a backend server, and from there it can be forwarded, for example, to a Telegram bot.

## 📌 Features
- Reads temperature and humidity from the DHT11 sensor
- Connects to Wi-Fi using [WiFiManager](https://github.com/tzapu/WiFiManager) (automatic configurator)
- Sends data to a server via HTTPS (with `X-Api-Key` and `X-Device-Id` headers)
- LED indicator while sending data
- Serial Monitor logging for debugging

## 🛠️ Hardware
- **ESP32-C3** (ESP8266/ESP32 can also be used with minor pin adjustments)
- **DHT11** sensor (can be replaced with DHT22 for better accuracy)
- LED indicator (`LED_PIN`)

## ⚙️ Setup
1. Install required libraries in Arduino IDE / PlatformIO:
   - `DHT sensor library`
   - `WiFiManager`
   - `WiFiClientSecure`
   - `HTTPClient`
2. Set your values in the code:
   ```cpp
   #define API_KEY "your_api_key"
   #define DEVICE_ID "your_device_id"
   ```
3. Flash the firmware to the ESP32-C3.
4. On the first run, the device will create an access point ESP32_AP with password 12345678 — connect and configure your Wi-Fi credentials.

## 🚀 How it works

1. ESP32 connects to Wi-Fi.

2. Every 30 seconds, it reads data from the DHT11.

3. Data is sent to the server via a POST request in JSON format:

 ```json
{
  "deviceId": "f8b2f8c4-9f31-4f0b-8f9c-f6e63c6b1d72",
  "temperature": 24.5,
  "humidity": 55.0
}
 ```

## 📡 Integration example

The backend receives the data and forwards it to a Telegram bot, so you can easily check the temperature and humidity in your room.


## 📷 Photo (example)


## 🔧 Author: udfowner
