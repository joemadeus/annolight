#include "LED.h"

LED::LED(const byte _pin) {
  pin = _pin;
  pinMode(pin, OUTPUT);
  on = false;
  currentPerceivedPower = 128;
}

LED::~LED() {
  // no-op
}

void LED::toggle() {
  on = !on;
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
  digitalWrite(pin, on * currentPerceivedPower);
}

