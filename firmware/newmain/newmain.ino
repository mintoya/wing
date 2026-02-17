#include "Arduino.h"
#include "esp32-hal-gpio.h"
#include "esp_io.hpp"
#define NOFILEPRINTER (1)
#define noAssertMessage (1)

#include "my-lib/print.h"
#undef print
#undef println
#undef print_
#undef println_
#define print_(fmt, ...) print_wfO(serialOutputFunction, 0, fmt, __VA_ARGS__)
#define println_(fmt, ...) print_(fmt "\r\n", __VA_ARGS__);
// #include "esp_io.hpp"

void(serialOutputFunction)(
    const c32 *c,
    void *_,
    uint len,
    bool flush
) {
  static u8 chars[512];
  static usize place = 0;

  const c32 *p = c;
  const c32 *end = c + len;

  while (p < end) {

    // fill buffer safely
    while (place < countof(chars) && p < end) {
      chars[place++] = (u8)(*p++); // truncates to byte
    }

    // flush full buffer
    if (place == countof(chars)) {
      ESP_IO::write(chars, place);
      place = 0;
    }
  }

  if (flush && place > 0) {
    ESP_IO::write(chars, place);
    ESP_IO::flush();
    place = 0;
  }
}
#include <USB.h>
#include <USBHID.h>
#include <USBHIDKeyboard.h>
// #include <Wire.h>

#include "key.hpp"
#include "my-lib/mytypes.h"

static inline umax minihash(const fptr str) {
  umax hash = 5381;
  for (usize i = 0; i < str.width; i++) {
    hash ^= hash >> 3;
    hash = hash * 65;
    hash ^= (str.ptr[i]);
  }
}

constexpr u8 SLAVE_ADDR = 0x42;
#define WIREA A5
#define WIREB A5
#define SERIAL_SPEED 9600
// #define SERIAL_SPEED  74880
// #define SERIAL_SPEED 115200

// #define ISMAINHALF (1)
#if defined(ISMAINHALF) // main side
  #include "fileSystemInterface.hpp"
  // #include "key.hpp"
  #include "layout.hpp"
//
//
//
//
static const uint rowGpios[] = {2, 3, 4, 5};
static const uint colGpios[] = {6, 7, 8, 9, 11, 10};
const uint nrowGpios = countof(rowGpios);
const uint ncolGpios = countof(colGpios);
static bool sateTable[countof(rowGpios)][countof(colGpios) * 2] = {};
namespace bounceTable {
static u8 pstate[countof(sateTable)][countof(sateTable[0])];
static bool lstate[countof(sateTable)][countof(sateTable[0])];
void update() {
  for (int i = 0; i < countof(sateTable); i++) {
    for (int j = 0; j < countof(sateTable[0]); j++) {
      pstate[i][j] <<= 1;
      pstate[i][j] |= !!sateTable[i][j];
      if (pstate[i][j] == 0xff || pstate[i][j] == 0x00)
        lstate[i][j] = !!pstate[i][j];
    }
  }
}
}; // namespace bounceTable
//
// keyboard report variables
USBHIDKeyboard Keyboard;
reportManager rm;

//
// keyboard configuration variables
//
// void (*keyboardFunctions[10])(void) = {};
slice(slice(KeyItem)) keyMapLayers{};
slice(tapDance) tapDances = {};

//
// report senders
//
volatile static bool fakeSenderEnabled = true;
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
  Serial1.begin(SERIAL_SPEED, SERIAL_8N1, WIREA, WIREB);
  Serial1.setTimeout(2);
}

void remote_stateTable_update() {
  while (Serial1.read() != 0xff)
    Serial1.read();

  int keyCount = Serial1.read(); // Read Number of Keys
  fptr message = {(usize)keyCount, aCreate(stdAlloc, u8, keyCount)};
  defer_(aFree(stdAlloc, message.ptr));

  Serial1.read(message.ptr, message.width);
  usize hash = minihash(message);
  usize inhash;
  Serial1.read((u8 *)&inhash, sizeof(inhash));
  if (inhash != hash)
    return;

  for (int i = 0; i < message.width; i += 2) {
    u8 col = message.ptr[i];
    u8 row = message.ptr[i + 1];

    if (col < countof(colGpios) && row < countof(rowGpios))
      sateTable[row][col + countof(colGpios)] = 1;
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
  ESP_IO::begin(115200);
  wireSetup();
  for (int i = 0; i < countof(colGpios); i++)
    pinMode(colGpios[i], OUTPUT);
  for (int i = 0; i < countof(rowGpios); i++)
    pinMode(rowGpios[i], INPUT_PULLDOWN);

  USB.begin();
  Keyboard.begin();
  FSISetup();
  println_("layout parsing begin");
  parseLayout();
  println_("reportmanager begin");
  rm = reportManager(sendHidReport, fakeSender);
  // rm = reportManager(nullptr, fakeSender);
  println_("loop begin");
}
usize counter = 0;
usize start = 0;
usize finish = 0;
  #include "command.hpp"
void loop() {
  counter++;
  if (!(counter % 10000)) [[unlikeley]] {
    start = finish;
    finish = millis();
    counter = 1;
  }
  if (ESP_IO::available()) [[unlikeley]] {
    usize startTime = millis();
    mList_scoped(u8) readlist = mList_init(stdAlloc, u8);
    {
    readmore:
      while (ESP_IO::available())
        mList_push(readlist, ESP_IO::read());
    }
    if (
        *mList_get(readlist, mList_len(readlist) - 1) != '\n' &&
        millis() < startTime + 5000
    )
      goto readmore;
    fptr input = mList_slice(readlist);
    println_("{}", input);
    commands::execute(input);
  }
  local_stateTable_update();
  remote_stateTable_update();

  bounceTable::update();
  keyMap::pressKeys<countof(rowGpios), countof(colGpios)>(bounceTable::lstate, rm); // !!will modify stateTable
  rm.send();
  static auto last = millis();
  if (millis() > last + 1000) [[unlikely]] {
    last = millis();
    for (auto i = 0; i < countof(colGpios) * 2; i++) {
      for (auto j = 0; j < countof(rowGpios); j++)
        print_("{c8}", bounceTable::lstate[j][i] ? 'X' : ' ');
      print_("|");
    }
    println_();
    println_("10K_time:{}", finish - start);
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

static u8 messagesBuf[countof(rowGpios) * countof(colGpios) * 2];
static fptr statemessatge = {.ptr = messagesBuf};
//
//
//
//
void wireSetup() {
  Serial1.begin(SERIAL_SPEED, SERIAL_8N1, WIREB, WIREA);
  Serial1.setTimeout(2);
}
void setup() {
  Serial.begin(115200);
  wireSetup();
  for (int i = 0; i < countof(colGpios); i++)
    pinMode(colGpios[i], OUTPUT);
  for (int i = 0; i < countof(rowGpios); i++)
    pinMode(rowGpios[i], INPUT_PULLDOWN);
}
// void remote_stateTable_update() {
//   while (Serial1.read() != 0xff)
//     Serial1.read();
//
//   int keyCount = Serial1.read(); // Read Number of Keys
//   fptr message = {(usize)keyCount, aCreate(stdAlloc, u8, keyCount)};
//   defer_(aFree(stdAlloc, message.ptr));
//
//   Serial1.read(message.ptr, message.width);
//   usize hash = minihash(message);
//   usize inhash;
//   Serial1.read((u8 *)&inhash, sizeof(inhash));
//   if (inhash != hash)
//     return;
//
//   for (int i = 0; i < message.width; i += 2) {
//     u8 col = message.ptr[i];
//     u8 row = message.ptr[i + 1];
//
//     if (col < countof(colGpios) && row < countof(rowGpios))
//       sateTable[row][col + countof(colGpios)] = 1;
//   }
// }
void loop() {
  statemessatge.width = 0;
  for (auto i = 0; i < countof(colGpios); i++) {
    digitalWrite(colGpios[i], HIGH);
    for (auto j = 0; j < countof(rowGpios); j++) {
      if (digitalRead(rowGpios[j])) {
        statemessatge.ptr[statemessatge.width++] = (u8)i;
        statemessatge.ptr[statemessatge.width++] = (u8)j;
      }
    }
    digitalWrite(colGpios[i], LOW);
  }
  Serial1.write(0xFF);
  Serial1.write((uint)statemessatge.width);
  Serial1.write(statemessatge.ptr, statemessatge.width);
  usize hash = minihash(statemessatge);
  Serial1.write((u8 *)&hash, sizeof(hash));

  static auto last = millis();
  if (millis() > last + 10) [[unlikely]] {
    last = millis();
    for (auto i = 0; i < countof(colGpios); i++) {
      digitalWrite(colGpios[i], HIGH);
      for (auto j = 0; j < countof(rowGpios); j++) {
        if (digitalRead(rowGpios[j])) {
          print_("{c8}", digitalRead(rowGpios[j]) ? 'X' : ' ');
        }
        print_("|");
      }
      println_();
    }
  }
}
#endif
#include "my-lib/wheels.h"
