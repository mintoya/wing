#include "list.cpp"
#include <cstddef>

template <typename pinType, typename keyType> struct KeyMap {
  listPlus<pinType> RowPins = nullptr;
  listPlus<pinType> ColPins = nullptr;
  listPlus<keyType> keys = nullptr;
  char (*scan)(pinType row, pinType col) = nullptr;
  void (*sendKey)(keyType) = nullptr;

  void scanAll() {
    for (size_t i = 0; i < RowPins.length(); i++) {
      for (size_t j = 0; j < ColPins.length(); j++) {
        if (scan(RowPins.get(i), ColPins.get(j))) {
          sendKey(keys.get(i * RowPins.length() + j));
        }
      }
    }
  }
};
