#include "LED.h"

LED::LED(const byte _pin, uint8_t _initialPerceivedPower, bool _isInitiallyOn) {
  if (Serial) Serial.printf("Initializing new LED on pin %d\n", _pin);

  pin = _pin;
  ledsOn = _isInitiallyOn;
  currentPerceivedPower = _initialPerceivedPower;
  pinMode(pin, OUTPUT);
  write();
}

LED::~LED() {
  // no-op
}

void LED::toggle() {
  ledsOn = !ledsOn;
  write();
}

bool LED::isOn() {
  return ledsOn;
}

void LED::on() {
  ledsOn = true;
  write();
}

void LED::off() {
  ledsOn = false;
  write();
}

uint8_t LED::getPerceivedPower() {
  return currentPerceivedPower;
}

void LED::setPerceivedPower(uint8_t power) {
  currentPerceivedPower = power;
  write();
}

void LED::setPower(uint8_t power) {
  currentPerceivedPower = pgm_read_byte(&gammaLUT[power]);
  write();
}

void LED::write() {
  if (Serial) Serial.printf("Setting power to %d; ledsOn = %d on pin %d\n", currentPerceivedPower, ledsOn, pin);
  analogWrite(pin, ledsOn * currentPerceivedPower);
}

