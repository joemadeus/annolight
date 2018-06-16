#ifndef LED_H
#define LED_H

#include <Arduino.h>

#define LED_MAX_PERCEIVED_POWER 255

class LED {

  private:
    byte pin;
    bool isOn;
    uint8_t currentPerceivedPower;

    // (i/255) ** 2.2 * 245 + 12
    const uint8_t PROGMEM gammaLUT[255] = {
       12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
       12,  13,  13,  13,  13,  13,  13,  13,  13,  13,  13,  14,  14,  14,  14,
       14,  14,  15,  15,  15,  15,  15,  16,  16,  16,  16,  16,  17,  17,  17,
       17,  18,  18,  18,  19,  19,  19,  19,  20,  20,  20,  21,  21,  21,  22,
       22,  23,  23,  23,  24,  24,  25,  25,  25,  26,  26,  27,  27,  28,  28,
       29,  29,  30,  30,  31,  31,  32,  32,  33,  33,  34,  34,  35,  36,  36,
       37,  37,  38,  39,  39,  40,  41,  41,  42,  43,  43,  44,  45,  45,  46,
       47,  48,  48,  49,  50,  51,  51,  52,  53,  54,  54,  55,  56,  57,  58,
       59,  60,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  71,
       72,  73,  74,  75,  76,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
       88,  89,  90,  92,  93,  94,  95,  96,  97,  99, 100, 101, 102, 104, 105,
      106, 107, 109, 110, 111, 112, 114, 115, 116, 118, 119, 120, 122, 123, 124,
      126, 127, 129, 130, 132, 133, 134, 136, 137, 139, 140, 142, 143, 145, 146,
      148, 149, 151, 152, 154, 156, 157, 159, 160, 162, 164, 165, 167, 169, 170,
      172, 174, 175, 177, 179, 180, 182, 184, 186, 187, 189, 191, 193, 194, 196,
      198, 200, 202, 204, 205, 207, 209, 211, 213, 215, 217, 219, 221, 222, 224,
      226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 247, 249, 251, 253, 255 };
  
    void write();
  
  public:
    LED(const byte _pin, uint8_t _initialPerceivedPower, bool _isInitiallyOn);
    ~LED();
    void toggle();
    void on();
    void off();
    void setPerceivedPower(uint8_t power);
    void setPower(uint8_t power);
};

#endif

