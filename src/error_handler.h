#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

enum ErrorType { 
  NO_ERROR = 0, 
  WIFI_ERROR = 1, 
  SENSOR_ERROR = 2, 
  POST_ERROR = 3, 
  DISPLAY_ERROR = 4 
};


void signalError(ErrorType error);
void handleBlinkPattern();
void setupBlinkLed();

#endif
