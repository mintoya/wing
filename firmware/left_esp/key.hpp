#pragma once
#include "debounce.hpp"
#include <stdint.h>
#include <stdio.h>
#define TAPDANCEDEFAULTTIMEOUT ((unsigned long)300)
#include "my-list/my-list.h"
#include "my-list/my-list.hpp"

extern unsigned long millis(void);

extern void (*keyboardFunctions[10])(void);

struct KeyItem {
  uint8_t character; // not really a character
  enum kType : char {
    PASSTHROUGH = 0,
    CHARACTER = 1,
    LAYER = 2,
    TAPDANCE = 4,
    MODIFIER = 8,
    FUNCTIONCALL = 64, // not implemented
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
static void printKeyItem(const KeyItem &k) {
  switch (k.type) {
  case KeyItem::CHARACTER:
    Serial.printf("K(%d)", k.character);
    break;
  case KeyItem::MODIFIER:
    Serial.printf("M(%d)", k.character);
    break;
  case KeyItem::LAYER:
    Serial.printf("L(%d)", k.character);
  case KeyItem::TAPDANCE:
    Serial.printf("TD(%d)", k.character);
    break;
  case KeyItem::FUNCTIONCALL:
    Serial.printf("F(%d)", k.character);
    break;
  case KeyItem::PASSTHROUGH:
  default:
    Serial.printf(" . ");
    break;
  }
}
typedef void (*senderFunction)(uint8_t, uint8_t *);

struct reportManager {
  reportManager() {
    wirelessSender = nullptr;
    wiredSender = nullptr;
  };
  senderFunction wirelessSender;
  senderFunction wiredSender;
  reportManager(senderFunction wireless, senderFunction wired) {
    wirelessSender = wireless;
    wiredSender = wired;
    keys.pad(6);
    lastKeys.pad(6);
  }
  uint8_t modifier = 0;
  listPlus<uint8_t> keys;
  uint8_t lastModifier = 0;
  listPlus<uint8_t> lastKeys;

  void send() {
    if (keys.length() < 6)
      keys.pad(6 - keys.length());
    if (lastKeys.length() < 6)
      lastKeys.pad(6 - lastKeys.length());

    bool different = false;

    different |= lastModifier != modifier;
    for (int i = 0; i < 6 && !different; i++)
      different |= lastKeys.get(i) != keys.get(i);
    if (different) {
      if (wirelessSender)
        wirelessSender(modifier, keys.self());
      if (wiredSender)
        wiredSender(modifier, keys.self());
    }

    lastModifier = modifier;
    modifier = 0;

    for (int i = 0; i < 6; i++)
      lastKeys.set(i, keys.get(i));

    keys.clear();
  }
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
};

enum KeyState : char {
  HELDUP = 0,
  PRESSED = 1,
  HELDDOWN = 2,
  RELEASED = 3,
};
// clang-format off
inline KeyState KeyState_up(KeyState state) {
  switch (state) {
    case HELDDOWN:  
    case PRESSED:   
      return RELEASED;
    case RELEASED:  
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
  KeyItem pressActions[10];
  KeyItem holdActions[10];
  unsigned int state;
  unsigned long currentCountDown;
  KeyState keystate;
  bool heldActionTriggered;
};
extern listPlus<tapDance> tapDances;
extern listPlus<listPlus<KeyItem>> keyMapLayers;
extern const unsigned int nrowGpios;
extern const unsigned int ncolGpios;

namespace keyMap {

static void pressKeys(dbool *state, reportManager &rm,
                      unsigned int currentLayer = 0) {
  unsigned int length = nrowGpios * ncolGpios * 2;

  static listPlus<KeyItem> que;

  unsigned long now = millis();

  for (unsigned int i = 0; i < tapDances.length(); i++) {
    tapDance *td = tapDances.self() + i;

    if (td->keystate == PRESSED) {
      if (td->state == 0 || now < td->currentCountDown) {
        td->state++;
        td->currentCountDown = now + TAPDANCEDEFAULTTIMEOUT;
      }
    } else if (td->state) {
      if (now > td->currentCountDown) {
        if (td->keystate == RELEASED) {
          if (!td->heldActionTriggered) {
            que.append(td->pressActions[(td->state - 1) % 10]);
          }
          td->state = 0;
          td->heldActionTriggered = false;
        } else if (td->keystate == HELDDOWN) {
          que.append(td->holdActions[(td->state - 1) % 10]);
          td->heldActionTriggered = true;
        } else if (td->keystate == HELDUP) {
          que.append(td->pressActions[(td->state - 1) % 10]);
          td->state = 0;
        }
      }
    }
  }

  for (int i = 0; i < que.length(); i++) {
    KeyItem currentKey = que.get(i);
    switch (currentKey.type) {
    case KeyItem::kType::FUNCTIONCALL: {
      keyboardFunctions[currentKey.character]();
    } break;
    case KeyItem::kType::LAYER: {
      currentLayer = currentKey.character; // idk
    }
    case KeyItem::kType::TAPDANCE: {
      tapDances.self()[currentKey.character].keystate =
          KeyState_down(tapDances.self()[currentKey.character].keystate);
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
    if (state[i].get() &&
        keyMapLayers.get(currentLayer).get(i).type == KeyItem::kType::LAYER) {
      state[i].set(false);
      return pressKeys(state, rm,
                       keyMapLayers.get(currentLayer).get(i).character);
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
    KeyItem currentKey = keyMapLayers.get(currentLayer).get(i);
    if (currentKey.type == KeyItem::kType::PASSTHROUGH) {
      for (int j = currentLayer; j >= 0; j--) {
        if (!(keyMapLayers.get(j).get(i).type == KeyItem::kType::PASSTHROUGH)) {
          currentKey = keyMapLayers.get(j).get(i);
          break;
        }
      }
    }
    if (state[i].get()) {
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
        tapDances.self()[currentKey.character].keystate =
            KeyState_down(tapDances.self()[currentKey.character].keystate);
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
        tapDances.self()[currentKey.character].keystate =
            KeyState_up(tapDances.self()[currentKey.character].keystate);
      }
    }
  }
  que.clear();
}

} // namespace keyMap
