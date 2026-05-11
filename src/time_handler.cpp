#include <Arduino.h>
#include <time.h>

#include "display_handler.h"

void syncTimeVerbose() {

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  displayMessage("Syncing time...");

  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();  

  struct tm timeinfo;
  int retries = 0;
  while (!getLocalTime(&timeinfo) && retries < 20) {
    delay(500);
    retries++;
  }

  if (retries >= 20) {
    displayMessage("NTP failed");
    delay(2000);
  } else {
    displayMessage("Time synced");
    delay(1000);
  }
}

void syncTime() {

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  displayMessage("Syncing time...");

  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();  

  struct tm timeinfo;
  int retries = 0;
  while (!getLocalTime(&timeinfo) && retries < 20) {
    delay(500);
    retries++;
  }
}

void attemptResyncTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  // Do not wait! Just initiate resync
}


void getUnixTimestamp(time_t* outTime) {
    if (outTime != nullptr) {
        time(outTime);  // sets value at outTime
    }
}

void displayCurrentTime() {
  struct tm timeinfo;
  time_t now;
  if (getLocalTime(&timeinfo)) {
    time(&now);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

    String message = String(now) + "\n" + timeStr;
    displayMessage(message);
    
    
    //displayTime(&timeinfo);
  } else {
    displayMessage("Time N/A");
  }
}