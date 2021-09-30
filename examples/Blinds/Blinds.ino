/*
 * Example for how to use SinricPro Blinds device
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

#include <SinricProBlinds.h>

#define WIFI_SSID  "YOUR-WIFI-SSID"
#define WIFI_PASS  "YOUR-WIFI-PASSWORD"
#define APP_KEY    "YOUR-APP-KEY"     // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "YOUR-APP-SECRET"  // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define BLINDS_ID  "YOUR-DEVICE-ID"   // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE  9600               // Change baudrate to your need

SinricProBlinds myBlinds(BLINDS_ID);
int             blindsPosition = 0;
bool            powerState = false;

bool onPowerState(const String &deviceId, bool &state) {
    Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state ? "on" : "off");
    powerState = state;
    return true;  // request handled properly
}

bool onRangeValue(const String &deviceId, int &position) {
    Serial.printf("Device %s set position to %d\r\n", deviceId.c_str(), position);
    return true;  // request handled properly
}

bool onAdjustRangeValue(const String &deviceId, int &positionDelta) {
    blindsPosition += positionDelta;
    Serial.printf("Device %s position changed about %i to %d\r\n", deviceId.c_str(), positionDelta, blindsPosition);
    positionDelta = blindsPosition;  // calculate and return absolute position
    return true;                     // request handled properly
}

// setup function for WiFi connection
void setupWiFi() {
    Serial.printf("\r\n[Wifi]: Connecting");
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.printf(".");
        delay(250);
    }
    IPAddress localIP = WiFi.localIP();
    Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
    myBlinds.onPowerState(onPowerState);
    myBlinds.onRangeValue(onRangeValue);
    myBlinds.onAdjustRangeValue(onAdjustRangeValue);

    // setup SinricPro
    SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
    SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });
    SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
    Serial.begin(BAUD_RATE);
    Serial.printf("\r\n\r\n");
    setupWiFi();
    setupSinricPro();
}

void loop() {
    SinricPro.handle();
}