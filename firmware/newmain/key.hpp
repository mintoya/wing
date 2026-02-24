#pragma once
#include "fileSystemInterface.hpp"
#include "my-lib/fptr.h"
#include <cstring>
#include <stdint.h>
#include <string>
#define TAPDANCEDEFAULTTIMEOUT ((unsigned long)300)
#include "my-lib/my-list.h"
#include "my-lib/mytypes.h"

extern ulong millis(void);
extern void (*keyboardFunctions[10])(void);

#include "hid_keys.h"

template <usize N, usize M, usize I = 0>
struct StrEq {
  static constexpr bool streqfn(const char (&str)[N], const char (&fixed)[M]) {
    return (N == M && str[I] == fixed[I]) ? StrEq<N, M, I + 1>::streqfn(str, fixed) : false;
  }
};
template <usize N, usize M>
struct StrEq<N, M, N> {
  static constexpr bool streqfn(const char (&str)[N], const char (&fixed)[M]) { return true; }
};

template <usize N, usize M>
static constexpr bool streq(const char (&str)[N], const char (&fixed)[M]) {
  return N != M ? false : StrEq<N, M, 0>::streqfn(str, fixed);
}

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
  static constexpr KeyItem K(const char c) {
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' ? KeyItem{(u8)((c | 32) - 'a' + KEY_A) /* */, KeyItem::CHARACTER}
           : c >= '1' && c <= '9'
               ? KeyItem{u8(KEY_1 + c - '1') /*    */, KeyItem::CHARACTER}
           : c == '0'  ? KeyItem{KEY_0 /*          */, KeyItem::CHARACTER}
           : c == ','  ? KeyItem{KEY_COMMA /*      */, KeyItem::CHARACTER}
           : c == '.'  ? KeyItem{KEY_DOT /*        */, KeyItem::CHARACTER}
           : c == '/'  ? KeyItem{KEY_SLASH /*      */, KeyItem::CHARACTER}
           : c == '\\' ? KeyItem{KEY_BACKSLASH /*  */, KeyItem::CHARACTER}
           : c == '\'' ? KeyItem{KEY_APOSTROPHE /* */, KeyItem::CHARACTER}
           : c == ';'  ? KeyItem{KEY_SEMICOLON /*  */, KeyItem::CHARACTER}
           : c == '-'  ? KeyItem{KEY_MINUS /*      */, KeyItem::CHARACTER}
           : c == '='  ? KeyItem{KEY_EQUAL /*      */, KeyItem::CHARACTER}
           : c == '['  ? KeyItem{KEY_LEFTBRACE /*  */, KeyItem::CHARACTER}
           : c == ']'  ? KeyItem{KEY_RIGHTBRACE /* */, KeyItem::CHARACTER}
           : c == '`'  ? KeyItem{KEY_APOSTROPHE /* */, KeyItem::CHARACTER}
                       : strlen("");
  }
  template <usize N>
  static constexpr KeyItem K(const char (&in)[N]) {
    return N == 2
               ? K(in[0])
           : streq(in, "SPC")   ? KeyItem{KEY_SPACE /*    */, KeyItem::CHARACTER}
           : streq(in, "ENT")   ? KeyItem{KEY_ENTER /*    */, KeyItem::CHARACTER}
           : streq(in, "TAB")   ? KeyItem{KEY_TAB /*      */, KeyItem::CHARACTER}
           : streq(in, "ESC")   ? KeyItem{KEY_ESC /*      */, KeyItem::CHARACTER}
           : streq(in, "BKS")   ? KeyItem{KEY_BACKSPACE /**/, KeyItem::CHARACTER}
           : streq(in, "INS")   ? KeyItem{KEY_INSERT /*   */, KeyItem::CHARACTER}
           : streq(in, "DEL")   ? KeyItem{KEY_DELETE /*   */, KeyItem::CHARACTER}
           : streq(in, "DOWN")  ? KeyItem{KEY_DOWN /*     */, KeyItem::CHARACTER}
           : streq(in, "LEFT")  ? KeyItem{KEY_LEFT /*     */, KeyItem::CHARACTER}
           : streq(in, "RIGHT") ? KeyItem{KEY_RIGHT /*    */, KeyItem::CHARACTER}
           : streq(in, "UP")    ? KeyItem{KEY_UP /*       */, KeyItem::CHARACTER}
                                : strlen("");
  }
  static constexpr KeyItem M(const char (&in)[3]) {
    return (in[0] | 32) == 'l'
               ? (in[1] | 32) == 's'
                     ? KeyItem{KEY_MOD_LSHIFT /**/, KeyItem::MODIFIER}
                 : (in[1] | 32) == 'a'
                     ? KeyItem{KEY_MOD_LALT /*  */, KeyItem::MODIFIER}
                 : (in[1] | 32) == 'c'
                     ? KeyItem{KEY_MOD_LCTRL /* */, KeyItem::MODIFIER}
                 : (in[1] | 32) == 'm'
                     ? KeyItem{KEY_MOD_LMETA /* */, KeyItem::MODIFIER}
                     : KeyItem{}
           : (in[0] | 32) == 'r'
               ? (in[1] | 32) == 's'
                     ? KeyItem{KEY_MOD_RSHIFT /**/, KeyItem::MODIFIER}
                 : (in[1] | 32) == 'a'
                     ? KeyItem{KEY_MOD_RALT /*  */, KeyItem::MODIFIER}
                 : (in[1] | 32) == 'c'
                     ? KeyItem{KEY_MOD_RCTRL /* */, KeyItem::MODIFIER}
                 : (in[1] | 32) == 'm'
                     ? KeyItem{KEY_MOD_RMETA /* */, KeyItem::MODIFIER}
                     : KeyItem{}
               : KeyItem{};
  }
  template <usize N>
  static constexpr KeyItem K(u8 num) {
    return num >= 1 & num <= 9
               ? KeyItem{u8(KEY_1 + num), KeyItem::CHARACTER}
               : KeyItem{KEY_0, KeyItem::CHARACTER};
  }
  static constexpr KeyItem L(u8 num) { return KeyItem{num, KeyItem::LAYER}; }
  static constexpr KeyItem T(u8 num) { return KeyItem{num, KeyItem::TAPDANCE}; }
  // static constexpr KeyItem F(u8 num) { return KeyItem{num, KeyItem::MODIFIER}; }
};
namespace KeyItem_ititiazizers {
constexpr KeyItem K(u8 n) { return KeyItem::K(n); }
constexpr KeyItem T(u8 n) { return KeyItem::T(n); }
template <usize N>
constexpr KeyItem K(const char (&s)[N]) { return KeyItem::K<N>(s); }
constexpr KeyItem L(u8 n) { return KeyItem::L(n); }
constexpr KeyItem M(const char (&s)[3]) { return KeyItem::M(s); }
} // namespace KeyItem_ititiazizers

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
    case KeyItem::MODIFIER:
    case KeyItem::LAYER:
    case KeyItem::TAPDANCE:
    case KeyItem::FUNCTIONCALL:
      USENAMEDPRINTER("x", in.character);
      break;
    case KeyItem::PASSTHROUGH_:
      PUTS(U"_ ");
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
  }
  u8 currentModifier = 0;
  u8 lastModifier = 0;
  mList(u8) currentReport = mList_init(stdAlloc, u8, 6);
  mList(u8) lastReport = mList_init(stdAlloc, u8, 6);

  void send() {
    while (mList_len(currentReport) < 6)
      mList_push(currentReport, 0);
    while (mList_len(lastReport) < 6)
      mList_push(lastReport, 0);
    bool different = lastModifier != currentModifier;
    if (!different)
      different = !!memcmp(
          mList_arr(lastReport),
          mList_arr(currentReport), 6
      );

    if (different) {
      u8 *keys = mList_arr(currentReport);
      if (wirelessSender)
        wirelessSender(currentModifier, keys);
      if (wiredSender)
        wiredSender(currentModifier, keys);

      lastModifier = currentModifier;

      auto l = lastReport;
      lastReport = currentReport;
      currentReport = l;
    }

    currentModifier = 0;
    mList_clear(currentReport);
  }
  void addKey(KeyItem k) {
    switch (k.type) {
      case KeyItem::MODIFIER:
        currentModifier |= k.character;
        break;
      case KeyItem::CHARACTER:
        mList_push(currentReport, k.character);
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

struct tapDance {
  KeyItem pressActions[10];
  KeyItem holdActions[10];
  uint state;
  usize currentCountDown;
  KeyState keystate;
  bool heldActionTriggered;
};

extern slice(tapDance) tapDances;

extern slice(slice(KeyItem)) keyMapLayers;

namespace keyMap {

template <usize rows, usize cols>
static void pressKeys(bool keyState[rows][cols * 2], reportManager &rm, unsigned int currentLayer = 0) {
  if (currentLayer >= keyMapLayers.len)
    return;
  static bool forceDown = false;
  unsigned int length = rows * cols * 2;
  auto keystate_get = [&](uint index) -> bool & {
    uint r = index / countof(keyState[0]);
    uint c = index % countof(keyState[0]);
    return keyState[r][c];
  };

  static mList(KeyItem) que = mList_init(stdAlloc, KeyItem);

  ulong now = millis();
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
      for (auto j = currentLayer + 1; j > 0; j--) {
        KeyItem temp = keyMapLayers[j - 1][i];
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
          // keyboardFunctions[currentKey.character]();
        } break;
        case KeyItem::kType::TAPDANCE: {
          if (currentKey.character < tapDances.len) {
            tapDances[currentKey.character].keystate =
                KeyState_down(tapDances[currentKey.character].keystate);
          }
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
        if (currentKey.character < tapDances.len) {
          tapDances[currentKey.character].keystate =
              KeyState_up(tapDances[currentKey.character].keystate);
        }
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
        // keyboardFunctions[que_arr[i].character]();
      } break;
      case KeyItem::kType::LAYER: {
        // idk
        currentLayer = que_arr[i].character;
      } break;
      case KeyItem::kType::TAPDANCE: {
        if (que_arr[i].character < tapDances.len) {
          tapDances[que_arr[i].character].keystate =
              KeyState_down(tapDances[que_arr[i].character].keystate);
        }
      } break;
      default:
        /*
          KeyItem::kType::CHARACTER
          KeyItem::kType::MODIFIER
        */
        rm.addKey(que_arr[i]);
    }
  }
  mList_clear(que);
}

} // namespace keyMap
