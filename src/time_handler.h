#ifndef TIME_HANDELR_H
#define TIME_HANDELR_H

#include <Arduino.h>
#include <time.h>

#include "display_handler.h"

void syncTimeVerbose();
void syncTime();
void attemptResyncTime();
void displayCurrentTime();
void getUnixTimestamp(time_t* outTime);


#endif
