#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>

#define VBAT_MV_PER_LSB   (0.73242188F)   // 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#define VBAT_DIVIDER      (0.71275837F)   // 2M + 0.806M voltage divider on VBAT = (2M / (0.806M + 2M))
#define VBAT_DIVIDER_COMP (1.403F)        // Compensation factor for the VBAT divider

class Battery {

  float mVPerLSB;
  float batteryPercent;
  byte pin;
  
  public:
    Battery(const byte _pin, const uint _analogReadMax, const float _batteryMAH);
    ~Battery();
    float percent();
    void update();
};

#endif
