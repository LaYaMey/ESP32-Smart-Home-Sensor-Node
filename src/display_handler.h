#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <Adafruit_SSD1306.h>
#include <Adafruit_SHT4x.h>
#include <Wire.h>
#include <time.h>

// Define display modes here so they are shared
enum DisplayMode {
  MODE_OFF = 0,
  MODE_TIME,
  MODE_TEMPHUM,
  MODE_ALL,
  NUM_DISPLAY_MODES
};

void setupDisplay();
void displayOff();
void displayOn();
void displayValues(float humidity, float temp);
void displayMessage(const String& message);  // Optional helper for text messages
void displayTime(const struct tm* timeinfo);
void displayAll(float humidity, float temp, const struct tm* timeinfo);
void updateDisplay(DisplayMode mode, float humidity, float temp, time_t now);


#endif
