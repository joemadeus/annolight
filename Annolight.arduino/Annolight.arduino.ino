#include <bluefruit.h>
#include "LED.h"

// Apple
#define MANUFACTURER_ID 0x0040

// AirLocate UUID: ba9a1262-77d7-4a2a-9677-1eac9ac9d2fe
uint8_t beaconUuid[16] = { 
  0xBa, 0x9A, 0x12, 0x62, 0x77, 0xD7, 0x4A, 0x2A, 
  0x96, 0x77, 0x1E, 0xAC, 0x9A, 0xC9, 0xD2, 0xFE, 
};
 
// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
BLEBeacon beacon(beaconUuid, 0x0001, 0x0000, -54);


LED WhiteLEDs = LED(30);
LED RedLEDs   = LED(29);
LED GreenLEDs = LED(20);
LED BlueLEDs  = LED(25);

float rawLightRange    = 1024; // 3.3v
float logLightRange    = 5.0; // 3.3v = 10^5 lux
int LeftLightPinIn     = 15;
int RightLightPinIn    = 16;
int LeftLightLinearIn  = 0;
int RightLightLinearIn = 0;

int SoftOffPinIn = 17;

int BatteryPinIn   = 31;
int BatteryReading = 0;

/*
 * Toggle the status of the LEDs
 */
void softToggle() {
  
}

void setup() {  
  // ~~~~~~~~~ PIN SETUP ~~~~~~~~~ //
  pinMode(LeftLightPinIn,  INPUT);
  pinMode(RightLightPinIn, INPUT);
  pinMode(BatteryPinIn,    INPUT);

  // ~~~~~~~~~ BLUETOOTH SETUP ~~~~~~~~~ //
  Bluefruit.begin();
  Bluefruit.autoConnLed(false);
  
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(0);
  Bluefruit.setName("Annolight");
  beacon.setManufacturer(MANUFACTURER_ID);

  // Setup the advertising packet
//  startAdv();
}

void startAdv(void) {
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

float RawToLux(int raw) {
  return pow(10, raw * logLightRange / rawLightRange);
}
