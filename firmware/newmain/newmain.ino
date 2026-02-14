
#define NOFILEPRINTER (1)
#define noAssertMessage (1)
#include "my-lib/print.h"
#undef print
#undef println
#undef print_
#undef println_
#include <USBCDC.h> // Serial
// override print behavior
// clang-format off
void(serialOutputFunction)(
    const c32 *c,
    void *_,
    uint len,
    bool flush
) {
  for (u32 i = 0; i < len; i++)
    Serial.write((char)(c[i]));
  if (flush)
    Serial.flush();
}
#define print_(fmt, ...) print_wfO(serialOutputFunction, 0, fmt, __VA_ARGS__)
#define println_(fmt, ...) print_(fmt "\n", __VA_ARGS__);
// clang-format on
//
// #define noAssertMessage
#include "Arduino.h"
#include "esp32-hal-gpio.h"
// #include "esp32-hal.h"

#include "key.hpp"
#include "my-lib/printers/vason.c"
#include "my-lib/stringList.h"
#include "my-lib/types.h"

//
//
//
#include <Wire.h>
#define cSum_REDUNDANCY_AMMOUNT ((unsigned int)2)
#include "my-lib/cSum.h"
dataChecker dc = cSum_new(stdAlloc);

constexpr u8 SLAVE_ADDR = 0x42;
constexpr usize WIRE_CLOCK = 400000;

// #define ISMAINHALF (1)
#if defined(ISMAINHALF) // main side
  #include "fileSystemInterface.hpp"
  #include "key.hpp"
  #include "layout.hpp"
  #include <USB.h>
  #include <USBHID.h>
  #include <USBHIDKeyboard.h>
//
//
//
//
static const uint rowGpios[] = {2, 3, 4, 5};
static const uint colGpios[] = {10, 11, 9, 8, 7, 6};
const uint nrowGpios = countof(rowGpios);
const uint ncolGpios = countof(colGpios);
static bool sateTable[countof(rowGpios)][countof(colGpios) * 2] = {};
//
// keyboard report variables
USBHIDKeyboard Keyboard;
reportManager rm;

//
// keyboard configuration variables
//
void (*keyboardFunctions[10])(void) = {};
mList(mList(KeyItem)) keyMapLayers = mList_init(
    stdAlloc,
    mList(KeyItem)
);
mList(tapDance) tapDances = mList_init(
    stdAlloc,
    tapDance
);
//
// report senders
//
static bool fakeSenderEnabled = true;
void fakeSender(u8 mod, u8 *keys) {
  if (fakeSenderEnabled)
    println_(
        "{x}|{x} {x} {x} {x} {x} {x}",
        mod,
        keys[0], keys[1], keys[2],
        keys[3], keys[4], keys[5]
    );
}
void sendHidReport(u8 modifiers, uint8_t *key_codes) {
  // typedef struct
  // {
  //   u8 modifiers;
  //   u8 reserved;
  //   u8 keys[6];
  // } KeyReport;
  static KeyReport k;
  k.modifiers = modifiers;
  k.reserved = 0;
  k.keys[0] = key_codes[0];
  k.keys[1] = key_codes[1];
  k.keys[2] = key_codes[2];
  k.keys[3] = key_codes[3];
  k.keys[4] = key_codes[4];
  k.keys[5] = key_codes[5];

  Keyboard.sendReport(&k);
}
//
// communication
//
void wireSetup() {
  Wire.begin();
  Wire.setTimeout(500);
  Wire.setClock(WIRE_CLOCK);
}
void remote_stateTable_update() {
  usize readSize = Wire.requestFrom(SLAVE_ADDR, (usize)255, true);
  u8 readBuffer[sizeof(sateTable) * 2];
  usize readBufLen;
  // known bug?
  // requestFrom just returns the second parameter
  checkData cd;
  fptr result;
  u64_vl_max maxB = {};
  vlength *ptr = maxB._;
  vlength v_curr = {};
  if (!Wire.available())
    goto skip;
  while ((v_curr = bitcast(vlength, Wire.read())).hasNext)
    *ptr++ = v_curr;
  *ptr++ = v_curr;
  readSize = vlen_toU64(maxB._);
  readBufLen = Wire.readBytes(readBuffer, readSize);
  if (readBufLen != readSize)
    goto skip;
  cd = (checkData){
      .data = (fptr){readBufLen, readBuffer},
  };
  result = cSum_fromSum(cd);
  if (!result.ptr)
    goto skip;
  for (int i = 0; i < countof(rowGpios); i++) {
    for (int j = 0; j < countof(colGpios); j++) {
      sateTable[i][j + countof(colGpios)] = 0;
    }
  }
  for (int i = 0; i < result.width; i += 2) {
    u8 col = result.ptr[i];
    u8 row = result.ptr[i + 1];
    sateTable[row][col + countof(colGpios)] = 1;
  }

skip:;
}
void local_stateTable_update() {
  for (auto i = 0; i < countof(colGpios); i++) {
    digitalWrite(colGpios[i], HIGH);
    for (auto j = 0; j < countof(rowGpios); j++)
      sateTable[i][j] = digitalRead(rowGpios[j]);
    digitalWrite(colGpios[i], LOW);
  }
}
void setup() {
  Serial.begin(115200);
  wireSetup();
  for (int i = 0; i < countof(colGpios); i++)
    pinMode(colGpios[i], OUTPUT);
  for (int i = 0; i < countof(rowGpios); i++)
    pinMode(rowGpios[i], INPUT_PULLDOWN);
  println_("keyboard function begin");
  Keyboard.begin();
  println_("file-system begin");
  FSISetup();
  println_("layout parsing begin");
  parseLayout();
  println_("printing");
  prettyPrintLayers();
  println_("reportmanager begin");
  rm = reportManager(sendHidReport, fakeSender);
  println_("loop begin");
}
void loop() {
  local_stateTable_update();
  remote_stateTable_update();
  keyMap::pressKeys<countof(rowGpios), countof(colGpios)>(sateTable, rm); // !!will modify stateTable
}
//
//
//
//
//
#else // peripheral side
//
//
//
//
static const uint rowGpios[] = {2, 3, 4, 5};
static const uint colGpios[] = {10, 11, 9, 8, 7, 6};
volatile static bool sateTable[2][countof(rowGpios)][countof(colGpios)] = {};
  #include <atomic>
volatile static std::atomic<bool> flipper(false);
//
//
//
//
// void onRequest() {
//   updatePins();
//   checkData cd =
//       cSum_toSum(dc, (fptr){.ptr = pinData, .width = sizeof(pinData)});
//   Wire.write(cd.data.width);
//   Wire.write((u8 *)cd.data.ptr, cd.data.width);
//   for (int i = 0; i < sizeof(pinData); i++)
//     printf_("{x}", pinData[i]);
//   println_();
// }
static u8 message[countof(rowGpios) * countof(colGpios) * 2];
void wireSetup() {
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest([]() {
    usize messagelen = 0;
    for (auto i = 0; i < countof(colGpios); i++)
      for (auto j = 0; j < countof(rowGpios); j++)
        if (sateTable[!flipper][j][i]) {
          message[messagelen++] = i;
          message[messagelen++] = j;
        }

    checkData cd = cSum_toSum(dc, (fptr){messagelen, message});
    auto vl_struct = u64_toVlen(cd.data.width);
    auto vl_ptr = vl_struct._;
    while (
        bitcast(u8, *vl_ptr) ==
        bitcast(u8, ((vlength){.hasNext = true, .data = 0}))
    )
      vl_ptr++;
    do
      Wire.write(bitcast(u8, *vl_ptr));
    while (vl_ptr++->hasNext);
    // Wire.write((uint)cd.data.width);
    Wire.write((u8 *)cd.data.ptr, cd.data.width);
  });
  Wire.setClock(WIRE_CLOCK);
}
void setup() {
  Serial.begin(115200);
  wireSetup();
  for (int i = 0; i < countof(colGpios); i++)
    pinMode(colGpios[i], OUTPUT);
  for (int i = 0; i < countof(rowGpios); i++)
    pinMode(rowGpios[i], INPUT_PULLDOWN);
}
void loop() {
  for (auto i = 0; i < countof(colGpios); i++) {
    digitalWrite(colGpios[i], HIGH);
    for (auto j = 0; j < countof(rowGpios); j++)
      sateTable[flipper][j][i] = digitalRead(rowGpios[j]);
    digitalWrite(colGpios[i], LOW);
  }
  flipper = !flipper;

  static auto last = millis();
  if (millis() > last + 10) [[unlikely]] {
    last = millis();
    for (auto i = 0; i < countof(colGpios); i++) {
      for (auto j = 0; j < countof(rowGpios); j++)
        print_("{c8}", sateTable[flipper][j][i] ? 'X' : ' ');
      print_("|");
    }
    println_();
  }
}
#endif
#include "my-lib/wheels.h"
