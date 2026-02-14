#pragma once
#include "fileSystemInterface.hpp"
#include "my-lib/fptr.h"
#include <stdint.h>
#define TAPDANCEDEFAULTTIMEOUT ((unsigned long)300)
#include "my-lib/my-list.h"
#include "my-lib/mytypes.h"

extern unsigned long millis(void);
extern void (*keyboardFunctions[10])(void);

struct KeyItem {
  u8 character; // not really a character
  enum kType : char {
    PASSTHROUGH_ = 0,
    CHARACTER /*   */ = (1 << 0),
    LAYER /*       */ = (1 << 1),
    TAPDANCE /*    */ = (1 << 2),
    MODIFIER /*    */ = (1 << 3),
    FUNCTIONCALL /**/ = (1 << 4), // not implemented
  } type;
  inline constexpr KeyItem(void) : character(0), type(kType::PASSTHROUGH_) {}
  inline constexpr KeyItem(u8 data)
      : character(data), type(data ? kType::CHARACTER : kType::PASSTHROUGH_) {}
  inline constexpr KeyItem(u8 data, kType t) : character(data), type(t) {}
};
REGISTER_PRINTER(KeyItem, {
  // clang-format off
  switch (in.type) {
    case KeyItem::CHARACTER    : { PUTS(U"K"); } break;
    case KeyItem::MODIFIER     : { PUTS(U"M"); } break;
    case KeyItem::LAYER        : { PUTS(U"L"); } break;
    case KeyItem::TAPDANCE     : { PUTS(U"T"); } break;
    case KeyItem::FUNCTIONCALL : { PUTS(U"F"); } break;
    case KeyItem::PASSTHROUGH_ : { PUTS(U"_"); } break;
  }
  // clang-format on
  PUTS(U":");
  switch (in.type) {
    case KeyItem::CHARACTER:
      USENAMEDPRINTER("x", in.character);
      break;
    case KeyItem::MODIFIER:
    case KeyItem::LAYER:
    case KeyItem::TAPDANCE:
    case KeyItem::FUNCTIONCALL:
      USENAMEDPRINTER("u8", in.character);
      break;
    case KeyItem::PASSTHROUGH_:
      PUTS(U"_");
      break;
  }
});
MAKE_PRINT_ARG_TYPE(KeyItem);
typedef void (*senderFunction)(u8, u8 *);

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

extern slice(tapDance) tapDances;

extern slice(slice(KeyItem)) keyMapLayers;

namespace keyMap {

template <usize rows, usize cols>
static void pressKeys(bool keyState[rows][cols * 2], reportManager &rm, unsigned int currentLayer = 0) {
  if (currentLayer > keyMapLayers.len)
    return;
  static bool forceDown = false;
  unsigned int length = rows * cols * 2;
  auto keystate_get = [&](uint index) -> bool & {
    uint width = cols * 2;
    uint r = index / width;
    uint c = index % width;
    return keyState[r][c];
  };

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
  slice(KeyItem) keyMapLayers_arr = keyMapLayers[currentLayer];
  for (unsigned int i = 0; i < length; i++) {
    if (keystate_get(i) &&
        keyMapLayers_arr[i].type == KeyItem::kType::LAYER) {
      keystate_get(i) = (false);
      return pressKeys<rows, cols>(keyState, rm, keyMapLayers_arr[i].character);
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
    KeyItem currentKey = keyMapLayers_arr[i];
    if (currentKey.type == KeyItem::kType::PASSTHROUGH_) {
      for (int j = currentLayer; j >= 0; j--) {
        KeyItem temp = keyMapLayers[j][i];
        if (temp.type != KeyItem::PASSTHROUGH_) {
          currentKey = temp;
          break;
        }
      }
    }
    if (keystate_get(i)) {
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
          forceDown = true;
          /*
            KeyItem::kType::CHARACTER
            KeyItem::kType::MODIFIER
          */
          mList_push(que, currentKey);
      }
    } else {
      if (currentKey.type == KeyItem::kType::TAPDANCE) {
        tapDances[currentKey.character].keystate =
            KeyState_up(tapDances[currentKey.character].keystate);
      }
    }
  }

  for (unsigned int i = 0; i < tapDances.len; i++) {
    tapDance *td = tapDances.ptr + i;
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
        tapDances[que_arr[i].character].keystate =
            KeyState_down(tapDances[que_arr[i].character].keystate);
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
