/*
 *    Copyright 2025 UDFOwner
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *
 *    More details: https://udfsoft.com/
 */

#include "DHT.h"

#include <WiFi.h>
#include <WiFiManager.h>  // lib tzapu/WiFiManager

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define APP_VERSION "1"
#define DEVICE_ID "xxxx-xxxx-xxxx-xxxx-xxxx" // YOUR DEVICE ID, to get it write to us: support@udfsoft.com

#define ENDPOINT_URL "https://smart.udfsoft.com/api/temperature/add"

#define API_KEY  ""xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"" // YOUR API Key, to get it write to us: support@udfsoft.com

#define LED_PIN 8

#define DHTPIN 2       // GPIO2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

unsigned long lastBlink = 0;
unsigned long lastSend = 0;

const unsigned long BLINK_INTERVAL = 5000;  // 5 sec
const unsigned long SEND_INTERVAL = 60000;   // 1 min

bool ledState = false;

void setup() {
  setupDhtSensor();

  printDeviceInfo();

  setupLedPin();
  setupWifi();
}

void printDeviceInfo() {

  Serial.println(" ======================= ");
  Serial.println("   Hi, I'm smart device)");
  Serial.println("   Device ID: " + String(DEVICE_ID));
  Serial.println("   App version: " + String(APP_VERSION));
  Serial.println(" ======================= ");
}

void setupDhtSensor() {
  Serial.begin(115200);
  dht.begin();
}

void setupLedPin() {
  pinMode(LED_PIN, OUTPUT);
}

void setupWifi() {
  WiFiManager wm;

  //for test
  // wm.resetSettings();

  wm.setConnectTimeout(120);  // 2 mins
  wm.setConfigPortalTimeout(300);

  // If the connection fails, the configurator will start
  if (!wm.autoConnect("ESP32_AP", "12345678")) {
    Serial.println("Failed to connect, rebooting...");
    ESP.restart();
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  unsigned long now = millis();

  if (now - lastBlink >= BLINK_INTERVAL) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    Serial.println("Blink!");
  }

  if (now - lastSend >= SEND_INTERVAL) {
    lastSend = now;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {

      sendSensorDataToWeb(temp, hum);

      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" °C  |  Humidity: ");
      Serial.print(hum);
      Serial.println(" %");
    } else {
      Serial.println("Reading error with DHT11!");
    }
  }

  delay(500);
}

void sendSensorDataToWeb(float humidity, float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();  // ⚠️ disable certificate verification

    HTTPClient https;

    https.setTimeout(15000);
    https.begin(client, ENDPOINT_URL);
    https.addHeader("Content-Type", "application/json");
    https.addHeader("X-Api-Key", String(API_KEY));
    https.addHeader("X-DEVICE-ID", String(DEVICE_ID));

    https.addHeader("X-APP-VERSION", String(APP_VERSION));

    String body = "{";
    body += "\"deviceId\":\"" + String(DEVICE_ID) + "\",";
    body += "\"temperature\":" + String(temperature, 1) + ",";
    body += "\"humidity\":" + String(humidity, 1);
    body += "}";

    Serial.print("Body: ");
    Serial.println(body);


    int httpResponseCode = https.POST(body);

    Serial.print("HTTPS Response code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      String response = https.getString();
      Serial.println("Response: " + response);
    }

    https.end();
  } else {
    Serial.println("WiFi is not connected");
  }
}

