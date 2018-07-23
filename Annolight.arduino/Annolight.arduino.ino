#include <Arduino.h>
#include <bluefruit.h>

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

/* ~~~~~~~~~~~~~~~~~~~
 * SOFTOFF/TOGGLE
 * ~~~~~~~~~~~~~~~~~~~
 */
unsigned long lastButt = millis();
unsigned long const debounceDelayMs = 100;
bool startAdvertisingToggle = false;
int SoftOffPinIn = A3;

/* 
 * Toggle the status of the white LEDs. The RGB indicator LEDs are left alone
 * since they indicate that something needs attention. If we're turning the
 * white LEDs on again, start BT advertising as well (but don't call 'start'
 * here, you apparently can't do BT stuff from an interrupt handler.)
 */
void onSoftoffButton() {
  unsigned long now = millis();
  if ((now - lastButt) > debounceDelayMs) {
    if (Serial) Serial.println("toggle LEDs");
    lastButt = now;
    WhiteLEDs->toggle();
    if (WhiteLEDs->isOn()) {
      // Start advertising if the light was just turned on. Easier to do than
      // trying to create a "dual purpose" button with long presses, &c
      startAdvertisingToggle = true;
      // If the light was just turned on but there's not enough power to really
      // show that it's on, set a reasonable brightness
      if (WhiteLEDs->getPerceivedPower() < 5) {
        WhiteLEDs->setPerceivedPower(64);
      }
    }
  }
}

/* ~~~~~~~~~~~~~~~~~~~
 * BLUETOOTH
 * ~~~~~~~~~~~~~~~~~~~
 */
#define BLUETOOTH_NAME "Annolight"
#define MANUFACTURER_ID 0x0059
// 27fdf34a-5d09-4f85-9a03-6fba63f1a41c
uint8_t beaconUuid[16] = {
  0x27, 0xfd, 0xf3, 0x4a, 0x5d, 0x09, 0x4f, 0x85,
  0x9a, 0x03, 0x6f, 0xba, 0x63, 0xf1, 0xa4, 0x1c
};

BLEDis    bledis;  // Discovery service
BLEUart   bleuart; // Peripheral uart service handles commands from the client
BLEBas    blebas;  // Peripheral battery service
BLEBeacon beacon(beaconUuid, 0x0001, 0x0000, -54);

void onBluetoothConnect(uint16_t conn_handle) {
  char central_name[32] = { 0 };
  Bluefruit.Gap.getPeerName(conn_handle, central_name, sizeof(central_name));
  BlueLEDs->setPerceivedPower(16);
  BlueLEDs->on();
  if (Serial) {
    Serial.printf("Connected to %s\n", central_name);
    Serial.printf("%s\n", central_name);
  }
}

void onBluetoothDisconnect(uint16_t conn_handle, uint8_t reason) {
  BlueLEDs->off();
  if (Serial) Serial.println("bluetooth disconnect");
}

void onAdvertiseStop(void) {
  if (Serial) Serial.println("bluetooth advertising stop");
}

void advertiseStart(void) {
  if (startAdvertisingToggle) {
    if (Serial) Serial.println("unset startAdvertisingToggle");
    startAdvertisingToggle = false;
  }

  if (Bluefruit.Advertising.isRunning()) {
    if (Serial) Serial.println("already advertising");
    return;
  }

  if (Serial) Serial.println("bluetooth advertising start");
  Bluefruit.Advertising.start(0); // 0 = Don't stop advertising
}

void onBleUartReceive() {
  if (bleuart.notifyEnabled()) {
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

      case 'P': {  // PARTY MODE!!
          commandPartyMode();
          break;
      }
    }
  }
}

/* ~~~~~~~~~~~~~~~~~~~
 * SETUP()
 * ~~~~~~~~~~~~~~~~~~~
 */
void setup() {
  Serial.begin(115200);
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);

  PSELP=0
  

  // Let the ADC settle
  delay(100);

  WhiteLEDs = new LED(16, 64, false);
  RedLEDs   = new LED(15, 64, false);
  GreenLEDs = new LED(7,  64, false);
  BlueLEDs  = new LED(11, 64, false);

  Battery1 = new Battery(A7, ANALOG_READ_REF_MV, ANALOG_READ_MAX);
  LeftLightSensor  = new GA1A12S202(A1, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);
  RightLightSensor = new GA1A12S202(A2, GA1A12S202_LOG_LUX_MAX, ANALOG_READ_MAX);

  // ~~~~~~~~~ BLUETOOTH SETUP ~~~~~~~~~ //
  Serial.println("bluetooth setup");
  BlueLEDs->setPerceivedPower(64);
  BlueLEDs->on();
  Bluefruit.begin();
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(0); // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setName(BLUETOOTH_NAME);
  Bluefruit.ScanResponse.addName();

  // Callbacks for Peripheral
  Bluefruit.setConnectCallback(onBluetoothConnect);
  Bluefruit.setDisconnectCallback(onBluetoothDisconnect);

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Bluetooth UART service config
  bleuart.begin();
  bleuart.setRxCallback(onBleUartReceive);

  // Start BLE Battery Service
  blebas.begin();
  Battery1->update();
  blebas.write(floor(Battery1->percent() + 0.5));

  // Bluetooth advertising config
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.setStopCallback(onAdvertiseStop);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.addService(bleuart);
  beacon.setManufacturer(MANUFACTURER_ID);
  Bluefruit.Advertising.setBeacon(beacon);

  // Start advertising on startup
  advertiseStart();

  Serial.println("bluetooth setup done");
  BlueLEDs->off();

  // For some reason this must appear after bluetooth setup, otherwise BT never starts
  Serial.println("attaching softoff pin interrupt");
  pinMode(SoftOffPinIn, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SoftOffPinIn), onSoftoffButton, FALLING);

  WhiteLEDs->on();
}

void commandSetBrightness() {
  if (Serial) Serial.println("Command: Set brightness");
  uint8_t pPower = bleuart.read();
  if (pPower == 0) {
    if (Serial) Serial.println("got perceived power of 0 from bluetooth, treating like commandOff()");
    WhiteLEDs->off();
  } else if (pPower < 0 || pPower > LED_MAX_PERCEIVED_POWER) {
    if (Serial) Serial.printf("bad perceived power setting from bluetooth: %d\n", pPower);
  } else {
    if (Serial) Serial.printf("got perceived power setting from bluetooth: %d\n", pPower);
    WhiteLEDs->setPerceivedPower(pPower);
    WhiteLEDs->on();
  }

  sendResponse("OK");
}

void commandOff() {
  if (Serial) Serial.println("Command: Off");
  WhiteLEDs->off();
  sendResponse("OK");
}

void commandOn() {
  if (Serial) Serial.println("Command: On");
  WhiteLEDs->on();
  sendResponse("OK");
}

void commandPartyMode() {
  if (Serial) Serial.println("Command: PARTY PARTY");
  // TODO
  sendResponse("OK");
}

void commandToggleLightSensors() {
  if (Serial) Serial.println("Command: Toggle light sensor compensation");
  // TODO
  sendResponse("OK");
}

void sendResponse(char const *response) {
  if (Serial) Serial.printf("Send Response: %s\n", response);
//  bleuart.write(response, strlen(response)*sizeof(char));
}

/* ~~~~~~~~~~~~~~~~~~~
 * MAIN LOOP 
 * ~~~~~~~~~~~~~~~~~~~
 */
long previousMillis = 0;
const long interval = 5000;
void loop() {
  // wait for the signal to start advertising again
  if (startAdvertisingToggle) advertiseStart();

  unsigned long currentMillis = millis();
  if (previousMillis == 0 || currentMillis - previousMillis > interval) {
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

    blebas.write(Battery1->percent());
  }

  if (Serial.available()) {
    int fromSerial = Serial.parseInt();
    if (fromSerial == -1) {
      startAdvertisingToggle = true;
    } else {
      WhiteLEDs->setPerceivedPower(fromSerial);
    }
  } else { }
}

