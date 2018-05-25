#include "LED.h"

LED::LED(const byte _pin, uint8_t _initialPerceivedPower, bool _isInitiallyOn) {
  Serial.print("Initializing LEDs on pin ");
  Serial.println(_pin);

  pin = _pin;
  pinMode(pin, OUTPUT);
  isOn = _isInitiallyOn;
  currentPerceivedPower = _initialPerceivedPower;
}

LED::~LED() {
  // no-op
}

void LED::toggle() {
  isOn = !isOn;
  write();
}

void LED::on() {
  isOn = true;
  write();
}

void LED::off() {
  isOn = false;
  write();
}

void LED::setPerceivedPower(uint8_t power) {
  currentPerceivedPower = pgm_read_byte(&gammaLUT[power]);
  write();
}

void LED::setPower(uint8_t power) {
  currentPerceivedPower = power;
  write();
}

void LED::write() {
  digitalWrite(pin, isOn * currentPerceivedPower);
}

