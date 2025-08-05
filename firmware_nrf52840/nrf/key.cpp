#include <string.h>
// found in
// hardware/nRFMicro/nRFMicro-Arduino-Core/libraries/Adafruit_TinyUSB_Arduino/src/class/hid/hid.h:
//  #def ine HID_ASCII_TO_KEYCODE \
//     {0, 0                      }, /* 0x00 Null      */ \
//     {0, 0                     }, /* 0x01           */ \
//     {0, 0                     }, /* 0x02           */ \
//     {0, 0                     }, /* 0x03           */ \
//     {0, 0                     }, /* 0x04           */ \
//     {0, 0                     }, /* 0x05           */ \
//     {0, 0                     }, /* 0x06           */ \
//     {0, 0                     }, /* 0x07           */ \
//     {0, HID_KEY_BACKSPACE     }, /* 0x08 Backspace */ \
//     {0, HID_KEY_TAB           }, /* 0x09 Tab       */ \
//     {0, HID_KEY_ENTER         }, /* 0x0A Line Feed */ \
//     {0, 0                     }, /* 0x0B           */ \
//     {0, 0                     }, /* 0x0C           */ \
//     {0, HID_KEY_ENTER         }, /* 0x0D CR        */ \
//     {0, 0                     }, /* 0x0E           */ \
//     {0, 0                     }, /* 0x0F           */ \
//     {0, 0                     }, /* 0x10           */ \
//     {0, 0                     }, /* 0x11           */ \
//     {0, 0                     }, /* 0x12           */ \
//     {0, 0                     }, /* 0x13           */ \
//     {0, 0                     }, /* 0x14           */ \
//     {0, 0                     }, /* 0x15           */ \
//     {0, 0                     }, /* 0x16           */ \
//     {0, 0                     }, /* 0x17           */ \
//     {0, 0                     }, /* 0x18           */ \
//     {0, 0                     }, /* 0x19           */ \
//     {0, 0                     }, /* 0x1A           */ \
//     {0, HID_KEY_ESCAPE        }, /* 0x1B Escape    */ \
//     {0, 0                     }, /* 0x1C           */ \
//     {0, 0                     }, /* 0x1D           */ \
//     {0, 0                     }, /* 0x1E           */ \
//     {0, 0                     }, /* 0x1F           */ \
//                                                       \
//     {0, HID_KEY_SPACE         }, /* 0x20           */ \
//     {1, HID_KEY_1             }, /* 0x21 !         */ \
//     {1, HID_KEY_APOSTROPHE    }, /* 0x22 "         */ \
//     {1, HID_KEY_3             }, /* 0x23 #         */ \
//     {1, HID_KEY_4             }, /* 0x24 $         */ \
//     {1, HID_KEY_5             }, /* 0x25 %         */ \
//     {1, HID_KEY_7             }, /* 0x26 &         */ \
//     {0, HID_KEY_APOSTROPHE    }, /* 0x27 '         */ \
//     {1, HID_KEY_9             }, /* 0x28 (         */ \
//     {1, HID_KEY_0             }, /* 0x29 )         */ \
//     {1, HID_KEY_8             }, /* 0x2A *         */ \
//     {1, HID_KEY_EQUAL         }, /* 0x2B +         */ \
//     {0, HID_KEY_COMMA         }, /* 0x2C ,         */ \
//     {0, HID_KEY_MINUS         }, /* 0x2D -         */ \
//     {0, HID_KEY_PERIOD        }, /* 0x2E .         */ \
//     {0, HID_KEY_SLASH         }, /* 0x2F /         */ \
//     {0, HID_KEY_0             }, /* 0x30 0         */ \
//     {0, HID_KEY_1             }, /* 0x31 1         */ \
//     {0, HID_KEY_2             }, /* 0x32 2         */ \
//     {0, HID_KEY_3             }, /* 0x33 3         */ \
//     {0, HID_KEY_4             }, /* 0x34 4         */ \
//     {0, HID_KEY_5             }, /* 0x35 5         */ \
//     {0, HID_KEY_6             }, /* 0x36 6         */ \
//     {0, HID_KEY_7             }, /* 0x37 7         */ \
//     {0, HID_KEY_8             }, /* 0x38 8         */ \
//     {0, HID_KEY_9             }, /* 0x39 9         */ \
//     {1, HID_KEY_SEMICOLON     }, /* 0x3A :         */ \
//     {0, HID_KEY_SEMICOLON     }, /* 0x3B ;         */ \
//     {1, HID_KEY_COMMA         }, /* 0x3C <         */ \
//     {0, HID_KEY_EQUAL         }, /* 0x3D =         */ \
//     {1, HID_KEY_PERIOD        }, /* 0x3E >         */ \
//     {1, HID_KEY_SLASH         }, /* 0x3F ?         */ \
//                                                       \
//     {1, HID_KEY_2             }, /* 0x40 @         */ \
//     {1, HID_KEY_A             }, /* 0x41 A         */ \
//     {1, HID_KEY_B             }, /* 0x42 B         */ \
//     {1, HID_KEY_C             }, /* 0x43 C         */ \
//     {1, HID_KEY_D             }, /* 0x44 D         */ \
//     {1, HID_KEY_E             }, /* 0x45 E         */ \
//     {1, HID_KEY_F             }, /* 0x46 F         */ \
//     {1, HID_KEY_G             }, /* 0x47 G         */ \
//     {1, HID_KEY_H             }, /* 0x48 H         */ \
//     {1, HID_KEY_I             }, /* 0x49 I         */ \
//     {1, HID_KEY_J             }, /* 0x4A J         */ \
//     {1, HID_KEY_K             }, /* 0x4B K         */ \
//     {1, HID_KEY_L             }, /* 0x4C L         */ \
//     {1, HID_KEY_M             }, /* 0x4D M         */ \
//     {1, HID_KEY_N             }, /* 0x4E N         */ \
//     {1, HID_KEY_O             }, /* 0x4F O         */ \
//     {1, HID_KEY_P             }, /* 0x50 P         */ \
//     {1, HID_KEY_Q             }, /* 0x51 Q         */ \
//     {1, HID_KEY_R             }, /* 0x52 R         */ \
//     {1, HID_KEY_S             }, /* 0x53 S         */ \
//     {1, HID_KEY_T             }, /* 0x55 T         */ \
//     {1, HID_KEY_U             }, /* 0x55 U         */ \
//     {1, HID_KEY_V             }, /* 0x56 V         */ \
//     {1, HID_KEY_W             }, /* 0x57 W         */ \
//     {1, HID_KEY_X             }, /* 0x58 X         */ \
//     {1, HID_KEY_Y             }, /* 0x59 Y         */ \
//     {1, HID_KEY_Z             }, /* 0x5A Z         */ \
//     {0, HID_KEY_BRACKET_LEFT  }, /* 0x5B [         */ \
//     {0, HID_KEY_BACKSLASH     }, /* 0x5C '\'       */ \
//     {0, HID_KEY_BRACKET_RIGHT }, /* 0x5D ]         */ \
//     {1, HID_KEY_6             }, /* 0x5E ^         */ \
//     {1, HID_KEY_MINUS         }, /* 0x5F _         */ \
//                                                       \
//     {0, HID_KEY_GRAVE         }, /* 0x60 `         */ \
//     {0, HID_KEY_A             }, /* 0x61 a         */ \
//     {0, HID_KEY_B             }, /* 0x62 b         */ \
//     {0, HID_KEY_C             }, /* 0x63 c         */ \
//     {0, HID_KEY_D             }, /* 0x66 d         */ \
//     {0, HID_KEY_E             }, /* 0x65 e         */ \
//     {0, HID_KEY_F             }, /* 0x66 f         */ \
//     {0, HID_KEY_G             }, /* 0x67 g         */ \
//     {0, HID_KEY_H             }, /* 0x68 h         */ \
//     {0, HID_KEY_I             }, /* 0x69 i         */ \
//     {0, HID_KEY_J             }, /* 0x6A j         */ \
//     {0, HID_KEY_K             }, /* 0x6B k         */ \
//     {0, HID_KEY_L             }, /* 0x6C l         */ \
//     {0, HID_KEY_M             }, /* 0x6D m         */ \
//     {0, HID_KEY_N             }, /* 0x6E n         */ \
//     {0, HID_KEY_O             }, /* 0x6F o         */ \
//     {0, HID_KEY_P             }, /* 0x70 p         */ \
//     {0, HID_KEY_Q             }, /* 0x71 q         */ \
//     {0, HID_KEY_R             }, /* 0x72 r         */ \
//     {0, HID_KEY_S             }, /* 0x73 s         */ \
//     {0, HID_KEY_T             }, /* 0x75 t         */ \
//     {0, HID_KEY_U             }, /* 0x75 u         */ \
//     {0, HID_KEY_V             }, /* 0x76 v         */ \
//     {0, HID_KEY_W             }, /* 0x77 w         */ \
//     {0, HID_KEY_X             }, /* 0x78 x         */ \
//     {0, HID_KEY_Y             }, /* 0x79 y         */ \
//     {0, HID_KEY_Z             }, /* 0x7A z         */ \
//     {1, HID_KEY_BRACKET_LEFT  }, /* 0x7B {         */ \
//     {1, HID_KEY_BACKSLASH     }, /* 0x7C |         */ \
//     {1, HID_KEY_BRACKET_RIGHT }, /* 0x7D }         */ \
//     {1, HID_KEY_GRAVE         }, /* 0x7E ~         */ \
//     {0, HID_KEY_DELETE        }  /* 0x7F Delete    */ \
//
//  #def ine HID_KEYCODE_TO_ASCII    \
 //     {0     , 0      }, /* 0x00 */ \
//     {0     , 0      }, /* 0x01 */ \
//     {0     , 0      }, /* 0x02 */ \
//     {0     , 0      }, /* 0x03 */ \
//     {'a'   , 'A'    }, /* 0x04 */ \
//     {'b'   , 'B'    }, /* 0x05 */ \
//     {'c'   , 'C'    }, /* 0x06 */ \
//     {'d'   , 'D'    }, /* 0x07 */ \
//     {'e'   , 'E'    }, /* 0x08 */ \
//     {'f'   , 'F'    }, /* 0x09 */ \
//     {'g'   , 'G'    }, /* 0x0a */ \
//     {'h'   , 'H'    }, /* 0x0b */ \
//     {'i'   , 'I'    }, /* 0x0c */ \
//     {'j'   , 'J'    }, /* 0x0d */ \
//     {'k'   , 'K'    }, /* 0x0e */ \
//     {'l'   , 'L'    }, /* 0x0f */ \
//     {'m'   , 'M'    }, /* 0x10 */ \
//     {'n'   , 'N'    }, /* 0x11 */ \
//     {'o'   , 'O'    }, /* 0x12 */ \
//     {'p'   , 'P'    }, /* 0x13 */ \
//     {'q'   , 'Q'    }, /* 0x14 */ \
//     {'r'   , 'R'    }, /* 0x15 */ \
//     {'s'   , 'S'    }, /* 0x16 */ \
//     {'t'   , 'T'    }, /* 0x17 */ \
//     {'u'   , 'U'    }, /* 0x18 */ \
//     {'v'   , 'V'    }, /* 0x19 */ \
//     {'w'   , 'W'    }, /* 0x1a */ \
//     {'x'   , 'X'    }, /* 0x1b */ \
//     {'y'   , 'Y'    }, /* 0x1c */ \
//     {'z'   , 'Z'    }, /* 0x1d */ \
//     {'1'   , '!'    }, /* 0x1e */ \
//     {'2'   , '@'    }, /* 0x1f */ \
//     {'3'   , '#'    }, /* 0x20 */ \
//     {'4'   , '$'    }, /* 0x21 */ \
//     {'5'   , '%'    }, /* 0x22 */ \
//     {'6'   , '^'    }, /* 0x23 */ \
//     {'7'   , '&'    }, /* 0x24 */ \
//     {'8'   , '*'    }, /* 0x25 */ \
//     {'9'   , '('    }, /* 0x26 */ \
//     {'0'   , ')'    }, /* 0x27 */ \
//     {'\r'  , '\r'   }, /* 0x28 */ \
//     {'\x1b', '\x1b' }, /* 0x29 */ \
//     {'\b'  , '\b'   }, /* 0x2a */ \
//     {'\t'  , '\t'   }, /* 0x2b */ \
//     {' '   , ' '    }, /* 0x2c */ \
//     {'-'   , '_'    }, /* 0x2d */ \
//     {'='   , '+'    }, /* 0x2e */ \
//     {'['   , '{'    }, /* 0x2f */ \
//     {']'   , '}'    }, /* 0x30 */ \
//     {'\\'  , '|'    }, /* 0x31 */ \
//     {'#'   , '~'    }, /* 0x32 */ \
//     {';'   , ':'    }, /* 0x33 */ \
//     {'\''  , '\"'   }, /* 0x34 */ \
//     {'`'   , '~'    }, /* 0x35 */ \
//     {','   , '<'    }, /* 0x36 */ \
//     {'.'   , '>'    }, /* 0x37 */ \
//     {'/'   , '?'    }, /* 0x38 */ \
//                                   \
//     {0     , 0      }, /* 0x39 */ \
//     {0     , 0      }, /* 0x3a */ \
//     {0     , 0      }, /* 0x3b */ \
//     {0     , 0      }, /* 0x3c */ \
//     {0     , 0      }, /* 0x3d */ \
//     {0     , 0      }, /* 0x3e */ \
//     {0     , 0      }, /* 0x3f */ \
//     {0     , 0      }, /* 0x40 */ \
//     {0     , 0      }, /* 0x41 */ \
//     {0     , 0      }, /* 0x42 */ \
//     {0     , 0      }, /* 0x43 */ \
//     {0     , 0      }, /* 0x44 */ \
//     {0     , 0      }, /* 0x45 */ \
//     {0     , 0      }, /* 0x46 */ \
//     {0     , 0      }, /* 0x47 */ \
//     {0     , 0      }, /* 0x48 */ \
//     {0     , 0      }, /* 0x49 */ \
//     {0     , 0      }, /* 0x4a */ \
//     {0     , 0      }, /* 0x4b */ \
//     {0     , 0      }, /* 0x4c */ \
//     {0     , 0      }, /* 0x4d */ \
//     {0     , 0      }, /* 0x4e */ \
//     {0     , 0      }, /* 0x4f */ \
//     {0     , 0      }, /* 0x50 */ \
//     {0     , 0      }, /* 0x51 */ \
//     {0     , 0      }, /* 0x52 */ \
//     {0     , 0      }, /* 0x53 */ \
//                                   \
//     {'/'   , '/'    }, /* 0x54 */ \
//     {'*'   , '*'    }, /* 0x55 */ \
//     {'-'   , '-'    }, /* 0x56 */ \
//     {'+'   , '+'    }, /* 0x57 */ \
//     {'\r'  , '\r'   }, /* 0x58 */ \
//     {'1'   , 0      }, /* 0x59 */ \
//     {'2'   , 0      }, /* 0x5a */ \
//     {'3'   , 0      }, /* 0x5b */ \
//     {'4'   , 0      }, /* 0x5c */ \
//     {'5'   , '5'    }, /* 0x5d */ \
//     {'6'   , 0      }, /* 0x5e */ \
//     {'7'   , 0      }, /* 0x5f */ \
//     {'8'   , 0      }, /* 0x60 */ \
//     {'9'   , 0      }, /* 0x61 */ \
//     {'0'   , 0      }, /* 0x62 */ \
//     {'.'   , 0      }, /* 0x63 */ \
//     {0     , 0      }, /* 0x64 */ \
//     {0     , 0      }, /* 0x65 */ \
//     {0     , 0      }, /* 0x66 */ \
//     {'='   , '='    }, /* 0x67 */ \
//

#include "my-list.hpp"

#include <stdint.h>
#include <stdlib.h>

// clang-format off


// stores type of data and data
struct KeyItem {
  uint8_t character;//not really a character 
  enum : char { 
           PASSTHROUGH = 0,
           CHARACTER = 1, 
           LAYER = 2,
           TAPDANCE = 4 
         } type;
  inline KeyItem(uint8_t c){character = c;type = CHARACTER;}
  inline KeyItem(void){character = 0,type = PASSTHROUGH;}
};
// clang-format on

// KeyItem matrix[][]  = {{0x41}};
constexpr uint8_t colPins[6] = {1, 2, 3, 4, 5, 6};
constexpr uint8_t rowPins[3] = {1, 2, 3};

listPlus<uint8_t> colPinsList((uint8_t *)colPins, 6);
listPlus<uint8_t> rowPinsList((uint8_t *)rowPins, 3);

enum direction : uint8_t { NONE = 0, LOWTOHIGH = 1, HIGHTOLOW = 2 };
struct activeBuffer {
  size_t _rows;
  size_t _cols;
  bool *a;
  bool *b;
  uint8_t *_change;
  bool active = 0;
  void inline flip() {
    active = !active;
    memset(getActive(), 0, _rows * _cols * sizeof(bool));
  }
  inline activeBuffer(unsigned int rows, unsigned int cols) {
    _rows = rows;
    _cols = cols;

    _change = (uint8_t *)calloc(rows * cols, sizeof(bool));
    a = (bool *)calloc(rows * cols, sizeof(bool));
    b = (bool *)calloc(rows * cols, sizeof(bool));
  }
  // treat as 2d array
  void set(unsigned int row, unsigned int col, bool value) {
    getActive()[row * _cols + col] = value;
  }
  inline bool *getActive() { return active ? a : b; }
  inline bool *getInActive() { return !active ? a : b; }
  uint8_t *getDifference() {
    bool *currentActive = getActive();
    bool *currentNotActive = getInActive();

    for (unsigned int i = 0; i < _rows * _cols; i++) {
      if (currentActive[i] && !currentNotActive[i]) {
        _change[i] = direction::HIGHTOLOW;
      } else if (!currentActive[i] && currentNotActive[i]) {
        _change[i] = direction::LOWTOHIGH;
      } else {
        _change[i] = direction::NONE;
      }
    }

    return _change;
  }
};
