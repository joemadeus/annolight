#ifndef GA1A12S202_H
#define GA1A12S202_H

#include <Arduino.h>

#define GA1A12S202_LOG_LUX_MAX 4.7

class GA1A12S202 {
  private:
    byte pin;
    float logLuxPerLSB;
    int rawValue; // 0..4095

    float rawToLux(int raw);

  public:
    GA1A12S202(const byte _pin, const float _logLightRangeMax, const float _analogReadMax);
    ~GA1A12S202();
    int raw();
    float lux();
    void update();
};

#endif

