#pragma once
#include <stdint.h>

#define dbounceT ((uint8_t)0b11)
struct dbool {
  bool lstate = 0;
  uint8_t states = 0;
  dbool() {}
  bool get() { return lstate; }

  void set(bool b) {
    states = (states << 1 | b);
    // clang-format off
    lstate = 
      ((states & dbounceT) == dbounceT || (states & dbounceT) == 0)
      ? (( states & dbounceT )? 1 : 0 )
      : (lstate);
    // clang-format on
  }
};
