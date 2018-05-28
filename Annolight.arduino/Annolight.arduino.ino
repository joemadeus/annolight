#include <bluefruit.h>

#include "Battery.h"
#include "GA1A12S202.h"
#include "LED.h"

#define ANALOG_READ_MAX 4096 // 12-bit analog reads systemwide
#define ANALOG_READ_REF_MV 3000.0 // 3.0v internal analog reference

// Apple
#define MANUFACTURER_ID 0x0040

// AirLocate UUID: ba9a1262-77d7-4a2a-9677-1eac9ac9d2fe
const uint8_t beaconUuid[16] = { 
  0xBa, 0x9A, 0x12, 0x62, 0x77, 0xD7, 0x4A, 0x2A, 
  0x96, 0x77, 0x1E, 0xAC, 0x9A, 0xC9, 0xD2, 0xFE, 
};

// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
BLEBeacon beacon(beaconUuid, 0x0001, 0x0000, -54);


LED WhiteLEDs = LED(30, 128, true);
LED RedLEDs   = LED(29,   0, false);
LED GreenLEDs = LED(20,   0, false);
LED BlueLEDs  = LED(25,   0, false);

#define GA1A12S202_LOG_LUX_MAX 4.7
GA1A12S202 LeftLightSensor  = GA1A12S202(A0, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);
GA1A12S202 RightLightSensor = GA1A12S202(A1, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);

Battery Battery1 = Battery(A7, ANALOG_READ_REF_MV, ANALOG_READ_MAX);

int SoftOffPinIn = 17;

/*
 * Toggle the status of thenwhite LEDs. The RGB indicator LEDs are left alone
 * since they indicate that something has to change.
 */
void softToggle() {
  Serial.println("toggling");
  WhiteLEDs.toggle();
}

void setup() {
  Serial.begin(9600);
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);

  // Let the ADC settle
  delay(1);

  // ~~~~~~~~~ PIN SETUP ~~~~~~~~~ //
  Serial.println("attach softoff pin interrupt");
  pinMode(SoftOffPinIn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SoftOffPinIn), softToggle, FALLING);

  // ~~~~~~~~~ BLUETOOTH SETUP ~~~~~~~~~ //
  Serial.println("bluetooth setup");
  BlueLEDs.on();
  Bluefruit.begin();
  Bluefruit.autoConnLed(false);
  
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(0);
  Bluefruit.setName("Annolight");
  beacon.setManufacturer(MANUFACTURER_ID);

  // Setup the advertising packet
  // startAdv();
  BlueLEDs.off();

  WhiteLEDs.on();
}

void startAdv(void) {
  Serial.println("bluetooth advertising setup");
  // Advertising packet
  // Set the beacon payload using the BLEBeacon class populated earlier
  Bluefruit.Advertising.setBeacon(beacon);

  // Secondary Scan Response packet (optional)
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * Apple Beacon specs
   * - Type: Non connectable, undirected
   * - Fixed interval: 100 ms -> fast = slow = 100 ms
   */
  //Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 160);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop() {
  // On loop, advertise the current temp, humidity, light levels, and
  // output settings
}

