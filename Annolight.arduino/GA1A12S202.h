#ifndef GA1A12S202_H
#define GA1A12S202_H

#include <Arduino.h>

class GA1A12S202 {
  private:
    byte pin;
    float logLuxPerLSB;
    float luxValue;

    float rawToLux(int raw);

  public:
    GA1A12S202(const byte _pin, const float _logLightRangeMax, const float _analogReadMax);
    ~GA1A12S202();
    float lux();
    void update();
};

#endif

