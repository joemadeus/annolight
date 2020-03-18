#include "GA1A12S202.h"

GA1A12S202::GA1A12S202(const byte _pin, const float _logLightRangeMax, const float _analogReadMax) {
  if (Serial) Serial.printf("Initializing new GA1A12S202 on pin %d\n", _pin);

  pin = _pin;
  pinMode(pin, INPUT);
  logLuxPerLSB = _logLightRangeMax / _analogReadMax;

  update();
}

GA1A12S202::~GA1A12S202() {
  // no-op
}

int GA1A12S202::raw() {
  return rawValue;
}

float GA1A12S202::lux() {
  return rawToLux(rawValue);
}

void GA1A12S202::update() {
  rawValue = analogRead(pin);
}

float GA1A12S202::rawToLux(int raw) {
  return pow(10, raw * logLuxPerLSB);
}

