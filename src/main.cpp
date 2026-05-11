#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include <Preferences.h>

#include <HTTPClient.h>
#include <Adafruit_SHT4x.h>

#include "error_handler.h"
#include "api_handler.h"
#include "time_handler.h"
#include "wifi_handler.h"
#include "display_handler.h"


// Server Address or Domain
const String server_domain = "orangepi5.local";

// Measurement interval in seconds
const int interval_seconds = 5 * 60;  // 5 minutes
int lastPostInterval = -1;

const int time_sync_interval = 3*60*60; // 3 h
const int syncOffset = interval_seconds/5;
int lastTimeSyncInterval = 0;

// Loop interval in ms
const unsigned long loop_interval_ms = 250; 

// Button Pin for switching display view
constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_8;

// I2C pins
#define I2C_SDA 35
#define I2C_SCL 33

// Analog sensor pin
const int analogPin = 1;

// SHT40 sensor
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

volatile bool buttonPressed = false;
volatile DisplayMode currentDisplayMode = MODE_TIME;

unsigned long lastButtonPress = 0;  // Time of last accepted press
const unsigned long debounceDelay = 1000; // 1 second

void IRAM_ATTR onButtonPressISR() {
  buttonPressed = true;
}

// Add Buffers for values
const int BUFFER_SIZE = 11;
float tempBuffer[BUFFER_SIZE] = {0};
float humidityBuffer[BUFFER_SIZE] = {0};
float soilBuffer[BUFFER_SIZE] = {0};
int bufferIndex = 0;
bool bufferFilled = false;

// Insert value into a buffer at current index
void insertIntoBuffer(float buffer[], float value) {
  buffer[bufferIndex] = value;
}

// Copy, sort, and return median
float getMedian(float buffer[]) {
  float temp[BUFFER_SIZE];
  int count = bufferFilled ? BUFFER_SIZE : bufferIndex;
  for (int i = 0; i < count; i++) temp[i] = buffer[i];

  // Simple bubble sort (efficient enough for 7 values)
  for (int i = 0; i < count - 1; i++) {
    for (int j = 0; j < count - i - 1; j++) {
      if (temp[j] > temp[j + 1]) {
        float t = temp[j];
        temp[j] = temp[j + 1];
        temp[j + 1] = t;
      }
    }
  }
  return temp[count / 2];
}


//void IRAM_ATTR onButtonPressISR() {
//  currentDisplayMode = static_cast<DisplayMode>((currentDisplayMode + 1) % NUM_DISPLAY_MODES);
//}


void setup() {
  pinMode(BUTTON_PIN, INPUT);  // Button active LOW assumed
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPressISR, RISING);

  Wire.begin(I2C_SDA, I2C_SCL);
  setupBlinkLed();
  setupDisplay();
  displayMessage("Starting...");

  connectToWiFi();
  unsigned long startWifiAttempt = millis();

  // SHT40 init
  if (!sht4.begin(&Wire)) {
    displayMessage("SHT40 not found");
    delay(2000);
    signalError(SENSOR_ERROR);
  }
  sht4.setHeater(SHT4X_NO_HEATER);
  displayMessage("SHT40 initiated");

  waitForWiFi(&startWifiAttempt);
    
  syncTime();

  IPAddress server_ip = mDNSResolveIP(server_domain);

  static ApiConfig apiConfig = {
    server_ip.toString(),             // FastAPI serverIP
    "esp_1_larsisZimmer"          // Sensor tag 
  };  
  setApiConfig(apiConfig);
}

void loop() {
  unsigned long loopStart = millis();

  // Always measure sensors
  sensors_event_t humidityRaw, tempRaw;
  sht4.getEvent(&humidityRaw, &tempRaw);
  float rawValueRaw = analogRead(analogPin);

  // Insert values into buffers
  insertIntoBuffer(tempBuffer, tempRaw.temperature);
  insertIntoBuffer(humidityBuffer, humidityRaw.relative_humidity);
  insertIntoBuffer(soilBuffer, rawValueRaw);

  // Advance index and wrap
  bufferIndex++;
  if (bufferIndex >= BUFFER_SIZE) {
    bufferIndex = 0;
    bufferFilled = true;
  }

  // Calculate median values
  float tempMedian = getMedian(tempBuffer);
  float humidityMedian = getMedian(humidityBuffer);
  float soilMedian = getMedian(soilBuffer);

  time_t now;
  getUnixTimestamp(&now);
  int currentInterval = now / interval_seconds;

  if (currentInterval != lastPostInterval) {
    ensureWiFiConnected();
    postSensorData(tempMedian, "temperature", "sensornumber", "1");
    postSensorData(humidityMedian, "humidity", "sensornumber", "1");
    postSensorData(soilMedian, "soilmoisture", "sensornumber", "1");

    lastPostInterval = currentInterval;
  }

  int currentTimeSyncInterval = (now-syncOffset) / interval_seconds;

  if (currentTimeSyncInterval != lastTimeSyncInterval) {
    attemptResyncTime();
    lastTimeSyncInterval = currentTimeSyncInterval;
  }

  if (buttonPressed) {
    unsigned long nowMillis = millis();
    if (nowMillis - lastButtonPress >= debounceDelay) {
      currentDisplayMode = static_cast<DisplayMode>((currentDisplayMode + 1) % NUM_DISPLAY_MODES);
      lastButtonPress = nowMillis;
    }
    buttonPressed = false;  // Always clear flag
  }
  updateDisplay(currentDisplayMode, humidityMedian, tempMedian, now);  // If you modify display handler
  // Wait until 250ms from loop start has passed
  unsigned long elapsed = millis() - loopStart;
  if (elapsed < loop_interval_ms) {
    delay(loop_interval_ms - elapsed);
  }
}






