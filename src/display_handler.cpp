#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>  // For larger text and graphics

#include "display_handler.h"
#include "error_handler.h"

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 'Thermometer', 16x16px
const unsigned char Thermometer [] PROGMEM = {
  0xfc, 0x7f, 0xfb, 0x8f, 0xfb, 0xbf, 0xfb, 0x8f,
  0xfb, 0xbf, 0xfb, 0x8f, 0xf8, 0x3f, 0xf8, 0x0f, 
  0xf8, 0x3f, 0xf8, 0x3f, 0xf0, 0x1f, 0xf0, 0x1f, 
  0xe0, 0x1f, 0xf0, 0x1f, 0xf0, 0x1f, 0xf8, 0x3f
};

// 'Tropfen' (droplet), 16x16px
const unsigned char Tropfen [] PROGMEM = {
  0xff, 0xff, 0xfe, 0x7f, 0xfc, 0x7f, 0xf8, 0x7f,
  0xf8, 0x3f, 0xf0, 0x1f, 0xf0, 0x0f, 0xe0, 0x0f,
  0xe0, 0x07, 0xe0, 0x07, 0xec, 0x07, 0xef, 0x07,
  0xe7, 0x87, 0xe7, 0x87, 0xf0, 0x0f, 0xfc, 0x3f
};

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    signalError(DISPLAY_ERROR);
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
}

void displayOff() {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void displayOn() {
    display.ssd1306_command(SSD1306_DISPLAYON);
}

void displayValues(float humidity, float temp) {
  displayOn();
  display.clearDisplay();

  // Temperature
  display.drawBitmap(2, 4, Thermometer, 16, 16, SSD1306_BLACK, SSD1306_WHITE);
  display.setCursor(20, 0);
  display.setTextSize(3);
  display.print(temp, 1);
  display.print(" C");

  // Humidity
  display.drawBitmap(2, 36, Tropfen, 16, 16, SSD1306_BLACK, SSD1306_WHITE);
  display.setCursor(20, 32);
  display.setTextSize(3);
  display.print(humidity, 1);
  display.print(" %");

  display.display();
}

void displayMessage(const char* message) {
  displayOn();
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}

void displayTime(const struct tm* timeinfo) {
  displayOn();
  char timeStrTop[16];
  char timeStrBottom[16];


  // Format HH:MM
  strftime(timeStrTop, sizeof(timeStrTop), "%H:%M", timeinfo);
  // Format SS
  strftime(timeStrBottom, sizeof(timeStrBottom), "%S", timeinfo);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(4);  // Top: large
  display.setCursor(6, 6);
  display.println(timeStrTop);

  display.setTextSize(2);  // Bottom: seconds
  display.setCursor(54, 46);
  display.println(timeStrBottom);

  display.display();
}

void displayAll(float humidity, float temp, const struct tm* timeinfo) {
  displayOn();
  display.clearDisplay();

  // Time
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(timeStr);

  // Temperature
  display.drawBitmap(2, 22, Thermometer, 16, 16, SSD1306_BLACK, SSD1306_WHITE);
  display.setCursor(20, 22);
  display.setTextSize(2);
  display.print(temp, 1);
  display.print(" C");

  // Humidity
  display.drawBitmap(2, 46, Tropfen, 16, 16, SSD1306_BLACK, SSD1306_WHITE);
  display.setCursor(20, 46);
  display.setTextSize(2);
  display.print(humidity, 1);
  display.print(" %");

  display.display();
}



void updateDisplay(DisplayMode mode, float humidity, float temp, time_t now) {
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  switch (mode) {
    case MODE_OFF:
      displayOff();
      break;

    case MODE_TIME:
      displayTime(&timeinfo);
      break;

    case MODE_TEMPHUM:
      displayValues(humidity, temp);
      break;

    case MODE_ALL:
      displayAll(humidity, temp, &timeinfo);
      break;

    default:
      displayMessage("Invalid mode");
      break;
  }
}

