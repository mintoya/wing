
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

#include "key.hpp"
#include "my-lib/mytypes.h"
// #include "my-lib/printers/vason.c"

//
//
//
#include <Wire.h>

constexpr u8 SLAVE_ADDR = 0x42;
constexpr usize WIRE_CLOCK = 400000;

#define ISMAINHALF (1)
#if defined(ISMAINHALF) // main side
  #include "fileSystemInterface.hpp"
  // #include "key.hpp"
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
// reportManager rm;

//
// keyboard configuration variables
//
// void (*keyboardFunctions[10])(void) = {};
slice(slice(KeyItem)) keyMapLayers{};
slice(tapDance) tapDances = {};

//
// report senders
//
// static bool fakeSenderEnabled = true;
// void fakeSender(u8 mod, u8 *keys) {
//   if (fakeSenderEnabled)
//     println_(
//         "{x}|{x} {x} {x} {x} {x} {x}",
//         mod,
//         keys[0], keys[1], keys[2],
//         keys[3], keys[4], keys[5]
//     );
// }
// void sendHidReport(u8 modifiers, uint8_t *key_codes) {
//   static KeyReport k;
//   k.modifiers = modifiers;
//   k.reserved = 0;
//   k.keys[0] = key_codes[0];
//   k.keys[1] = key_codes[1];
//   k.keys[2] = key_codes[2];
//   k.keys[3] = key_codes[3];
//   k.keys[4] = key_codes[4];
//   k.keys[5] = key_codes[5];
//
//   Keyboard.sendReport(&k);
// }

//
// communication
//
void wireSetup() {
  Wire.begin();
  Wire.setTimeout(500);
  Wire.setClock(WIRE_CLOCK);
}
REGISTER_SPECIAL_PRINTER("reset_reason", int, {
  switch (in) {
    case 1:
      PUTS(U"Vbat power on reset");
      break;
    case 3:
      PUTS(U"Software reset digital core");
      break;
    case 4:
      PUTS(U"Legacy watch dog reset digital core");
      break;
    case 5:
      PUTS(U"Deep Sleep reset digital core");
      break;
    case 6:
      PUTS(U"Reset by SLC module, reset digital core");
      break;
    case 7:
      PUTS(U"Timer Group0 Watch dog reset digital core");
      break;
    case 8:
      PUTS(U"Timer Group1 Watch dog reset digital core");
      break;
    case 9:
      PUTS(U"RTC Watch dog Reset digital core");
      break;
    case 10:
      PUTS(U"Instrusion tested to reset CPU");
      break;
    case 11:
      PUTS(U"Time Group reset CPU");
      break;
    case 12:
      PUTS(U"Software reset CPU");
      break;
    case 13:
      PUTS(U"RTC Watch dog Reset CPU");
      break;
    case 14:
      PUTS(U"for APP CPU, reseted by PRO CPU");
      break;
    case 15:
      PUTS(U"Reset when the vdd voltage is not stable");
      break;
    case 16:
      PUTS(U"RTC Watch dog reset digital core and rtc module");
      break;
    default:
      PUTS(U"NO_MEAN");
  }
});
void remote_stateTable_update() {
  usize requestSize = 32;
  usize receivedBytes = Wire.requestFrom(SLAVE_ADDR, requestSize, true);

  if (receivedBytes == 0 || !Wire.available())
    return;

  int dataLen = Wire.read();

  if (dataLen > receivedBytes - 1)
    dataLen = receivedBytes - 1;

  for (int i = 0; i < countof(rowGpios); i++) {
    for (int j = 0; j < countof(colGpios); j++) {
      sateTable[i][j + countof(colGpios)] = 0;
    }
  }

  for (int i = 0; i < dataLen; i += 2) {
    if (Wire.available() < 2)
      break;

    u8 col = Wire.read();
    u8 row = Wire.read();

    // Bounds check to prevent crashes
    if (col < countof(colGpios) && row < countof(rowGpios)) {
      sateTable[row][col + countof(colGpios)] = 1;
    }
  }
}
void local_stateTable_update() {
  for (auto i = 0; i < countof(colGpios); i++) {
    digitalWrite(colGpios[i], HIGH);
    for (auto j = 0; j < countof(rowGpios); j++)
      sateTable[j][i] = digitalRead(rowGpios[j]);
    digitalWrite(colGpios[i], LOW);
  }
}
void setup() {
  Serial.begin(115200);
  delay(1000);
  wireSetup();
  // delay(1000);
  println_(
      "reset reason : {reset_reason}",
      esp_rom_get_reset_reason(0)
  );
  for (int i = 0; i < countof(colGpios); i++)
    pinMode(colGpios[i], OUTPUT);
  for (int i = 0; i < countof(rowGpios); i++)
    pinMode(rowGpios[i], INPUT_PULLDOWN);
  delay(1000);
  println_("keyboard function begin");
  delay(1000);
  Keyboard.begin();
  println_("file-system begin");
  delay(1000);
  FSISetup();
  println_("layout parsing begin");
  for(int i = 0;i<100;i++)print_("skd");
  delay(1000);
  parseLayout();
  // println_("printing");
  // delay(1000);
  // prettyPrintLayers();
  println_("reportmanager begin");
  delay(1000);
  // rm = reportManager(sendHidReport, fakeSender);
  println_("loop begin");
  delay(1000);
}
void loop() {
  local_stateTable_update();
  // println_("remote update");
  remote_stateTable_update();
  // keyMap::pressKeys<countof(rowGpios), countof(colGpios)>(sateTable, rm); // !!will modify stateTable
  static auto last = millis();
  if (millis() > last + 10) [[unlikely]] {
    last = millis();
    for (auto i = 0; i < countof(colGpios) * 2; i++) {
      for (auto j = 0; j < countof(rowGpios); j++)
        print_("{c8}", sateTable[j][i] ? 'X' : ' ');
      print_("|");
    }
    println_();
  }
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

    Wire.write((uint)messagelen);
    Wire.write((u8 *)message, messagelen);
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
