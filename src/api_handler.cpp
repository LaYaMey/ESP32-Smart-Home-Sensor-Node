#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "api_handler.h"
#include "error_handler.h"
#include "display_handler.h"

static ApiConfig apiConfig;

void setApiConfig(const ApiConfig& config) {
  apiConfig = config;
  displayMessage("API configured" + apiConfig.serverIP);
}

IPAddress mDNSResolveIP(const String& domain_name) {
  IPAddress server_ip = IPAddress(0,0,0,0);

  int attempts = 3;
  displayMessage("Resolving mDNS to IP");
  while (attempts > 0){
    attempts--;
    WiFi.hostByName(domain_name.c_str(), server_ip);
    if (server_ip != IPAddress(0,0,0,0)){
      return server_ip;
    }
  }
  displayMessage("mDNS resolution failed");
  return server_ip;
}

bool postSensorData(float value, const String& measurement, const String& extraTagKey, const String&  extraTagValue) {
  if (WiFi.status() != WL_CONNECTED) {
    signalError(WIFI_ERROR);
    displayMessage("WiFi Error");
    return false;
  }

  HTTPClient http;
  String url = "http://" + String(apiConfig.serverIP) + ":8000/submit";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  // Use ArduinoJson for safer JSON construction
  StaticJsonDocument<256> json;
  json["measurement"] = measurement;
  json["value"] = value;

  JsonObject tags = json.createNestedObject("tags");
  tags["sensorname"] = apiConfig.sensorId;

  if (!extraTagKey.isEmpty() && !extraTagValue.isEmpty()) {
    tags[extraTagKey] = extraTagValue;
  }

  String requestBody;
  serializeJson(json, requestBody);

  int responseCode = http.POST(requestBody);
  http.end();

  if (responseCode < 200 || responseCode >= 300) {
    signalError(POST_ERROR);
    //displayMessage("API Error");

    String msg = "HTTP " + String(responseCode);
    displayMessage(msg);
    delay(5000);
    return false;
  }
  //displayMessage("API Success");
  return true;
}
