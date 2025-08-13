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
  enum kType : char{ 
           PASSTHROUGH = 0,
           CHARACTER = 1, 
           LAYER = 2,
           TAPDANCE = 4 ,
           MODIFIER = 8
         } type;
  inline KeyItem(uint8_t c){character = c;type = CHARACTER;}
  inline KeyItem(void){character = 0,type = PASSTHROUGH;}
  inline KeyItem(kType t,uint8_t data){type = t;character = data;}
};
namespace KeyMapDec{
  KeyItem M(char c){
// #define KEY_MOD_LCTRL  0x01  c
// #define KEY_MOD_LSHIFT 0x02  s
// #define KEY_MOD_LALT   0x04  a
// #define KEY_MOD_LMETA  0x08  m
// #define KEY_MOD_RCTRL  0x10  C
// #define KEY_MOD_RSHIFT 0x20  S
// #define KEY_MOD_RALT   0x40  A
// #define KEY_MOD_RMETA  0x80  M
//
// left : uncapitalized
// right: capitalized

    uint8_t actual;
    switch (c){
        case 'c': actual = KEY_MOD_LCTRL; break;
        case 's': actual = KEY_MOD_LSHIFT; break;
        case 'a': actual = KEY_MOD_LALT; break;
        case 'm': actual = KEY_MOD_LMETA; break;
        case 'C': actual = KEY_MOD_RCTRL; break;
        case 'S': actual = KEY_MOD_RSHIFT; break;
        case 'A': actual = KEY_MOD_RALT; break;
        case 'M': actual = KEY_MOD_RMETA; break;
    }
    return KeyItem(KeyItem::kType::MODIFIER,actual);
  }
  KeyItem L(uint8_t layer){
    return KeyItem(KeyItem::kType::LAYER,layer);
  }
  KeyItem N(){return KeyItem();}
  KeyItem C(char c){
    uint8_t actual;

    switch (c) {
        case 'a': actual = KEY_A; break;
        case 'b': actual = KEY_B; break;
        case 'c': actual = KEY_C; break;
        case 'd': actual = KEY_D; break;
        case 'e': actual = KEY_E; break;
        case 'f': actual = KEY_F; break;
        case 'g': actual = KEY_G; break;
        case 'h': actual = KEY_H; break;
        case 'i': actual = KEY_I; break;
        case 'j': actual = KEY_J; break;
        case 'k': actual = KEY_K; break;
        case 'l': actual = KEY_L; break;
        case 'm': actual = KEY_M; break;
        case 'n': actual = KEY_N; break;
        case 'o': actual = KEY_O; break;
        case 'p': actual = KEY_P; break;
        case 'q': actual = KEY_Q; break;
        case 'r': actual = KEY_R; break;
        case 's': actual = KEY_S; break;
        case 't': actual = KEY_T; break;
        case 'u': actual = KEY_U; break;
        case 'v': actual = KEY_V; break;
        case 'w': actual = KEY_W; break;
        case 'x': actual = KEY_X; break;
        case 'y': actual = KEY_Y; break;
        case 'z': actual = KEY_Z; break;

        case '0': actual = KEY_0; break;
        case '1': actual = KEY_1; break;
        case '2': actual = KEY_2; break;
        case '3': actual = KEY_3; break;
        case '4': actual = KEY_4; break;
        case '5': actual = KEY_5; break;
        case '6': actual = KEY_6; break;
        case '7': actual = KEY_7; break;
        case '8': actual = KEY_8; break;
        case '9': actual = KEY_9; break;

        case ' ': actual = KEY_SPACE; break;
        case '/': actual = KEY_SLASH; break;
        case '.': actual = KEY_DOT; break;
        case ',': actual = KEY_COMMA; break;
        case ';': actual = KEY_SEMICOLON; break;


    }
    return KeyItem(KeyItem::kType::CHARACTER, actual);
    }

}

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
  listPlus<uint8_t> modifiers;
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

void pressKeys(uint8_t length, KeyItem *maps, bool *state, reportManager rm) {
  for (unsigned int i = 0; i < length; i++) {
    if (state[i]) {
      rm.addKey(maps[i]);
    }
  }
}
} // namespace keyMap
