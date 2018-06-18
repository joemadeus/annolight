#include "LED.h"

LED::LED(const byte _pin, uint8_t _initialPerceivedPower, bool _isInitiallyOn) {
  if (Serial) {
    Serial.print("Initializing new LED on pin ");
    Serial.println(_pin);
  }

  pin = _pin;
  isOn = _isInitiallyOn;
  currentPerceivedPower = _initialPerceivedPower;
  pinMode(pin, OUTPUT);
  write();
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
  currentPerceivedPower = power;
  write();
}

void LED::setPower(uint8_t power) {
  currentPerceivedPower = pgm_read_byte(&gammaLUT[power]);
  write();
}

void LED::write() {
  if (Serial) {
    Serial.print("Setting power to ");
    Serial.print(currentPerceivedPower);
    Serial.print("; isOn = ");
    Serial.print(isOn);
    Serial.print(" on pin ");
    Serial.println(pin);
  }
  analogWrite(pin, isOn * currentPerceivedPower);
}

