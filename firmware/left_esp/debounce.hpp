#pragma once
#include <stdint.h>

#define dbounceT ((uint8_t)0b11)
struct dbool {
  bool tureState = 0;
  uint8_t states = 0;
  dbool(){}
  bool get() { return tureState; }
  
  void set(bool b) {
    states = (states << 1 | b);
    // clang-format off
    tureState = 
      ((states & dbounceT) == dbounceT || (states & dbounceT) == 0)
      ? (( states & dbounceT )? 1 : 0 )
      : (tureState);
    // clang-format on
  }
};
