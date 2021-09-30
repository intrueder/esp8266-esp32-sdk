/*
 * Example for smart lock
 * 
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG_ESP_PORT Serial
#define NODEBUG_WEBSOCKETS
#define NDEBUG
#endif

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif

#include <SinricProLock.h>

#define WIFI_SSID  "YOUR_WIFI_SSID"
#define WIFI_PASS  "YOUR_WIFI_PASSWORD"
#define APP_KEY    "YOUR_APP_KEY_HERE"     // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "YOUR_APP_SECRET_HERE"  // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define LOCK_ID    "YOUR_DEVICE_ID_HERE"   // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE  9600                    // Change baudrate to your need

SinricProLock myLock(LOCK_ID);

bool onLockState(String deviceId, bool &lockState) {
    Serial.printf("Device %s is %s\r\n", deviceId.c_str(), lockState ? "locked" : "unlocked");
    return true;
}

void setupWiFi() {
    Serial.printf("\r\n[Wifi]: Connecting");
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.printf(".");
        delay(250);
    }
    Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
    myLock.onLockState(onLockState);

    // setup SinricPro
    SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
    SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });
    SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
    Serial.begin(BAUD_RATE);
    Serial.printf("\r\n\r\n");
    setupWiFi();
    setupSinricPro();
}

void loop() {
    SinricPro.handle();
}
