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
#include <WiFiManager.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define API_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  // YOUR API Key, to get it write to us: support@udfsoft.com
#define DEVICE_ID "xxxx-xxxx-xxxx-xxxx-xxxx"  // YOUR DEVICE ID, to get it write to us: support@udfsoft.com


#define LED_PIN 8

#define DHTPIN 2 // GPIO2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  setupDhtSensor();
  setupLedPin();
  setupWifi();
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

  // wm.resetSettings();

  if (!wm.autoConnect("ESP32_AP", "12345678")) {
    Serial.println("Failed to connect, rebooting...");
    ESP.restart();
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Reading error with DHT11!");
    return;
  }

  sendSensorDataToWeb(h, t);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  digitalWrite(LED_PIN, HIGH);

  delay(2000);

  digitalWrite(LED_PIN, LOW);
  delay(28000);
}

void sendSensorDataToWeb(float humidity, float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    https.begin(client, "https://smart.udfsoft.com/api/temperature/add");
    https.addHeader("Content-Type", "application/json");
    https.addHeader("X-Api-Key", String(API_KEY));
    https.addHeader("X-DEVICE-ID", String(DEVICE_ID));


    // формируем JSON
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
    Serial.println("WiFi is not connected!");
  }
}
