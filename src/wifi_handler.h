#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <WiFi.h>

// Function to start WiFi connection process
void connectToWiFi();
bool checkWiFiConnected();
void waitForWiFi(unsigned long* startWifiAttempt);
void ensureWiFiConnected();

// Global flag to check WiFi connection status
extern bool wifiConnected;

#endif // WIFI_HANDLER_H
