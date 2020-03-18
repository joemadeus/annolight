#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
  unsigned long lastDebounceTime = 0;

  public:
    Button(const byte _pin, unsigned long debounceDelay);
    ~Button();
}

#endif

