#pragma once
#include "hid_keys.h"
#include <string.h>

#include "arduino/hid/Adafruit_USBD_HID.h"
#include "my-list.h"
#include "my-list.hpp"
#include <bluefruit.h>
#include <stdint.h>
#include <stdlib.h>

// clang-format off


// stores type of data and data
struct KeyItem {
  uint8_t character;//not really a character 
  enum : char { 
           PASSTHROUGH = 0,
           CHARACTER = 1, 
           LAYER = 2,
           TAPDANCE = 4 ,
           MODIFIER = 8
         } type;
  inline KeyItem(uint8_t c){character = c;type = CHARACTER;}
  inline KeyItem(void){character = 0,type = PASSTHROUGH;}
};
// clang-format on

// KeyItem matrix[][]  = {{0x41}};
// listPlus<coord> getActiveKeys() {
//   static List* _activeKeysBuffer = NULL;
//   if (!_activeKeysBuffer) {
//     _activeKeysBuffer = List_new(sizeof(coord));
//   }
//   _activeKeysBuffer->length = 0;
//   for (size_t i = 0; i < sizeof(gpios) / sizeof(uint8_t); i++) {
//     pinMode(gpios[i], OUTPUT);
//     for (size_t j = 0; j < sizeof(gpios) / sizeof(uint8_t); j++) {
//       if (j == i) continue;
//       if (digitalRead(gpios[j])) {
//         coord c = { .from = gpios[i], .to = gpios[j] };
//         List_append(_activeKeysBuffer, &c);
//       }
//     }
//     pinMode(gpios[i], INPUT_PULLDOWN);
//     delay(1);
//   }
//   return listPlus<coord>(_activeKeysBuffer);
// }

//   9 12
// 5
// 3
struct reportManager {
  reportManager() {};
  BLEHidAdafruit blehid;
  Adafruit_USBD_HID usbhid;
  reportManager(BLEHidAdafruit ble, Adafruit_USBD_HID usb) {
    blehid = ble;
    usbhid = usb;
  }
  listPlus<bool> modifiers;
  listPlus<uint8_t> keys;

  void addKey(KeyItem k) {
    switch (k.type) {
    case KeyItem::MODIFIER:
      modifiers.append(k.character);
      break;
    case KeyItem::CHARACTER:
      keys.append(k.character);
      break;
    default:
      break;
    }
  }
  void send() {
    uint8_t modifierResult = 0;
    for (size_t i = 0; i < modifiers.length(); i++) {
      modifierResult |= modifiers.get(i);
    }

    while (keys.length() < 6) {
      keys.append(0);
    }
    blehid.keyboardReport(0, modifierResult, keys.autoCast());
    usbhid.keyboardReport(0, modifierResult, keys.autoCast());
    modifiers.ptr->length = 0;
    keys.ptr->length = 0;
  }
};

namespace keyMap {

  void pressKeys(uint8_t length,KeyItem* maps, bool*state,reportManager rm){
    for (unsigned int i = 0;i<length;i++){
      if(state[i]){
        rm.addKey(maps[i]);
      }

    }
  }
}
