#include <Arduino.h>

#include "Battery.h"
#include "GA1A12S202.h"
#include "LED.h"

/* ~~~~~~~~~~~~~~~~~~~
 * LEDS 
 * ~~~~~~~~~~~~~~~~~~~
 */
LED *WhiteLEDs;
LED *RedLEDs;
LED *GreenLEDs;
LED *BlueLEDs;

/* ~~~~~~~~~~~~~~~~~~~
 * SENSOR/BATTERY
 * ~~~~~~~~~~~~~~~~~~~
 */
#define ANALOG_READ_MAX 4096 // 12-bit analog reads systemwide
#define ANALOG_READ_REF_MV 3000.0 // 3.0v internal analog reference
GA1A12S202 *LeftLightSensor;
GA1A12S202 *RightLightSensor;

Battery *Battery1;

// ~~~~~~~~~ SOFTOFF ~~~~~~~~~ //
int SoftOffPinIn = A3;

/* 
 * Toggle the status of the white LEDs. The RGB indicator LEDs are left alone
 * since they indicate that something needs attention.
 */
void softToggle() {
  WhiteLEDs->toggle();
}

void setup() {
  Serial.begin(115200);
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);

  // Let the ADC settle
  delay(100);

  WhiteLEDs = new LED(16, 128, false);
  RedLEDs   = new LED(15, 128, false);
  GreenLEDs = new LED(7,  128, false);
  BlueLEDs  = new LED(11, 128, false);

  Battery1 = new Battery(A7, ANALOG_READ_REF_MV, ANALOG_READ_MAX);
  LeftLightSensor  = new GA1A12S202(A1, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);
  RightLightSensor = new GA1A12S202(A2, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);

  Serial.println("Attaching softoff pin interrupt");
  pinMode(SoftOffPinIn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SoftOffPinIn), softToggle, FALLING);

  WhiteLEDs->on();

  Serial.println("Ready!");
}

long previousMillis = 0;
const long interval = 2000;
void loop() {
  unsigned long currentMillis = millis();
  if (Serial && currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   

    Battery1->update();
    LeftLightSensor->update();
    RightLightSensor->update();

    Serial.print("Battery mV: ");
    Serial.print(Battery1->mV());
    Serial.print("; Battery Percent: ");
    Serial.print(Battery1->percent());

    Serial.print("; Left: ");
    Serial.print(LeftLightSensor->lux());
    Serial.print("; Right: ");
    Serial.println(RightLightSensor->lux());
  }

  float maxLux = max(LeftLightSensor->raw(), RightLightSensor->raw());

  if (Serial.available()) {
    WhiteLEDs->setPerceivedPower(Serial.parseInt());
  } else { }
}

