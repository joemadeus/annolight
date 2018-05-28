#include "Battery.h"

Battery::Battery(const byte _pin, const float _analogReadRefMv, const uint _analogReadMax) {
  Serial.print("Initializing battery on pin ");
  Serial.println(_pin);

  pin = _pin;
  pinMode(pin, INPUT);
  mVPerLSB = _analogReadRefMv / _analogReadMax;

  analogRead(pin);
  delay(10);
  update();
}

Battery::~Battery() {}

float Battery::percent() {
  return batteryPercent;
}

void Battery::update() {
  batteryPercent = mvToPercent(analogRead(pin));
}

uint8_t Battery::mvToPercent(float mvolts) {
    if (mvolts >= 3000) return 100;
    else if (mvolts > 2900) return 100 - ((3000 - mvolts) * 58) / 100;
    else if (mvolts > 2740) return  42 - ((2900 - mvolts) * 24) / 160;
    else if (mvolts > 2440) return  18 - ((2740 - mvolts) * 12) / 300;
    else if (mvolts > 2100) return   6 - ((2440 - mvolts) *  6) / 340;
    else return 0;
}

