#include "GA1A12S202.h"

GA1A12S202::GA1A12S202(const byte _pin) {
  pin = _pin;
  pinMode(pin, INPUT);
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
  return pow(10, raw * logLightRange / rawLightRange);
}

