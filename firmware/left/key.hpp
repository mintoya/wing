
#pragma once
#define TAPDANCEDEFAULTTIMEOUT 200
#include "hid_keys.h"

#include "arduino/hid/Adafruit_USBD_HID.h"
#include "my-list.h"
#include "my-list.hpp"
#include <bluefruit.h>
#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

extern unsigned long millis(void);
extern void (*keyboardFunctions[255])(void);

// stores type of data and data
struct KeyItem {
  uint8_t character; // not really a character
  enum kType : char {
    PASSTHROUGH = 0,
    CHARACTER = 1,
    LAYER = 2,
    TAPDANCE = 4,
    MODIFIER = 8,
    FUNCTIONCALL = 64,
  } type;
  inline KeyItem(void) {
    character = 0;
    type = kType::PASSTHROUGH;
  }
  inline KeyItem(uint8_t data) {
    character = data;
    type = (data) ? (kType::CHARACTER) : (kType::PASSTHROUGH);
  }
  inline KeyItem(uint8_t data, kType t) {
    character = data;
    type = t;
  }
};

namespace KeyMapDec {

KeyItem M(char c) {
  uint8_t actual;
  switch (c) {
  case 'c':
    actual = KEY_MOD_LCTRL;
    break;
  case 's':
    actual = KEY_MOD_LSHIFT;
    break;
  case 'a':
    actual = KEY_MOD_LALT;
    break;
  case 'm':
    actual = KEY_MOD_LMETA;
    break;
  case 'C':
    actual = KEY_MOD_RCTRL;
    break;
  case 'S':
    actual = KEY_MOD_RSHIFT;
    break;
  case 'A':
    actual = KEY_MOD_RALT;
    break;
  case 'M':
    actual = KEY_MOD_RMETA;
    break;
  }
  return KeyItem(actual, KeyItem::kType::MODIFIER);
}
KeyItem L(uint8_t layer) { return KeyItem(layer, KeyItem::kType::LAYER); }
KeyItem FN(uint8_t functionIndex) {
  return KeyItem(functionIndex, KeyItem::kType::FUNCTIONCALL);
}
// KeyItem TD(uint8_t tapdanceIndex) {
//   static listPlus<uint8_t> tapdanceUsed;
//   if (List_search(tapdanceUsed.ptr, &tapdanceIndex) > 0) {
//     tapdanceUsed.append(tapdanceIndex);
//   return KeyItem(tapdanceIndex, KeyItem::kType::TAPDANCE);
//   }
//   return KeyItem();
// }

KeyItem TD(uint8_t tapdanceIndex) {
  return KeyItem(tapdanceIndex, KeyItem::kType::TAPDANCE);
}
KeyItem C(char c) {
  uint8_t actual;

  switch (c) {
  case 'a':
    actual = KEY_A;
    break;
  case 'b':
    actual = KEY_B;
    break;
  case 'c':
    actual = KEY_C;
    break;
  case 'd':
    actual = KEY_D;
    break;
  case 'e':
    actual = KEY_E;
    break;
  case 'f':
    actual = KEY_F;
    break;
  case 'g':
    actual = KEY_G;
    break;
  case 'h':
    actual = KEY_H;
    break;
  case 'i':
    actual = KEY_I;
    break;
  case 'j':
    actual = KEY_J;
    break;
  case 'k':
    actual = KEY_K;
    break;
  case 'l':
    actual = KEY_L;
    break;
  case 'm':
    actual = KEY_M;
    break;
  case 'n':
    actual = KEY_N;
    break;
  case 'o':
    actual = KEY_O;
    break;
  case 'p':
    actual = KEY_P;
    break;
  case 'q':
    actual = KEY_Q;
    break;
  case 'r':
    actual = KEY_R;
    break;
  case 's':
    actual = KEY_S;
    break;
  case 't':
    actual = KEY_T;
    break;
  case 'u':
    actual = KEY_U;
    break;
  case 'v':
    actual = KEY_V;
    break;
  case 'w':
    actual = KEY_W;
    break;
  case 'x':
    actual = KEY_X;
    break;
  case 'y':
    actual = KEY_Y;
    break;
  case 'z':
    actual = KEY_Z;
    break;

  case '0':
    actual = KEY_0;
    break;
  case '1':
    actual = KEY_1;
    break;
  case '2':
    actual = KEY_2;
    break;
  case '3':
    actual = KEY_3;
    break;
  case '4':
    actual = KEY_4;
    break;
  case '5':
    actual = KEY_5;
    break;
  case '6':
    actual = KEY_6;
    break;
  case '7':
    actual = KEY_7;
    break;
  case '8':
    actual = KEY_8;
    break;
  case '9':
    actual = KEY_9;
    break;

  case ' ':
    actual = KEY_SPACE;
    break;
  case '/':
    actual = KEY_SLASH;
    break;
  case '.':
    actual = KEY_DOT;
    break;
  case ',':
    actual = KEY_COMMA;
    break;
  case ';':
    actual = KEY_SEMICOLON;
    break;
  case '-':
    actual = KEY_MINUS;
    break;
  case '=':
    actual = KEY_EQUAL;
    break;
  }
  return KeyItem(actual, KeyItem::kType::CHARACTER);
}

} // namespace KeyMapDec

// clang-format on

struct reportManager {
  reportManager() {};
  BLEHidAdafruit blehid;
  Adafruit_USBD_HID usbhid;
  reportManager(BLEHidAdafruit ble, Adafruit_USBD_HID usb) {
    blehid = ble;
    usbhid = usb;
  }
  // listPlus<uint8_t> modifiers;
  uint8_t modifier;
  listPlus<uint8_t> keys;

  void addKey(KeyItem k) {
    switch (k.type) {
    case KeyItem::MODIFIER:
      modifier |= k.character;
      // modifiers.append(k.character);
      break;
    case KeyItem::CHARACTER:
      keys.append(k.character);
      break;
    default:
      break;
    }
  }
  void send() {
    // uint8_t modifierResult = 0;
    // for (size_t i = 0; i < modifiers.length(); i++) {
    //   modifierResult |= modifiers.get(i);
    // }
    while (keys.length() < 6) {
      keys.append(0);
    }
    blehid.keyboardReport(0, modifier, keys.autoCast());
    usbhid.keyboardReport(0, modifier, keys.autoCast());
    // blehid.keyboardReport(0, modifierResult, keys.autoCast());
    // usbhid.keyboardReport(0, modifierResult, keys.autoCast());
    // modifiers.ptr->length = 0;
    modifier = 0;
    keys.ptr->length = 0;
  }
};

enum KeyState : char {
  HELDUP = 0,
  PRESSED = 1,
  HELDDOWN = 2,
  RELEASED = 3,

};
// clang-format off
// pressed

// released
// FIXED VERSION
inline KeyState KeyState_up(KeyState state) {
  switch (state) {
    case HELDDOWN:  
      return RELEASED;
    case PRESSED:   
      return RELEASED;
    case RELEASED:  
      return HELDUP;
    case HELDUP:    
      return HELDUP;
  }
  return HELDUP;
}
inline KeyState KeyState_down(KeyState state) {
  switch (state) {
    case HELDUP:    
    case RELEASED: 
      return PRESSED;
    case PRESSED:
    case HELDDOWN: 
      return HELDDOWN;
  }
  return HELDUP;
}
// clang-format on

struct tapDance {
  /*
   * for the "1" state we should be checking the timeouts[0]
   */
  KeyItem actions[10];
  unsigned int state; // 0: no state , 1: initial state
  unsigned int length;
  unsigned long currentCountDown;
  KeyState keystate;
  /*
   * when the keystate is pressed for the first time
   *  state++;
   *  currentCountDown = now+timeouts[state]
   * when keystate is pressed for other times
   *  IF( now<currentCountDown ){ state++;currentCountDown = now+timeouts[state]
   * } when keystate is released foo(actions[state-1]); state = 0;
   *
   */
};
tapDance tapDances[1] = {
    {
        .actions = {KEY_A, KEY_B, KEY_C, KEY_D},
        .state = 0,
        .length = 3,
        .currentCountDown = 0,
        .keystate = KeyState::HELDUP,
    },
};

namespace keyMap {
// only 10 layers for now
void pressKeys(uint8_t length, KeyItem *maps[10], bool *state,
               reportManager &rm, unsigned int currentLayer = 0) {
  static listPlus<KeyItem> que;

  unsigned long now = millis();

  for (unsigned int i = 0; i < sizeof(tapDances) / sizeof(tapDance); i++) {
    tapDance *td = tapDances + i;
    if (td->state == 0 and td->keystate == PRESSED) {
      td->state++;
      td->currentCountDown = now + TAPDANCEDEFAULTTIMEOUT;
    } else if (td->keystate == PRESSED && now < td->currentCountDown) {
      td->state++;
      td->currentCountDown = now + TAPDANCEDEFAULTTIMEOUT;
    } else if (td->state && td->keystate == RELEASED &&
               now > td->currentCountDown) {
      que.append(td->actions[td->state - 1]);
      td->state = 0;
    } else if (td->state && td->keystate == HELDUP &&
               now > td->currentCountDown) {
      que.append(td->actions[td->state - 1]);
      td->state = 0;
    }
  }

  for (int i = 0; i < que.length(); i++) {
    KeyItem currentKey = que.get(i);
    switch (currentKey.type) {
    case KeyItem::kType::FUNCTIONCALL: {
      keyboardFunctions[currentKey.character]();
    } break;
    case KeyItem::kType::TAPDANCE: {
      tapDances[currentKey.character].keystate =
          KeyState_down(tapDances[currentKey.character].keystate);
    } break;
    default:
      /*
        KeyItem::kType::CHARACTER
        KeyItem::kType::MODIFIER
      */
      rm.addKey(currentKey);
    }
  }

  /*
    KeyItem::kType::CHARACTER
    KeyItem::kType::FUNCTIONCALL
    KeyItem::kType::LAYER
    KeyItem::kType::MODIFIER
    KeyItem::kType::PASSTHROUGH
    KeyItem::kType::TAPDANCE
  */
  // check layers
  for (unsigned int i = 0; i < length; i++) {
    if (state[i] && maps[currentLayer][i].type == KeyItem::kType::LAYER) {
      state[i] = false;
      return pressKeys(length, maps, state, rm,
                       maps[currentLayer][i].character);
    }
  }

  /*
    KeyItem::kType::CHARACTER
    KeyItem::kType::FUNCTIONCALL
    KeyItem::kType::MODIFIER
    KeyItem::kType::PASSTHROUGH
    KeyItem::kType::TAPDANCE
  */

  for (unsigned int i = 0; i < length; i++) {
    KeyItem currentKey = maps[currentLayer][i];
    if (currentKey.type == KeyItem::kType::PASSTHROUGH) {
      for (int j = currentLayer; j >= 0; j--) {
        if (!(maps[j][i].type == KeyItem::kType::PASSTHROUGH)) {
          currentKey = maps[j][i];
          break;
        }
      }
    }
    if (state[i]) {
      /*
        KeyItem::kType::CHARACTER
        KeyItem::kType::FUNCTIONCALL
        KeyItem::kType::MODIFIER
        KeyItem::kType::TAPDANCE
      */
      switch (currentKey.type) {
      case KeyItem::kType::FUNCTIONCALL: {
        keyboardFunctions[currentKey.character]();
      } break;
      case KeyItem::kType::TAPDANCE: {
        tapDances[currentKey.character].keystate =
            KeyState_down(tapDances[currentKey.character].keystate);
      } break;
      default:
        /*
          KeyItem::kType::CHARACTER
          KeyItem::kType::MODIFIER
        */
        rm.addKey(currentKey);
      }
    } else {
      if (currentKey.type == KeyItem::kType::TAPDANCE) {
        tapDances[currentKey.character].keystate =
            KeyState_up(tapDances[currentKey.character].keystate);
      }
    }
  }
  que.ptr->length = 0;
}
} // namespace keyMap
