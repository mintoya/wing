
#ifdef ISRIGHT
static const unsigned int rowGpios[] = { 2, 3, 4, 5 };
static const unsigned int colGpios[] = { 10, 11, 9, 8, 7, 6 };
#else  //ISLEFT
static const unsigned int rowGpios[] = { 2, 3, 4, 5 };
static const unsigned int colGpios[] = { 6, 7, 8, 9, 11, 10 };
#endif

const unsigned int nrowGpios = sizeof(rowGpios) / sizeof(rowGpios[0]);
const unsigned int ncolGpios = sizeof(colGpios) / sizeof(colGpios[0]);


#include <Wire.h>
#define cSum_REDUNDANCY_AMMOUNT ((unsigned int)4)
#include "kml/kml.h"
#include "cSum/cSum.h"
#include "key.hpp"
#include "my-list/my-list.hpp"
#include "layout.hpp"
#define KML_PARSER_C
#include "kml/kml.h"
#include <USB.h>
#include <USBHID.h>
#include <USBHIDKeyboard.h>
#include "debounce.hpp"


#define MY_LIST_C
#include "my-list/my-list.h"

#define SLAVE_ADDR 0x42

USBHIDKeyboard Keyboard;
dataChecker dc = cSum_new();
reportManager rm;

listPlus<listPlus<KeyItem>> keyMapLayers;
listPlus<tapDance> tapDances;
uint8_t pinData[nrowGpios] = { 0 };
uint8_t *oPinData = nullptr;
dbool state[nrowGpios * ncolGpios * 2] = {};
void (*keyboardFunctions[255])(void) = {};


void fakeSender(uint8_t mod, uint8_t *keys) {
  Serial.printf("%02X|%02X %02X %02X %02X %02X %02X\n",
                mod,
                keys[0], keys[1], keys[2],
                keys[3], keys[4], keys[5]);
}
void sendHidReport(uint8_t modifiers, uint8_t *key_codes) {
  // typedef struct
  // {
  //   uint8_t modifiers;
  //   uint8_t reserved;
  //   uint8_t keys[6];
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



static inline bool getKeyState(unsigned int row, unsigned int col) {
  uint8_t res = (col < ncolGpios) ? (pinData[row] & (((uint8_t)1) << col)) : (oPinData ? (oPinData[row] & (((uint8_t)1) << (col - ncolGpios))) : 0);
  return res;
}



void updatePins() {
  memset(pinData, 0, sizeof(uint8_t) * nrowGpios);
  for (int i = 0; i < ncolGpios; i++) {
    digitalWrite(colGpios[i], HIGH);
    for (int ii = 0; ii < nrowGpios; ii++) {
      pinData[ii] |= (digitalRead(rowGpios[ii]) << i);
    }
    digitalWrite(colGpios[i], LOW);
  }
}

#ifdef ISRIGHT
void onRequest() {
  updatePins();
  checkData cd = cSum_toSum(dc, (um_fp){ .ptr = pinData, .width = sizeof(pinData) });
  Wire.write(cd.data.width);
  Wire.write((uint8_t *)cd.data.ptr, cd.data.width);
}
#endif
void wireSetup() {
#ifdef ISRIGHT
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(onRequest);
#else
  Wire.begin();

#endif
}
#ifndef ISRIGHT
listPlus<uint8_t> retBuf;
void wireUpdate(){
  int rSize = Wire.requestFrom(SLAVE_ADDR, 0xFF, true);

  if (!Wire.available()) {
  } else {
    int lSize = Wire.read();
    retBuf.clear();
    while(retBuf.length()<lSize){
      for (int i = 0; i < rSize; i++) {
        if (Wire.available())
          retBuf.append((uint8_t)Wire.read());
      }
      rSize = Wire.requestFrom(SLAVE_ADDR, 0xFF, true);
    }
  }

  checkData cd = (checkData){
    .data = (um_fp){
      .ptr = retBuf.self(),
      .width = retBuf.length() },
  };

  um_fp result = cSum_fromSum(cd);
  if (!result.ptr) {
    oPinData = nullptr;
  } else {
    oPinData = (uint8_t *)result.ptr;
  }
}
static inline void fillKeyStates() {
  updatePins();
  wireUpdate();
  unsigned int totalCols = ncolGpios * 2;  // primary + optional
  for (unsigned int r = 0; r < nrowGpios; r++) {
    for (unsigned int c = 0; c < totalCols; c++) {
      state[r * totalCols + c].set(getKeyState(r, c));
    }
  }
}
#endif
void setup() {
  Serial.begin(115200);
  wireSetup();
  for (int i = 0; i < ncolGpios; i++) {
    pinMode(colGpios[i], OUTPUT);
    digitalWrite(colGpios[i], LOW);
  }
  for (int i = 0; i < nrowGpios; i++) {
    pinMode(rowGpios[i], INPUT_PULLDOWN);
  }
#ifndef ISRIGHT
  Keyboard.begin();
  parseLayout();
  prettyPrintLayers();
  //HAS TO BE IN THIS ORDER
  rm = reportManager(sendHidReport, fakeSender);
#endif
}

#ifdef ISRIGHT
void loop() {}
#else
void loop() {
  if (Serial.available()) {
    listPlus<uint8_t> readBuf;
    while (Serial.available()) {
      uint8_t b = Serial.read();
      readBuf.append(b);
    }

    um_fp layo = { .ptr = (void *)readBuf.self(), .width = readBuf.length() };
    Serial.write((uint8_t *)layo.ptr, layo.width);
    parseLayout(layo);


    prettyPrintLayers();

    char c = 0;
    while (c != 'y' && c != 'n') {
      Serial.println("y/n to confirm");
      while (!Serial.available()) { delay(10); }
      c = Serial.read();
      while (Serial.available()) { Serial.read(); }
    }

    if (c == 'y') {
      Serial.println("using this layout");
    } else {
      parseLayout();  // fallback layout
    }
    readBuf.unmake();
  }
  fillKeyStates();
  keyMap::pressKeys(nrowGpios * ncolGpios * 2, keyMapLayers, state, rm);
  rm.send();
}
#endif
