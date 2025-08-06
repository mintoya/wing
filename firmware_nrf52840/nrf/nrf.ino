#include "Adafruit_TinyUSB.h"
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include "arduino/hid/Adafruit_USBD_HID.h"
#include "my-list.h"
#include "my-list.hpp"
#include <bluefruit.h>
#include "key.hpp"


constexpr uint8_t gpios[] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

BLEHidAdafruit blehid;
Adafruit_USBD_HID device(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);



#define FILENAME "/adafruit.txt";
#define CONTENTS "Adafruit Little File System test file contents";

using namespace Adafruit_LittleFS_Namespace;
File file(InternalFS);

//   9 12
// 5 a  b
// 3 c  d

uint8_t colPins[2] = { 9, 12 };
uint8_t rowPins[2] = { 5, 3 };
// clang-format off
  KeyItem matrix[4] = {
    KEY_A, KEY_B, 
    KEY_C, KEY_D
  };
  reportManager rm;
  keyMap mk;
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
  device.begin();

  while (!TinyUSBDevice.mounted()) {
    delay(10);
  }

  // GPIO
  for (size_t i = 0; i < sizeof(gpios) / sizeof(uint8_t); i++) {
    pinMode(gpios[i], INPUT_PULLDOWN);
  }

  SerialTinyUSB.begin(115200);
  startAdv();


  rm = reportManager(blehid,device);
  mk = keyMap(rowPins,2,colPins,2,matrix);
}
void startAdv(void) {
  Bluefruit.setName("NRF");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  // Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);    // number of seconds in fast mode
  Bluefruit.Advertising.start(0);              // 0 = Don't stop advertising after n seconds
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
  for (size_t i = 0; i < sizeof(gpios) / sizeof(uint8_t); i++) {
    pinMode(gpios[i], OUTPUT);
    for (size_t j = 0; j < sizeof(gpios) / sizeof(uint8_t); j++) {
      if (j == i) continue;
      if (digitalRead(gpios[j])) {
        coord c = { .from = gpios[i], .to = gpios[j] };
        List_append(_activeKeysBuffer, &c);
      }
    }
    pinMode(gpios[i], INPUT_PULLDOWN);
    delay(1);
  }
  return listPlus<coord>(_activeKeysBuffer);
}

void loop() {

  mk.updateState();
  mk.pressKeys(rm);
  SerialTinyUSB.println(rm.keys.get(0));
  rm.send();
  delay(1);

  // clang-format on
  // listPlus<coord> activeKeys = getActiveKeys();
  //
  // if (!activeKeys.length()) {
  //   SerialTinyUSB.println(" no pins connected ");
  //   uint8_t report[6] = { 0, 0, 0, 0, 0, 0 };
  //   blehid.keyboardReport(0, report);     //reportid,modifiers,codes
  //   device.keyboardReport(0, 0, report);  //modifiers,codes
  // } else {
  //   for (size_t i = 0; i < activeKeys.length(); i++) {
  //     // if(blehid.connected()){
  //
  //     uint8_t report[6] = { 0x4, 0, 0, 0, 0, 0 };
  //     blehid.keyboardReport(0, 0, report);  //reportid,modifiers,codes
  //     device.keyboardReport(0, 0, report);  //modifiers,codes
  //     // }
  //     coord current = activeKeys.get(i);
  //     SerialTinyUSB.printf("%i -> %i\n", current.from, current.to);
  //   }
  //   delay(10);
  // }
}
