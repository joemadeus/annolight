#include <Arduino.h>
#include <bluefruit.h>

#include "Battery.h"
#include "GA1A12S202.h"
#include "LED.h"


/* ~~~~~~~~~~~~~~~~~~~
 * LED SETUP 
 * ~~~~~~~~~~~~~~~~~~~
 */
LED WhiteLEDs = LED(16, 128, true);
LED RedLEDs   = LED(15,   0, false);
LED GreenLEDs = LED(7,    0, false);
LED BlueLEDs  = LED(11,   0, false);


/* ~~~~~~~~~~~~~~~~~~~
 * SENSOR/BUTTON SETUP 
 * ~~~~~~~~~~~~~~~~~~~
 */
#define ANALOG_READ_MAX 4096 // 12-bit analog reads systemwide
#define ANALOG_READ_REF_MV 3000.0 // 3.0v internal analog reference
GA1A12S202 LeftLightSensor  = GA1A12S202(A1, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);
GA1A12S202 RightLightSensor = GA1A12S202(A2, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);

Battery Battery1 = Battery(A7, ANALOG_READ_REF_MV, ANALOG_READ_MAX);

int SoftOffPinIn = 17;

/*
 * Toggle the status of the white LEDs. The RGB indicator LEDs are left alone
 * since they indicate that something has to change.
 */
void softToggle() {
  Serial.println("toggling");
  WhiteLEDs.toggle();
}


/* ~~~~~~~~~~~~~~~~~~~
 * BLUETOOTH SETUP 
 * ~~~~~~~~~~~~~~~~~~~
 */
#define MANUFACTURER_ID 0x0059 // Nordic Semi
// BLE Services
BLEDis  bledis;
BLEUart bleuart;

// AirLocate UUID: ba9a1262-77d7-4a2a-9677-1eac9ac9d2fe
const uint8_t beaconUuid[16] = { 
  0xBa, 0x9A, 0x12, 0x62, 0x77, 0xD7, 0x4A, 0x2A, 
  0x96, 0x77, 0x1E, 0xAC, 0x9A, 0xC9, 0xD2, 0xFE, 
};

// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
BLEBeacon beacon(beaconUuid, 0x0001, 0x0000, -54);

/* ~~~~~~~~~~~~~~~~~~~
 * SETUP()
 * ~~~~~~~~~~~~~~~~~~~
 */
void setup() {
  Serial.begin(115200);
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);

  // Let the ADC settle
  delay(10);

  // ~~~~~~~~~ PIN SETUP ~~~~~~~~~ //
  // NB, the LEDs, light sensors and battery set pins for themselves
  Serial.println("attaching softoff pin interrupt");
  pinMode(SoftOffPinIn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SoftOffPinIn), softToggle, FALLING);

  // ~~~~~~~~~ BLUETOOTH SETUP ~~~~~~~~~ //
  Serial.println("bluetooth setup");
  BlueLEDs.on();
  Bluefruit.begin();
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(0); // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setName("Annolight");
  Bluefruit.setConnectCallback(bluetooth_connect_callback);

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();  

  // Configure and start BLE UART service
  bleuart.begin();

  // Set up and start advertising
  startAdv();

  // Setup the advertising packet
  // startAdv();
  BlueLEDs.off();

  WhiteLEDs.on();
}

void bluetooth_connect_callback(uint16_t conn_handle) {
  char central_name[32] = { 0 };
  Bluefruit.Gap.getPeerName(conn_handle, central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

void startAdv(void) {
  Serial.println("bluetooth advertising setup");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  
  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}


/* ~~~~~~~~~~~~~~~~~~~
 * MAIN LOOP 
 * ~~~~~~~~~~~~~~~~~~~
 */
void loop() {
  // On loop, handle commands from a connected 'central'; otherwise advertise the
  // current temp, humidity, light levels, and output settings
  if ( Bluefruit.connected() && bleuart.notifyEnabled() ) {
    int command = bleuart.read();

    switch (command) {
      case 'B': {   // Set Brightness
          commandSetBrightness();
          break;
      }

      case 'L': {  // Toggle light sensors
          commandToggleLightSensors();
          break;
      }

      case 'F': {  // Shut off the white LEDs
          commandOff();
          break;
      }

      case 'N': {  // Turn on the white LEDs
          commandOn();
          break;
      }
    }
  }
}

/* RTOS idle callback is automatically invoked by FreeRTOS when there
 * are no active threads, e.g when loop() calls delay() and there is no
 * bluetooth or hw event.
 */
void rtos_idle_callback(void) {
  // Don't call any other FreeRTOS blocking API()
  // Perform background task(s) here
}

void commandSetBrightness() {
  Serial.println(F("Command: Set brightness"));
  uint8_t pPower = bleuart.read();
  if (pPower == 0) {
    Serial.println(F("got perceived power of 0 from bluetooth, treating as commandOff()"));
    WhiteLEDs.off();
  } else if (pPower < 0 || pPower > LED_MAX_PERCEIVED_POWER) {
    Serial.print(F("bad perceived power setting from bluetooth: "));
    Serial.println(pPower);
  } else {
    Serial.print(F("got power setting from bluetooth: "));
    Serial.println(pPower);
    WhiteLEDs.on();
    WhiteLEDs.setPerceivedPower(pPower);
  }

  sendResponse("OK");
}

void commandOff() {
  Serial.println(F("Command: Off"));
  WhiteLEDs.off();
  sendResponse("OK");
}

void commandOn() {
  Serial.println(F("Command: On"));
  WhiteLEDs.on();
  sendResponse("OK");
}

void commandToggleLightSensors() {
  Serial.println(F("Command: Toggle light sensors"));
  // TODO
}

void sendResponse(char const *response) {
    Serial.printf("Send Response: %s\n", response);
    bleuart.write(response, strlen(response)*sizeof(char));
}

