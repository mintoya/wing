
#include "my-list.hpp"
#include <cstdint>
#include <stdint.h>

// clang-format off


// stores type of data and data
struct KeyItem {
  char character;
  enum : char { 
           CHARACTER = 0, 
           LAYER = 1,
           TAPDANCE = 2 
         } type;
};
// clang-format on

constexpr uint8_t colPins[6] = {1, 2, 3, 4, 5, 6};
constexpr uint8_t rowPins[3] = {1, 2, 3};
constexpr uint8_t colSize = sizeof(colPins) / sizeof(uint8_t);
constexpr uint8_t rowSize = sizeof(rowPins) / sizeof(uint8_t);

// clang-format off
KeyItem matix
  [sizeof(rowPins) / sizeof(uint8_t)]
  [sizeof(colPins) / sizeof(uint8_t)] = 
{

};
// clang-format on
