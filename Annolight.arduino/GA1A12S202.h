#ifndef GA1A12S202_H
#define GA1A12S202_H

#include <Arduino.h>

class GA1A12S202 {
  private:
    const float rawLightRange = 1024.0; // 3.3v
    const float logLightRange = 5.0;    // 3.3v = 10^5 lux
    byte pin;
    float luxValue;

    float rawToLux(int raw);

  public:
    GA1A12S202(const byte _pin);
    ~GA1A12S202();
    float lux();
    void update();
};

#endif

