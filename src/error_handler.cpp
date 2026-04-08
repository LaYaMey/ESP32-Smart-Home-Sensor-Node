#include <Arduino.h>

#include "display_handler.h"
#include "error_handler.h"

ErrorType currentError = NO_ERROR;
unsigned long lastBlinkTime = 0;
int blinkCount = 0;
int patternIndex = 0;
bool ledState = false;
bool blinking = false;

const int ledPin = 15;

void setupBlinkLed() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void signalError(ErrorType error) {
  currentError = error;
  blinkCount = 0;
  blinking = true;
  ledState = false;
  lastBlinkTime = millis();
}


void handleBlinkPattern() {
  if (!blinking || currentError == NO_ERROR) return;

  unsigned long now = millis();

  // Define blink patterns for each error
  static const int wifiPattern[] = {250, 250, 250, 250, 250, 1000}; // 3 short blinks
  static const int postPattern[] = {250, 250, 250, 1000};           // 2 short blinks
  static const int sensorPattern[] = {500, 250, 250, 250, 250, 1000}; // 1 long, 2 short
  static const int displayPattern[] = {500, 250, 250, 1000}; // 1 long, 1 short


  const int *pattern = nullptr;
  int patternLength = 0;

  switch (currentError) {
    case WIFI_ERROR:
      pattern = wifiPattern;
      patternLength = sizeof(wifiPattern) / sizeof(wifiPattern[0]);
      break;
    case POST_ERROR:
      pattern = postPattern;
      patternLength = sizeof(postPattern) / sizeof(postPattern[0]);
      break;
    case SENSOR_ERROR:
      pattern = sensorPattern;
      patternLength = sizeof(sensorPattern) / sizeof(sensorPattern[0]);
      break;
    case DISPLAY_ERROR:
      pattern = displayPattern;
      patternLength = sizeof(displayPattern) / sizeof(displayPattern[0]);
      break;
    default:
      blinking = false;
      digitalWrite(ledPin, LOW);
      return;
    }

  if (now - lastBlinkTime >= pattern[patternIndex]) {
    // Toggle LED
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    lastBlinkTime = now;
    blinkCount++;

    if (blinkCount >= patternLength) {
      // Done with this round of blinking, pause before repeat
      blinkCount = 0;
    }
  }
}
