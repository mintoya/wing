#pragma once
#include "fileSystemInterface.hpp"
#include "my-lib/fptr.h"
#include "my-lib/macros.h"
#include <cstring>
#include <functional>
#include <stdint.h>
#include <string>
constexpr uint TAPDANCEDEFAULTTIMEOUT = 300;
#include "my-lib/mylist.h"
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
  enum kType : c8 {
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
           : c == '`'  ? KeyItem{KEY_GRAVE /*      */, KeyItem::CHARACTER}
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
KeyItem M(fptr fp) {
  auto in = fp.ptr;
  if (fp.width == 2) {
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
  } else {
    return KeyItem{};
  }
}
KeyItem K(fptr in) {
  return in == fp("\\/")     ? KeyItem{KEY_SLASH /*    */, KeyItem::CHARACTER}
         : in == fp("\\\\")  ? KeyItem{KEY_BACKSLASH /**/, KeyItem::CHARACTER}
         : in == fp("SPC")   ? KeyItem{KEY_SPACE /*    */, KeyItem::CHARACTER}
         : in == fp("ENT")   ? KeyItem{KEY_ENTER /*    */, KeyItem::CHARACTER}
         : in == fp("TAB")   ? KeyItem{KEY_TAB /*      */, KeyItem::CHARACTER}
         : in == fp("ESC")   ? KeyItem{KEY_ESC /*      */, KeyItem::CHARACTER}
         : in == fp("BKS")   ? KeyItem{KEY_BACKSPACE /**/, KeyItem::CHARACTER}
         : in == fp("INS")   ? KeyItem{KEY_INSERT /*   */, KeyItem::CHARACTER}
         : in == fp("DEL")   ? KeyItem{KEY_DELETE /*   */, KeyItem::CHARACTER}
         : in == fp("DOWN")  ? KeyItem{KEY_DOWN /*     */, KeyItem::CHARACTER}
         : in == fp("LEFT")  ? KeyItem{KEY_LEFT /*     */, KeyItem::CHARACTER}
         : in == fp("RIGHT") ? KeyItem{KEY_RIGHT /*    */, KeyItem::CHARACTER}
         : in == fp("UP")    ? KeyItem{KEY_UP /*       */, KeyItem::CHARACTER}
         : in == fp("F1")    ? KeyItem{KEY_F1 /*       */, KeyItem::CHARACTER}
         : in == fp("F2")    ? KeyItem{KEY_F2 /*       */, KeyItem::CHARACTER}
         : in == fp("F3")    ? KeyItem{KEY_F3 /*       */, KeyItem::CHARACTER}
         : in == fp("F4")    ? KeyItem{KEY_F4 /*       */, KeyItem::CHARACTER}
         : in == fp("F5")    ? KeyItem{KEY_F5 /*       */, KeyItem::CHARACTER}
         : in == fp("F6")    ? KeyItem{KEY_F6 /*       */, KeyItem::CHARACTER}
         : in == fp("F7")    ? KeyItem{KEY_F7 /*       */, KeyItem::CHARACTER}
         : in == fp("F8")    ? KeyItem{KEY_F8 /*       */, KeyItem::CHARACTER}
         : in == fp("F9")    ? KeyItem{KEY_F9 /*       */, KeyItem::CHARACTER}
         : in == fp("F10")   ? KeyItem{KEY_F10 /*      */, KeyItem::CHARACTER}
         : in == fp("F11")   ? KeyItem{KEY_F11 /*      */, KeyItem::CHARACTER}
         : in == fp("F12")   ? KeyItem{KEY_F12 /*      */, KeyItem::CHARACTER}
         : in.width == 1     ? K(in.ptr[0])
                             : KeyItem{};
}
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
  void addKey(const KeyItem k) {
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

struct KeyState {
  enum : c8 {
    HELDUP,
    PRESSED,
    HELDDOWN,
    RELEASED,
  } state : 2;
  inline void up() {
    switch (state) {
      case HELDDOWN:
      case PRESSED:
        state = RELEASED;
        break;
      default:
        state = HELDUP;
        break;
    }
  }
  inline void down() {
    switch (state) {
      case HELDUP:
      case RELEASED:
        state = PRESSED;
        break;
      default:
        state = HELDDOWN;
        break;
    }
  }
};

struct tapDance {
  KeyItem pressActions[10];
  KeyItem holdActions[10];
  uint state;
  usize currentCountDown;
  KeyState keystate;
  bool heldActionTriggered;
};

extern slice_t<tapDance> tapDances;
extern slice_t<slice_t<KeyItem>> keyMapLayers;

namespace keyMap {

template <usize rows, usize cols>
static void pressKeys(bool matrix[rows][cols * 2], reportManager &rm) {
  constexpr auto length = rows * cols * 2;
  auto matrix_get = [matrix](uint i) -> bool & { return matrix[i / countof(matrix[0])][i % countof(matrix[0])]; };
  static mList(uint) activeKeys = mList_init(stdAlloc, uint);
  static mList(KeyItem) reportque = mList_init(stdAlloc, KeyItem);
  // will always finish at 0 length
  u8 currentLayer = 0;
  auto now = millis();

  // since tapdances need to know when theyre not pressed
  bool tapdances_check[tapDances.len];
  memset(tapdances_check, 0, sizeof(tapdances_check));

  for (auto i = 0; i < length; i++)
    if (matrix_get(i))
      mList_push(activeKeys, i);

  for (auto i = 0; i < mList_len(activeKeys); i++) {
    uint ki = mList_arr(activeKeys)[i];
    KeyItem k =
        currentLayer < keyMapLayers.len && ki < keyMapLayers[currentLayer].len
            ? keyMapLayers[currentLayer][ki]
            : KeyItem{};
    switch (k.type) {
      case KeyItem::LAYER:
        currentLayer = k.character;
        mList_rem(activeKeys, i);
        i = -1;
        break;
      case KeyItem::TAPDANCE:
        tapdances_check[k.character] = true;
        mList_rem(activeKeys, i);
        i = -1;
        break;
      case KeyItem::FUNCTIONCALL: // TODO
        mList_rem(activeKeys, i);
        break;
      case KeyItem::PASSTHROUGH_:
        break;
    }
  }

  for (auto i = 0; i < tapDances.len; i++)
    tapdances_check[i] ? tapDances[i].keystate.down() : tapDances[i].keystate.up();
  std::function<void(KeyItem)> td_key_handle = [&currentLayer](KeyItem k) -> void {
    switch (k.type) {
      case KeyItem::TAPDANCE: // no way
      case KeyItem::PASSTHROUGH_:
        break;
      case KeyItem::LAYER:
        currentLayer = k.character;
        break;
      case KeyItem::CHARACTER:
      case KeyItem::MODIFIER:
        mList_push(reportque, k);
      case KeyItem::FUNCTIONCALL: // TODO
        break;
    }
  };
  for (tapDance &td : tapDances) {
    if (td.keystate.state == KeyState::PRESSED) {
      if (td.state == 0 || now < td.currentCountDown) {
        td.state++;
        td.currentCountDown = now + TAPDANCEDEFAULTTIMEOUT;
      }
    } else if (td.state) {
      if (mList_len(activeKeys) || now > td.currentCountDown) {
        if (td.keystate.state == KeyState::RELEASED) {
          if (!td.heldActionTriggered)
            td_key_handle(td.pressActions[(td.state - 1) % 10]);
          td.state = 0;
          td.heldActionTriggered = false;
        } else if (td.keystate.state == KeyState::HELDDOWN) {
          td_key_handle(td.holdActions[(td.state - 1) % 10]);
          td.heldActionTriggered = true;
        } else if (td.keystate.state == KeyState::HELDUP) {
          td_key_handle(td.pressActions[(td.state - 1) % 10]);
          td.state = 0;
        }
      }
    }
  }
  for (auto kindex : *mList_vla(activeKeys)) {
    for (auto useLayer = currentLayer;; useLayer--) {
      KeyItem currentKey =
          useLayer < keyMapLayers.len && kindex < keyMapLayers[useLayer].len
              ? keyMapLayers[useLayer][kindex]
              : KeyItem{};
      if (currentKey.type == KeyItem::PASSTHROUGH_ && useLayer)
        continue;
      switch (currentKey.type) {
        case KeyItem::MODIFIER:
        case KeyItem::CHARACTER:
          mList_push(reportque, currentKey);
          break;
        case KeyItem::FUNCTIONCALL: // TODO
          break;
        default: // no more layer/tapdance recursion
          break;
      }
      break;
    }
  }
  mList_clear(activeKeys);
  while (mList_len(reportque))
    rm.addKey(mList_pop(reportque));
}
} // namespace keyMap
