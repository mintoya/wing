#ifndef HID_KEY_NAMES_H
#define HID_KEY_NAMES_H

#include "hid_keys.h"
#include "key.hpp"
// #include "kml/kml.h"
#include "my-lib/fptr.h"
#include <cstdint>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <string>

const struct {
  u8 code;
  std::string name;
} keyCodeMap[] = {
    {KEY_NONE, "KEY_NONE"},
    {KEY_MOD_LCTRL, "KEY_MOD_LCTRL"},
    {KEY_MOD_LSHIFT, "KEY_MOD_LSHIFT"},
    {KEY_MOD_LALT, "KEY_MOD_LALT"},
    {KEY_MOD_LMETA, "KEY_MOD_LMETA"},
    {KEY_MOD_RCTRL, "KEY_MOD_RCTRL"},
    {KEY_MOD_RSHIFT, "KEY_MOD_RSHIFT"},
    {KEY_MOD_RALT, "KEY_MOD_RALT"},
    {KEY_MOD_RMETA, "KEY_MOD_RMETA"},
    {KEY_ERR_OVF, "KEY_ERR_OVF"},
    {KEY_A, "KEY_A"},
    {KEY_B, "KEY_B"},
    {KEY_C, "KEY_C"},
    {KEY_D, "KEY_D"},
    {KEY_E, "KEY_E"},
    {KEY_F, "KEY_F"},
    {KEY_G, "KEY_G"},
    {KEY_H, "KEY_H"},
    {KEY_I, "KEY_I"},
    {KEY_J, "KEY_J"},
    {KEY_K, "KEY_K"},
    {KEY_L, "KEY_L"},
    {KEY_M, "KEY_M"},
    {KEY_N, "KEY_N"},
    {KEY_O, "KEY_O"},
    {KEY_P, "KEY_P"},
    {KEY_Q, "KEY_Q"},
    {KEY_R, "KEY_R"},
    {KEY_S, "KEY_S"},
    {KEY_T, "KEY_T"},
    {KEY_U, "KEY_U"},
    {KEY_V, "KEY_V"},
    {KEY_W, "KEY_W"},
    {KEY_X, "KEY_X"},
    {KEY_Y, "KEY_Y"},
    {KEY_Z, "KEY_Z"},
    {KEY_1, "KEY_1"},
    {KEY_2, "KEY_2"},
    {KEY_3, "KEY_3"},
    {KEY_4, "KEY_4"},
    {KEY_5, "KEY_5"},
    {KEY_6, "KEY_6"},
    {KEY_7, "KEY_7"},
    {KEY_8, "KEY_8"},
    {KEY_9, "KEY_9"},
    {KEY_0, "KEY_0"},
    {KEY_ENTER, "KEY_ENTER"},
    {KEY_ESC, "KEY_ESC"},
    {KEY_BACKSPACE, "KEY_BACKSPACE"},
    {KEY_TAB, "KEY_TAB"},
    {KEY_SPACE, "KEY_SPACE"},
    {KEY_MINUS, "KEY_MINUS"},
    {KEY_EQUAL, "KEY_EQUAL"},
    {KEY_LEFTBRACE, "KEY_LEFTBRACE"},
    {KEY_RIGHTBRACE, "KEY_RIGHTBRACE"},
    {KEY_BACKSLASH, "KEY_BACKSLASH"},
    {KEY_HASHTILDE, "KEY_HASHTILDE"},
    {KEY_SEMICOLON, "KEY_SEMICOLON"},
    {KEY_APOSTROPHE, "KEY_APOSTROPHE"},
    {KEY_GRAVE, "KEY_GRAVE"},
    {KEY_COMMA, "KEY_COMMA"},
    {KEY_DOT, "KEY_DOT"},
    {KEY_SLASH, "KEY_SLASH"},
    {KEY_CAPSLOCK, "KEY_CAPSLOCK"},
    {KEY_F1, "KEY_F1"},
    {KEY_F2, "KEY_F2"},
    {KEY_F3, "KEY_F3"},
    {KEY_F4, "KEY_F4"},
    {KEY_F5, "KEY_F5"},
    {KEY_F6, "KEY_F6"},
    {KEY_F7, "KEY_F7"},
    {KEY_F8, "KEY_F8"},
    {KEY_F9, "KEY_F9"},
    {KEY_F10, "KEY_F10"},
    {KEY_F11, "KEY_F11"},
    {KEY_F12, "KEY_F12"},
    {KEY_SYSRQ, "KEY_SYSRQ"},
    {KEY_SCROLLLOCK, "KEY_SCROLLLOCK"},
    {KEY_PAUSE, "KEY_PAUSE"},
    {KEY_INSERT, "KEY_INSERT"},
    {KEY_HOME, "KEY_HOME"},
    {KEY_PAGEUP, "KEY_PAGEUP"},
    {KEY_DELETE, "KEY_DELETE"},
    {KEY_END, "KEY_END"},
    {KEY_PAGEDOWN, "KEY_PAGEDOWN"},
    {KEY_RIGHT, "KEY_RIGHT"},
    {KEY_LEFT, "KEY_LEFT"},
    {KEY_DOWN, "KEY_DOWN"},
    {KEY_UP, "KEY_UP"},
    {KEY_NUMLOCK, "KEY_NUMLOCK"},
    {KEY_KPSLASH, "KEY_KPSLASH"},
    {KEY_KPASTERISK, "KEY_KPASTERISK"},
    {KEY_KPMINUS, "KEY_KPMINUS"},
    {KEY_KPPLUS, "KEY_KPPLUS"},
    {KEY_KPENTER, "KEY_KPENTER"},
    {KEY_KP1, "KEY_KP1"},
    {KEY_KP2, "KEY_KP2"},
    {KEY_KP3, "KEY_KP3"},
    {KEY_KP4, "KEY_KP4"},
    {KEY_KP5, "KEY_KP5"},
    {KEY_KP6, "KEY_KP6"},
    {KEY_KP7, "KEY_KP7"},
    {KEY_KP8, "KEY_KP8"},
    {KEY_KP9, "KEY_KP9"},
    {KEY_KP0, "KEY_KP0"},
    {KEY_KPDOT, "KEY_KPDOT"},
    {KEY_102ND, "KEY_102ND"},
    {KEY_COMPOSE, "KEY_COMPOSE"},
    {KEY_POWER, "KEY_POWER"},
    {KEY_KPEQUAL, "KEY_KPEQUAL"},
    {KEY_F13, "KEY_F13"},
    {KEY_F14, "KEY_F14"},
    {KEY_F15, "KEY_F15"},
    {KEY_F16, "KEY_F16"},
    {KEY_F17, "KEY_F17"},
    {KEY_F18, "KEY_F18"},
    {KEY_F19, "KEY_F19"},
    {KEY_F20, "KEY_F20"},
    {KEY_F21, "KEY_F21"},
    {KEY_F22, "KEY_F22"},
    {KEY_F23, "KEY_F23"},
    {KEY_F24, "KEY_F24"},
    {KEY_OPEN, "KEY_OPEN"},
    {KEY_HELP, "KEY_HELP"},
    {KEY_PROPS, "KEY_PROPS"},
    {KEY_FRONT, "KEY_FRONT"},
    {KEY_STOP, "KEY_STOP"},
    {KEY_AGAIN, "KEY_AGAIN"},
    {KEY_UNDO, "KEY_UNDO"},
    {KEY_CUT, "KEY_CUT"},
    {KEY_COPY, "KEY_COPY"},
    {KEY_PASTE, "KEY_PASTE"},
    {KEY_FIND, "KEY_FIND"},
    {KEY_MUTE, "KEY_MUTE"},
    {KEY_VOLUMEUP, "KEY_VOLUMEUP"},
    {KEY_VOLUMEDOWN, "KEY_VOLUMEDOWN"},
    {KEY_KPCOMMA, "KEY_KPCOMMA"},
    {KEY_RO, "KEY_RO"},
    {KEY_KATAKANAHIRAGANA, "KEY_KATAKANAHIRAGANA"},
    {KEY_YEN, "KEY_YEN"},
    {KEY_HENKAN, "KEY_HENKAN"},
    {KEY_MUHENKAN, "KEY_MUHENKAN"},
    {KEY_KPJPCOMMA, "KEY_KPJPCOMMA"},
    {KEY_HANGEUL, "KEY_HANGEUL"},
    {KEY_HANJA, "KEY_HANJA"},
    {KEY_KATAKANA, "KEY_KATAKANA"},
    {KEY_HIRAGANA, "KEY_HIRAGANA"},
    {KEY_ZENKAKUHANKAKU, "KEY_ZENKAKUHANKAKU"},
    {KEY_KPLEFTPAREN, "KEY_KPLEFTPAREN"},
    {KEY_KPRIGHTPAREN, "KEY_KPRIGHTPAREN"},
    {KEY_LEFTCTRL, "KEY_LEFTCTRL"},
    {KEY_LEFTSHIFT, "KEY_LEFTSHIFT"},
    {KEY_LEFTALT, "KEY_LEFTALT"},
    {KEY_LEFTMETA, "KEY_LEFTMETA"},
    {KEY_RIGHTCTRL, "KEY_RIGHTCTRL"},
    {KEY_RIGHTSHIFT, "KEY_RIGHTSHIFT"},
    {KEY_RIGHTALT, "KEY_RIGHTALT"},
    {KEY_RIGHTMETA, "KEY_RIGHTMETA"},
    {KEY_MEDIA_PLAYPAUSE, "KEY_MEDIA_PLAYPAUSE"},
    {KEY_MEDIA_STOPCD, "KEY_MEDIA_STOPCD"},
    {KEY_MEDIA_PREVIOUSSONG, "KEY_MEDIA_PREVIOUSSONG"},
    {KEY_MEDIA_NEXTSONG, "KEY_MEDIA_NEXTSONG"},
    {KEY_MEDIA_EJECTCD, "KEY_MEDIA_EJECTCD"},
    {KEY_MEDIA_VOLUMEUP, "KEY_MEDIA_VOLUMEUP"},
    {KEY_MEDIA_VOLUMEDOWN, "KEY_MEDIA_VOLUMEDOWN"},
    {KEY_MEDIA_MUTE, "KEY_MEDIA_MUTE"},
    {KEY_MEDIA_WWW, "KEY_MEDIA_WWW"},
    {KEY_MEDIA_BACK, "KEY_MEDIA_BACK"},
    {KEY_MEDIA_FORWARD, "KEY_MEDIA_FORWARD"},
    {KEY_MEDIA_STOP, "KEY_MEDIA_STOP"},
    {KEY_MEDIA_FIND, "KEY_MEDIA_FIND"},
    {KEY_MEDIA_SCROLLUP, "KEY_MEDIA_SCROLLUP"},
    {KEY_MEDIA_SCROLLDOWN, "KEY_MEDIA_SCROLLDOWN"},
    {KEY_MEDIA_EDIT, "KEY_MEDIA_EDIT"},
    {KEY_MEDIA_SLEEP, "KEY_MEDIA_SLEEP"},
    {KEY_MEDIA_COFFEE, "KEY_MEDIA_COFFEE"},
    {KEY_MEDIA_REFRESH, "KEY_MEDIA_REFRESH"},
    {KEY_MEDIA_CALC, "KEY_MEDIA_CALC"},

};

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
void removeEscapeCharacters(fptr in, fptr *out) {
  out->width = 0;
  bool flag = false;
  for (auto i = 0; i < in.width; i++) {
    if (!flag)
      switch (in.ptr[i]) {
        case '\\':
          flag = true;
          break;
        default:
          out->ptr[out->width++] = in.ptr[i];
          break;
      }
    else {
      flag = false;
      out->ptr[out->width++] = in.ptr[i];
    }
  }
}

static KeyItem kn_Match(fptr name) {
  for (unsigned int i = 0; i < countof(keyCodeMap); i++) {
    if (fp(keyCodeMap[i].name) == name)
      return KeyItem(keyCodeMap[i].code);
  }
  return KeyItem{};
}

#endif
