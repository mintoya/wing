#include "Adafruit_TinyUSB.h"
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include "arduino/hid/Adafruit_USBD_HID.h"
#include "my-list.h"
#include "my-list.hpp"
#include <bluefruit.h>


constexpr uint8_t gpios[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

BLEHidAdafruit blehid;
Adafruit_USBD_HID device(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);



#define FILENAME "/adafruit.txt";
#define CONTENTS "Adafruit Little File System test file contents";

using namespace Adafruit_LittleFS_Namespace;
File file(InternalFS);

void setup() {
  InternalFS.begin();

  // USB
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin();
  }
  // BLEHID
  Bluefruit.begin(); 
  blehid.begin();



  // HID
  // device.begin();

  while (!TinyUSBDevice.mounted()) {
    delay(10);
  }

  // GPIO
  for (size_t i = 0; i < sizeof(gpios) / sizeof(uint8_t); i++) {
    pinMode(gpios[i], INPUT_PULLDOWN);
  }

  SerialTinyUSB.begin(115200);
  startAdv();
}
void startAdv(void) {  
  Bluefruit.setName("NRF");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  // Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

struct coord {
  uint8_t from;
  uint8_t to;
  //connection between wires from high to low
};

listPlus<coord> getActiveKeys() {
  static List* _activeKeysBuffer = NULL;
  if (!_activeKeysBuffer) {
    _activeKeysBuffer = List_new(sizeof(coord));
  }
  _activeKeysBuffer->length = 0;
  for (size_t i = 0; i < sizeof(gpios)/sizeof(uint8_t); i++) {
    pinMode(gpios[i], OUTPUT);
    for (size_t j = 0; j < sizeof(gpios)/sizeof(uint8_t); j++) {
      if (j == i) continue;
      if (digitalRead(gpios[j])) {
        coord c = { .from = gpios[i], .to = gpios[j] } ;
        List_append(_activeKeysBuffer, &c);
                                         

        // keycode is a uint_8[6]
        // blehid.keyboardReport(0,0,keycode); //reportid,modifiers,codes
        // device.keyboardReport(  0,keycode); //modifiers,codes

        // SerialTinyUSB.print(i);
        // SerialTinyUSB.print(" -> ");
        // SerialTinyUSB.println(j);
      }
    }
    pinMode(gpios[i], INPUT_PULLDOWN);
    delay(1);
  }
  return listPlus<coord>(_activeKeysBuffer);
}

void loop() {
  listPlus<coord> activeKeys = getActiveKeys();

  if (!activeKeys.length()) {
    SerialTinyUSB.println(" no pins connected ");
  } else { 
    for(size_t i = 0; i<activeKeys.length();i++){
      // if(blehid.connected()){
        blehid.keyPress('a');
        delay(1000);
        blehid.keyRelease('a');
      // }
      coord current = activeKeys.get(i);
      SerialTinyUSB.printf("%i -> %i\n",current.from,current.to);
    }
    delay(10);
  }
}
