#pragma once
#include "fileSystemInterface.hpp"
#include "my-lib/fptr.h"
#include <stdint.h>
#define TAPDANCEDEFAULTTIMEOUT ((unsigned long)300)
#include "my-lib/my-list.h"
#include "my-lib/types.h"

extern unsigned long millis(void);

extern void (*keyboardFunctions[10])(void);
struct KeyItem {
  uint8_t character; // not really a character
  enum kType : char {
    PASSTHROUGH_ = 0,
    CHARACTER = 1,
    LAYER = 2,
    TAPDANCE = 4,
    MODIFIER = 8,
    FUNCTIONCALL = 64, // not implemented
  } type;
  inline KeyItem(void) {
    character = 0;
    type = kType::PASSTHROUGH_;
  }
  inline KeyItem(uint8_t data) {
    character = data;
    type = (data) ? (kType::CHARACTER) : (kType::PASSTHROUGH_);
  }
  inline KeyItem(uint8_t data, kType t) {
    character = data;
    type = t;
  }
};
static void printKeyItem(const KeyItem &k) {
  switch (k.type) {
  case KeyItem::CHARACTER:
    print_("K({u8})", k.character);
    break;
  case KeyItem::MODIFIER:
    print_("M({u8})", k.character);
    break;
  case KeyItem::LAYER:
    print_("L({u8})", k.character);
    break;
  case KeyItem::TAPDANCE:
    print_("TD({u8})", k.character);
    break;
  case KeyItem::FUNCTIONCALL:
    print_("FN({u8})", k.character);
    break;
  default:
    print_(" . ");
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
    mList_pad(keys, 6);
    mList_pad(lastKeys, 6);
  }
  u8 modifier = 0;
  mList(u8) keys = mList_init(stdAlloc, u8, 6);
  u8 lastModifier = 0;
  mList(u8) lastKeys = mList_init(stdAlloc, u8, 6);

  void send() {
    if (mList_len(keys) < 6)
      mList_pad(keys, 6 - mList_len(keys));
    if (mList_len(lastKeys) < 6)
      mList_pad(lastKeys, 6 - mList_len(lastKeys));
    bool different = false;

    different |= lastModifier != modifier;
    for (int i = 0; i < 6 && !different; i++)
      different |= mList_get(lastKeys, i) != mList_get(keys, i);
    if (different) {
      // in case some updates get missed
      if (wirelessSender)
        wirelessSender(modifier, mList_arr(keys));
      if (wiredSender)
        wiredSender(modifier, mList_arr(keys));
    }

    lastModifier = modifier;
    modifier = 0;

    for (int i = 0; i < 6; i++)
      mList_set(lastKeys, i, mList_getOr(keys, i, 0));
    ((List *)keys)->length = 0;
  }
  void addKey(KeyItem k) {
    switch (k.type) {
    case KeyItem::MODIFIER:
      modifier |= k.character;
      break;
    case KeyItem::CHARACTER:
      mList_push(keys, k.character);
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

extern mList(tapDance) tapDances;
extern mList(mList(KeyItem)) keyMapLayers;

extern const uint nrowGpios;
extern const uint ncolGpios;

namespace keyMap {

static void pressKeys(bool *state, reportManager &rm,
                      unsigned int currentLayer = 0) {
  if (currentLayer > mList_len(keyMapLayers))
    return;
  static bool forceDown = false;
  unsigned int length = nrowGpios * ncolGpios * 2;

  static mList(KeyItem) que = mList_init(stdAlloc, KeyItem);

  unsigned long now = millis();
  /*
    KeyItem::kType::CHARACTER
    KeyItem::kType::FUNCTIONCALL
    KeyItem::kType::LAYER
    KeyItem::kType::MODIFIER
    KeyItem::kType::PASSTHROUGH_
    KeyItem::kType::TAPDANCE
  */
  mList(KeyItem) keyMapLayers_arr = *mList_get(keyMapLayers, currentLayer);
  for (unsigned int i = 0; i < length; i++) {
    if (state[i] &&
        mList_get(keyMapLayers_arr, i)->type == KeyItem::kType::LAYER) {
      // keyMapLayers.get(currentLayer).get(i).type == KeyItem::kType::LAYER) {
      state[i] = (false);
      return pressKeys(state, rm, mList_get(keyMapLayers_arr, i)->character);
    }
  }

  /*
    KeyItem::kType::CHARACTER
    KeyItem::kType::FUNCTIONCALL
    KeyItem::kType::MODIFIER
    KeyItem::kType::PASSTHROUGH_
    KeyItem::kType::TAPDANCE
  */

  forceDown = false;
  for (auto i = 0; i < length; i++) {
    KeyItem currentKey = *mList_get(keyMapLayers_arr, i);
    if (currentKey.type == KeyItem::kType::PASSTHROUGH_) {
      for (int j = currentLayer; j >= 0; j--) {
        auto temp = mList_get(*mList_get(keyMapLayers, j), i);
        if (!(temp->type == KeyItem::PASSTHROUGH_)) {
          currentKey = *temp;
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
        mList_arr(tapDances)[currentKey.character].keystate =
            KeyState_down(mList_arr(tapDances)[currentKey.character].keystate);
      } break;
      default:
        forceDown = true;
        /*
          KeyItem::kType::CHARACTER
          KeyItem::kType::MODIFIER
        */
        mList_push(que, currentKey);
      }
    } else {
      if (currentKey.type == KeyItem::kType::TAPDANCE) {
        mList_arr(tapDances)[currentKey.character].keystate =
            KeyState_up(mList_arr(tapDances)[currentKey.character].keystate);
      }
    }
  }

  for (unsigned int i = 0; i < mList_len(tapDances); i++) {
    tapDance *td = mList_get(tapDances, i);
    if (!td)
      break;

    if (td->keystate == PRESSED) {
      if (td->state == 0 || now < td->currentCountDown) {
        td->state++;
        td->currentCountDown = now + TAPDANCEDEFAULTTIMEOUT;
      }
    } else if (td->state) {
      if (forceDown || now > td->currentCountDown) {
        if (td->keystate == RELEASED) {
          if (!td->heldActionTriggered) {
            mList_ins(que, 0, td->pressActions[(td->state - 1) % 10]);
          }
          td->state = 0;
          td->heldActionTriggered = false;
        } else if (td->keystate == HELDDOWN) {
          mList_ins(que, 0, td->holdActions[(td->state - 1) % 10]);
          td->heldActionTriggered = true;
        } else if (td->keystate == HELDUP) {
          mList_ins(que, 0, td->pressActions[(td->state - 1) % 10]);
          td->state = 0;
        }
      }
    }
  }

  auto que_arr = mList_arr(que);
  for (int i = 0; i < mList_len(que); i++) {

    switch (que_arr[i].type) {
    case KeyItem::kType::FUNCTIONCALL: {
      keyboardFunctions[que_arr[i].character]();
    } break;
    case KeyItem::kType::LAYER: {
      currentLayer = que_arr[i].character; // idk
    } break;
    case KeyItem::kType::TAPDANCE: {
      mList_arr(tapDances)[que_arr[i].character].keystate =
          KeyState_down(mList_arr(tapDances)[que_arr[i].character].keystate);
    } break;
    default:
      /*
        KeyItem::kType::CHARACTER
        KeyItem::kType::MODIFIER
      */
      rm.addKey(que_arr[i]);
    }
  }
  ((List *)que)->length = 0;
}

} // namespace keyMap
