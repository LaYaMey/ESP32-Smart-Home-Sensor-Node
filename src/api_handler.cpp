#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "api_handler.h"
#include "error_handler.h"
#include "display_handler.h"

static ApiConfig apiConfig;

void setApiConfig(const ApiConfig& config) {
  apiConfig = config;
}

bool postSensorData(float value, const char* measurement, const char* extraTagKey, const char* extraTagValue) {
  if (WiFi.status() != WL_CONNECTED) {
    signalError(WIFI_ERROR);
    displayMessage("WiFi Error");
    return false;
  }

  HTTPClient http;
  String url = String(apiConfig.baseUrl) + "/submit";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  // Use ArduinoJson for safer JSON construction
  StaticJsonDocument<256> json;
  json["measurement"] = measurement;
  json["value"] = value;

  JsonObject tags = json.createNestedObject("tags");
  tags["sensorname"] = apiConfig.sensorId;

  if (extraTagKey && extraTagValue) {
    tags[extraTagKey] = extraTagValue;
  }

  String requestBody;
  serializeJson(json, requestBody);

  int responseCode = http.POST(requestBody);
  http.end();

  if (responseCode < 200 || responseCode >= 300) {
    signalError(POST_ERROR);
    displayMessage("API Error");
    return false;
  }
  //displayMessage("API Success");
  return true;
}
