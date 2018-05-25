#include "GA1A12S202.h"

GA1A12S202::GA1A12S202(const byte _pin, const float _analogReadMax, const float _logLightRangeMax) {
  Serial.print("Initializing light sensor on pin ");
  Serial.println(_pin);

  pin = _pin;
  pinMode(pin, INPUT);
  logLuxPerLSB = _logLightRangeMax / _analogReadMax;

  update();
}

GA1A12S202::~GA1A12S202() {
  // no-op
}

float GA1A12S202::lux() {
  return luxValue;
}

void GA1A12S202::update() {
  luxValue = rawToLux(analogRead(pin));
}

float GA1A12S202::rawToLux(int raw) {
  return pow(10, raw * logLuxPerLSB);
}

