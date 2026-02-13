#ifndef HID_KEY_NAMES_H
#define HID_KEY_NAMES_H

#include "hid_keys.h"
#include "key.hpp"
// #include "kml/kml.h"
#include "my-lib/fptr.h"
#include <cstdint>
#include <stdint.h>
#include <string.h>

fptr fp_inside(char bounds[2], fptr in) {
  u8 open = bounds[0];
  u8 close = bounds[1];

  while (in.width > 0 && *in.ptr != open) {
    in.ptr++;
    in.width--;
  }

  if (in.width <= 1)
    return nullFptr;

  in.ptr++;
  in.width--;

  int depth = 1;
  int i = 0;
  for (; i < in.width; i++) {
    if (in.ptr[i] == open) {
      depth++;
    } else if (in.ptr[i] == close) {
      depth--;
      if (depth == 0)
        break;
    }
  }

  in.width = i;
  return in;
}
fptr fp_around(u8 bounds[2], fptr in) {
  u8 open = bounds[0];
  u8 close = bounds[1];

  while (in.width > 0 && *in.ptr != open) {
    in.ptr++;
    in.width--;
  }

  if (in.width == 0)
    return nullFptr;

  int depth = 0;
  int i = 0;
  for (; i < in.width; i++) {
    if (in.ptr[i] == open) {
      depth++;
    } else if (in.ptr[i] == close) {
      depth--;
      if (depth == 0) {
        i++;
        break;
      }
    }
  }

  in.width = i;
  return in;
}
usize fp_indexOf(fptr in, u8 c) {
  usize i = 0;
  for (; i < in.width && in.ptr[i] != c; i++)
    ;
  return i;
}
fptr fp_trim(fptr in) {
  fptr out = in;
  while (*in.ptr < 33) {
    in.ptr++;
    in.width--;
  }
  while (in.ptr[in.width - 1] < 33) {
    in.width--;
  }
  return in;
}

static const u8 keyCodes[] = {
    KEY_NONE,
    KEY_MOD_LCTRL,
    KEY_MOD_LSHIFT,
    KEY_MOD_LALT,
    KEY_MOD_LMETA,
    KEY_MOD_RCTRL,
    KEY_MOD_RSHIFT,
    KEY_MOD_RALT,
    KEY_MOD_RMETA,
    KEY_ERR_OVF,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_ENTER,
    KEY_ESC,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_SPACE,
    KEY_MINUS,
    KEY_EQUAL,
    KEY_LEFTBRACE,
    KEY_RIGHTBRACE,
    KEY_BACKSLASH,
    KEY_HASHTILDE,
    KEY_SEMICOLON,
    KEY_APOSTROPHE,
    KEY_GRAVE,
    KEY_COMMA,
    KEY_DOT,
    KEY_SLASH,
    KEY_CAPSLOCK,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_SYSRQ,
    KEY_SCROLLLOCK,
    KEY_PAUSE,
    KEY_INSERT,
    KEY_HOME,
    KEY_PAGEUP,
    KEY_DELETE,
    KEY_END,
    KEY_PAGEDOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,
    KEY_NUMLOCK,
    KEY_KPSLASH,
    KEY_KPASTERISK,
    KEY_KPMINUS,
    KEY_KPPLUS,
    KEY_KPENTER,
    KEY_KP1,
    KEY_KP2,
    KEY_KP3,
    KEY_KP4,
    KEY_KP5,
    KEY_KP6,
    KEY_KP7,
    KEY_KP8,
    KEY_KP9,
    KEY_KP0,
    KEY_KPDOT,
    KEY_102ND,
    KEY_COMPOSE,
    KEY_POWER,
    KEY_KPEQUAL,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_F16,
    KEY_F17,
    KEY_F18,
    KEY_F19,
    KEY_F20,
    KEY_F21,
    KEY_F22,
    KEY_F23,
    KEY_F24,
    KEY_OPEN,
    KEY_HELP,
    KEY_PROPS,
    KEY_FRONT,
    KEY_STOP,
    KEY_AGAIN,
    KEY_UNDO,
    KEY_CUT,
    KEY_COPY,
    KEY_PASTE,
    KEY_FIND,
    KEY_MUTE,
    KEY_VOLUMEUP,
    KEY_VOLUMEDOWN,
    KEY_KPCOMMA,
    KEY_RO,
    KEY_KATAKANAHIRAGANA,
    KEY_YEN,
    KEY_HENKAN,
    KEY_MUHENKAN,
    KEY_KPJPCOMMA,
    KEY_HANGEUL,
    KEY_HANJA,
    KEY_KATAKANA,
    KEY_HIRAGANA,
    KEY_ZENKAKUHANKAKU,
    KEY_KPLEFTPAREN,
    KEY_KPRIGHTPAREN,
    KEY_LEFTCTRL,
    KEY_LEFTSHIFT,
    KEY_LEFTALT,
    KEY_LEFTMETA,
    KEY_RIGHTCTRL,
    KEY_RIGHTSHIFT,
    KEY_RIGHTALT,
    KEY_RIGHTMETA,
    KEY_MEDIA_PLAYPAUSE,
    KEY_MEDIA_STOPCD,
    KEY_MEDIA_PREVIOUSSONG,
    KEY_MEDIA_NEXTSONG,
    KEY_MEDIA_EJECTCD,
    KEY_MEDIA_VOLUMEUP,
    KEY_MEDIA_VOLUMEDOWN,
    KEY_MEDIA_MUTE,
    KEY_MEDIA_WWW,
    KEY_MEDIA_BACK,
    KEY_MEDIA_FORWARD,
    KEY_MEDIA_STOP,
    KEY_MEDIA_FIND,
    KEY_MEDIA_SCROLLUP,
    KEY_MEDIA_SCROLLDOWN,
    KEY_MEDIA_EDIT,
    KEY_MEDIA_SLEEP,
    KEY_MEDIA_COFFEE,
    KEY_MEDIA_REFRESH,
    KEY_MEDIA_CALC,
};
static const char *keyNames[] = {
    "KEY_NONE",
    "KEY_MOD_LCTRL",
    "KEY_MOD_LSHIFT",
    "KEY_MOD_LALT",
    "KEY_MOD_LMETA",
    "KEY_MOD_RCTRL",
    "KEY_MOD_RSHIFT",
    "KEY_MOD_RALT",
    "KEY_MOD_RMETA",
    "KEY_ERR_OVF",
    "KEY_A",
    "KEY_B",
    "KEY_C",
    "KEY_D",
    "KEY_E",
    "KEY_F",
    "KEY_G",
    "KEY_H",
    "KEY_I",
    "KEY_J",
    "KEY_K",
    "KEY_L",
    "KEY_M",
    "KEY_N",
    "KEY_O",
    "KEY_P",
    "KEY_Q",
    "KEY_R",
    "KEY_S",
    "KEY_T",
    "KEY_U",
    "KEY_V",
    "KEY_W",
    "KEY_X",
    "KEY_Y",
    "KEY_Z",
    "KEY_1",
    "KEY_2",
    "KEY_3",
    "KEY_4",
    "KEY_5",
    "KEY_6",
    "KEY_7",
    "KEY_8",
    "KEY_9",
    "KEY_0",
    "KEY_ENTER",
    "KEY_ESC",
    "KEY_BACKSPACE",
    "KEY_TAB",
    "KEY_SPACE",
    "KEY_MINUS",
    "KEY_EQUAL",
    "KEY_LEFTBRACE",
    "KEY_RIGHTBRACE",
    "KEY_BACKSLASH",
    "KEY_HASHTILDE",
    "KEY_SEMICOLON",
    "KEY_APOSTROPHE",
    "KEY_GRAVE",
    "KEY_COMMA",
    "KEY_DOT",
    "KEY_SLASH",
    "KEY_CAPSLOCK",
    "KEY_F1",
    "KEY_F2",
    "KEY_F3",
    "KEY_F4",
    "KEY_F5",
    "KEY_F6",
    "KEY_F7",
    "KEY_F8",
    "KEY_F9",
    "KEY_F10",
    "KEY_F11",
    "KEY_F12",
    "KEY_SYSRQ",
    "KEY_SCROLLLOCK",
    "KEY_PAUSE",
    "KEY_INSERT",
    "KEY_HOME",
    "KEY_PAGEUP",
    "KEY_DELETE",
    "KEY_END",
    "KEY_PAGEDOWN",
    "KEY_RIGHT",
    "KEY_LEFT",
    "KEY_DOWN",
    "KEY_UP",
    "KEY_NUMLOCK",
    "KEY_KPSLASH",
    "KEY_KPASTERISK",
    "KEY_KPMINUS",
    "KEY_KPPLUS",
    "KEY_KPENTER",
    "KEY_KP1",
    "KEY_KP2",
    "KEY_KP3",
    "KEY_KP4",
    "KEY_KP5",
    "KEY_KP6",
    "KEY_KP7",
    "KEY_KP8",
    "KEY_KP9",
    "KEY_KP0",
    "KEY_KPDOT",
    "KEY_102ND",
    "KEY_COMPOSE",
    "KEY_POWER",
    "KEY_KPEQUAL",
    "KEY_F13",
    "KEY_F14",
    "KEY_F15",
    "KEY_F16",
    "KEY_F17",
    "KEY_F18",
    "KEY_F19",
    "KEY_F20",
    "KEY_F21",
    "KEY_F22",
    "KEY_F23",
    "KEY_F24",
    "KEY_OPEN",
    "KEY_HELP",
    "KEY_PROPS",
    "KEY_FRONT",
    "KEY_STOP",
    "KEY_AGAIN",
    "KEY_UNDO",
    "KEY_CUT",
    "KEY_COPY",
    "KEY_PASTE",
    "KEY_FIND",
    "KEY_MUTE",
    "KEY_VOLUMEUP",
    "KEY_VOLUMEDOWN",
    "KEY_KPCOMMA",
    "KEY_RO",
    "KEY_KATAKANAHIRAGANA",
    "KEY_YEN",
    "KEY_HENKAN",
    "KEY_MUHENKAN",
    "KEY_KPJPCOMMA",
    "KEY_HANGEUL",
    "KEY_HANJA",
    "KEY_KATAKANA",
    "KEY_HIRAGANA",
    "KEY_ZENKAKUHANKAKU",
    "KEY_KPLEFTPAREN",
    "KEY_KPRIGHTPAREN",
    "KEY_LEFTCTRL",
    "KEY_LEFTSHIFT",
    "KEY_LEFTALT",
    "KEY_LEFTMETA",
    "KEY_RIGHTCTRL",
    "KEY_RIGHTSHIFT",
    "KEY_RIGHTALT",
    "KEY_RIGHTMETA",
    "KEY_MEDIA_PLAYPAUSE",
    "KEY_MEDIA_STOPCD",
    "KEY_MEDIA_PREVIOUSSONG",
    "KEY_MEDIA_NEXTSONG",
    "KEY_MEDIA_EJECTCD",
    "KEY_MEDIA_VOLUMEUP",
    "KEY_MEDIA_VOLUMEDOWN",
    "KEY_MEDIA_MUTE",
    "KEY_MEDIA_WWW",
    "KEY_MEDIA_BACK",
    "KEY_MEDIA_FORWARD",
    "KEY_MEDIA_STOP",
    "KEY_MEDIA_FIND",
    "KEY_MEDIA_SCROLLUP",
    "KEY_MEDIA_SCROLLDOWN",
    "KEY_MEDIA_EDIT",
    "KEY_MEDIA_SLEEP",
    "KEY_MEDIA_COFFEE",
    "KEY_MEDIA_REFRESH",
    "KEY_MEDIA_CALC",
};
static_assert(countof(keyCodes) == countof(keyNames), "one code per key");

static inline u8 um_asUint(fptr um) {
  u8 res = 0;
  for (usize i = 0; i < um.width; i++) {
    char c = um.ptr[i];
    if (c < '0' || c > '9')
      break;
    res = res * 10 + (c - '0');
  }
  return res;
}
static KeyItem kn_Match_Special(fptr name) {
  name = fp_trim(name);
  char D = name.ptr[0];
  fptr id = fp_trim(fp_inside("()", name));
  switch (D) {
  case 'M':
    if (id == fp("lc"))
      return KeyItem(KEY_MOD_LCTRL, KeyItem::MODIFIER);
    if (id == fp("la"))
      return KeyItem(KEY_MOD_LALT, KeyItem::MODIFIER);
    if (id == fp("ls"))
      return KeyItem(KEY_MOD_LSHIFT, KeyItem::MODIFIER);
    if (id == fp("lm"))
      return KeyItem(KEY_MOD_LMETA, KeyItem::MODIFIER);
    if (id == fp("rc"))
      return KeyItem(KEY_MOD_RCTRL, KeyItem::MODIFIER);
    if (id == fp("ra"))
      return KeyItem(KEY_MOD_RALT, KeyItem::MODIFIER);
    if (id == fp("rs"))
      return KeyItem(KEY_MOD_RSHIFT, KeyItem::MODIFIER);
    if (id == fp("rm"))
      return KeyItem(KEY_MOD_RMETA, KeyItem::MODIFIER);
    break;
  case 'L':
    return KeyItem(um_asUint(id), KeyItem::LAYER);
  case 'T':
    return KeyItem(um_asUint(id), KeyItem::TAPDANCE);
  }
  Serial.print("couldn't find key from mods:  ");
  Serial.printf("%.*s\n", (int)name.width, (char *)name.ptr);
  return KeyItem();
}
static KeyItem kn_Match(fptr name) {
  Serial.write((u8 *)name.ptr, name.width);
  Serial.println("");
  if (fp_indexOf(name, '(') < name.width) {
    return kn_Match_Special(name);
  }
  for (unsigned int i = 0; i < sizeof(keyCodes) / sizeof(u8); i++) {
    const char *keyName =
        keyNames[i]; // array of key names corresponding to keyCodes

    // compare first 'name.width' characters
    if (strncmp((char *)name.ptr, keyName, name.width) == 0 &&
        keyName[name.width] == '\0') {
      return KeyItem(keyCodes[i]);
    }
  }

  Serial.print("couldn't find key:  ");
  Serial.printf("%.*s\n", (int)name.width, (char *)name.ptr);
  return KeyItem();
}

#endif
