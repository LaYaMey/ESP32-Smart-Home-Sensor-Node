#include <WiFi.h>

#include "display_handler.h"
#include "wifi_handler.h"
#include "error_handler.h"

#include "secrets.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;


bool wifiConnected = false;

void connectToWiFi() {
  WiFi.begin(ssid, password);  // Use the credentials directly here
}

bool checkWiFiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    return true;
  } else {
    return false;
  }
}

void waitForWiFi(unsigned long* startWifiAttempt) {
  while (!checkWiFiConnected() && millis() - *startWifiAttempt < 15000) {
    displayMessage("Waiting for WiFi");
    delay(500);
  }
  if (!wifiConnected) {
    signalError(WIFI_ERROR); // Only signal if it actually failed
    displayMessage("WiFi timeout");
    delay(1000);
  } else {
    displayMessage("WiFi connected");
  }
}

void ensureWiFiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    displayMessage("Reconnecting WiFi...");
    unsigned long wifiStart = millis();
    connectToWiFi();
    waitForWiFi(&wifiStart);
  }
}
