#include "Arduino.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
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
  return hash;
}

constexpr u8 SLAVE_ADDR = 0x42;
#define WIREA A5
#define WIREB A4
// #define SERIAL_SPEED 9600
// #define SERIAL_SPEED 74880
#define SERIAL_SPEED 115200

#define cSum_REDUNDANCY_AMMOUNT ((uint)1) // cant be 0
#include "my-lib/cSum.h"
static dataChecker dc = cSum_new(stdAlloc);

#define ISMAINHALF (1)
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
static long pstate[countof(sateTable)][countof(sateTable[0])]; // last state, negetive for high, positive for low
static bool lstate[countof(sateTable)][countof(sateTable[0])];
void update() {
  for (int i = 0; i < countof(sateTable); i++) {
    for (int j = 0; j < countof(sateTable[0]); j++) {
      auto now = micros();
      if (sateTable[i][j]) {
        if (pstate[i][j] > 0)
          pstate[i][j] = -now;
      } else {
        if (pstate[i][j] < 0)
          pstate[i][j] = now;
      }

      if (abs(pstate[i][j]) + 1000 < now)
        lstate[i][j] = pstate[i][j] < 0;
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
  Serial1.begin(SERIAL_SPEED, SERIAL_8N2, WIREA, WIREB);
  Serial1.setTimeout(5);
}

void remote_stateTable_update() {

  static mList(u8) message = mList_init(stdAlloc, u8);
  defer { Serial1.write('/0'); };
  if (!Serial1.available())
    return;
  {
    int marker;
    while ((u8)(marker = Serial1.read()) == 0x00)
      mList_push(message, (u8)Serial1.read());
  }

  /*if (mList_len(message) > 0)*/ {
    checkData cd = {.data = {mList_len(message), mList_arr(message)}};

    // fptr result = cSum_fromSum(cd);
    fptr result = cd.data;

    for (auto i = 0; i < countof(rowGpios); i++) {
      for (auto j = 0; j < countof(colGpios); j++) {
        sateTable[i][j + countof(colGpios)] = 0;
      }
    }

    for (int i = 0; i < result.width; i++) {
      u8 row = result.ptr[i] >> 4;
      u8 col = result.ptr[i] & 0xf;
      if (col < countof(colGpios) && row < countof(rowGpios))
        sateTable[row][col + countof(colGpios)] = 1;
    }
  }
  mList_clear(message);
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
  Serial1.write('/0');
}
usize counter = 0;
usize start = 0;
usize finish = 0;
  #include "command.hpp"
void loop() {
  counter++;
  if (!(counter % 1000)) [[unlikeley]] {
    start = finish;
    finish = millis();
    counter = 1;
  }
  if (ESP_IO::available()) [[unlikeley]] {
    usize startTime = millis();
    mList_scoped(u8) readlist = mList_init(stdAlloc, u8);
    do {
      if (ESP_IO::available())
        mList_push(readlist, ESP_IO::read());
    } while (
        mList_arr(readlist)[mList_len(readlist) - 1] != '\n' &&
        millis() < startTime + 5000
    );
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
    println_("1K_time:{}", finish - start);
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
  Serial1.begin(SERIAL_SPEED, SERIAL_8N2, WIREB, WIREA);
  Serial1.setTimeout(5);
}
void setup() {
  ESP_IO::begin(115200);
  wireSetup();
  for (int i = 0; i < countof(colGpios); i++)
    pinMode(colGpios[i], OUTPUT);
  for (int i = 0; i < countof(rowGpios); i++)
    pinMode(rowGpios[i], INPUT_PULLDOWN);
}
void loop() {
  statemessatge.width = 0;
  for (auto i = 0; i < countof(colGpios); i++) {
    digitalWrite(colGpios[i], HIGH);
    for (auto j = 0; j < countof(rowGpios); j++) {
      if (digitalRead(rowGpios[j]))
        statemessatge.ptr[statemessatge.width++] = (u8)j << 4 | (u8)i;
    }
    digitalWrite(colGpios[i], LOW);
  }
  checkData cd = {statemessatge};
  // checkData cd = cSum_toSum(dc, statemessatge);
  static mList(u8) ll = mList_init(stdAlloc, u8);
  for (auto i = 0; i < cd.data.width; i++) {
    mList_push(ll, 0x00);
    mList_push(ll, cd.data.ptr[i]);
  }
  mList_push(ll, 0xff);

  if (Serial1.available()) {
    while (Serial1.available())
      Serial1.read();
    Serial1.write(mList_arr(ll), mList_len(ll));
    println_("wrote {fptr<void>}", ((fptr){mList_len(ll), mList_arr(ll)}));
  }
  mList_clear(ll);
}
#endif
#include "my-lib/wheels.h"
