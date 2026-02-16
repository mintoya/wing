#include "Arduino.h"
#include "esp_io.hpp"
#define NOFILEPRINTER (1)
#define noAssertMessage (1)

#include "my-lib/print.h"
#undef print
#undef println
#undef print_
#undef println_
#define print_(fmt, ...) print_wfO(serialOutputFunction, 0, fmt, __VA_ARGS__)
#define println_(fmt, ...) print_(fmt "\n", __VA_ARGS__);
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
#include <Wire.h>

#include "key.hpp"
#include "my-lib/mytypes.h"

constexpr u8 SLAVE_ADDR = 0x42;
constexpr usize WIRE_CLOCK = 400000;

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
static const uint colGpios[] = {10, 11, 9, 8, 7, 6};
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
  Wire.begin();
  Wire.setTimeout(500);
  Wire.setClock(WIRE_CLOCK);
}

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
  println_("loop begin");
}
usize counter = 0;
usize start = 0;
usize finish = 0;
#include "command.hpp"
void loop() {
  counter++;
  if(!( counter% 10000)) [[ unlikeley ]]{
    start = finish;
    finish = millis();
    counter = 1;
  }
  if (ESP_IO::available())[[unlikeley]] {
    mList_scoped(u8) readlist = mList_init(stdAlloc,u8);
    {
    readmore:
      while(ESP_IO::available())
        mList_push(readlist,ESP_IO::read());
    }
    if(*mList_get(readlist,mList_len(readlist)-1)!='\n')goto readmore;
    fptr input = mList_slice(readlist);
    ESP_IO::write(input.ptr,input.width);
    commands::execute(input);
    // print_("pretty printing layers");
    // delay(1000);
    // prettyPrintLayers();
    // delay(10000);
  }
  local_stateTable_update();
  remote_stateTable_update();

  bounceTable::update();
  keyMap::pressKeys
    <countof(rowGpios), countof(colGpios)>
  (bounceTable::lstate, rm); // !!will modify stateTable
  rm.send();
  static auto last = millis();
  if (millis() > last + 1000) [[unlikely]] {
    last = millis();
    for (auto i = 0; i < countof(colGpios) * 2; i++) {
      for (auto j = 0; j < countof(rowGpios); j++)
        print_("{c8}", bounceTable::lstate[j][i] ? 'X' : ' ');
      print_("|");
    }
    println_("{}",finish-start);
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
