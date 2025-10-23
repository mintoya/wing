
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
#include "fileSystemInterface.hpp"


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
bool state_presskeys[nrowGpios * ncolGpios * 2] = { false };
void (*keyboardFunctions[10])(void) = {};

bool fakeSenderEnabled = true;
void fakeSender(uint8_t mod, uint8_t *keys) {
  if (!fakeSenderEnabled) return;
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
  for (int i = 0; i < sizeof(pinData); i++)
    Serial.printf("%02X", pinData[i]);
  Serial.println();
}
#endif
void wireSetup() {
#ifdef ISRIGHT
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(onRequest);
#else
  Wire.begin();
#endif
  Wire.setClock(400000UL);
}
#ifndef ISRIGHT
listPlus<uint8_t> retBuf;
void wireUpdate() {
  int rSize = Wire.requestFrom(SLAVE_ADDR, 0xFF, true);

  if (!Wire.available()) {
  } else {
    // Serial.println("recieved");
    int lSize = Wire.read();
    retBuf.clear();
    for (int i = 0; i < lSize; i++) {
      if (Wire.available())
        retBuf.append((uint8_t)Wire.read());
    }
  }

  checkData cd = (checkData){
    .data = (um_fp){
      .ptr = retBuf.self(),
      .width = retBuf.length() },
  };

  um_fp result = cSum_fromSum(cd);
  if (!result.ptr) {
    // Serial.println("recieve fail");
    oPinData = nullptr;
  } else {
    oPinData = (uint8_t *)result.ptr;
    // for (int i = 0; i < result.width; i++)
    //   Serial.printf("%02X", oPinData[i]);
    // Serial.println();
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
  for (unsigned int r = 0; r < nrowGpios; r++) {
    for (unsigned int c = 0; c < totalCols; c++) {
      state_presskeys[r * totalCols + c] = state[r * totalCols + c].get();
    }
  }
}
#endif
void setup() {
  Serial.begin(115200);
  delay(2000);
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
  FSISetup();
  parseLayout();
  prettyPrintLayers();
  rm = reportManager(sendHidReport, fakeSender);
#endif
}

#ifdef ISRIGHT
void loop() {}
#else
void serialRequestManager(um_fp layo) {
  um_fp requestType;
  if (!(requestType = findKey(layo, um_from("request"))).ptr) {
    Serial.println("status:fail;");
    Serial.println("no requestType provided");
    Serial.write((uint8_t *)layo.ptr, layo.width);
    Serial.println();
    Serial.println(
      "examples\n\n"
      "request:setLayout;keyboard:{layers:{{KEY_A}}}\t\t-- sets layout\n"
      "request:getLayout;\t\t-- prints layout\n"
      "request:getDefaultLayout;\t\t-- prints default layout \n"
      "request:disableStrokes;\t\t-- disables the keycode report\n"
      "request:enableStrokes;\t\t-- enables them \n");
  } else {

    Serial.println("status:sucess;");
    if (requestType == um_from("setLayout")) {
      parseLayout(layo);
    } else if (requestType == um_from("format")) {
      Serial.println(esp_reset_reason());
      Serial.println("Unmounting FFat ...");
      FFat.end();
      if (FFat.format()) {
        Serial.println("FFat formatted successfully. Attempting to remount...");
        if (FFat.begin()) {
          Serial.println("FFat remounted successfully. Filesystem is now empty.");
        } else {
          Serial.println("ERROR: FFat failed to remount after format! Please reset the device.");
        }
      } else {
        Serial.println("ERROR: FFat format failed!");
      }
    } else if (requestType == um_from("ls")) {
      listDir("/");
    } else if (
      requestType == um_from("setDefaultLayout") || requestType == um_from("set default layout")) {
      Serial.println("setting default Layout");
      parseLayout(defaultLayout_um);
    } else if (
      requestType == um_from("getLayout") || requestType == um_from("get layout")) {
      um_fp savedLayout = readFile("/lay.kml");
      Serial.write((uint8_t *)savedLayout.ptr, savedLayout.width);
      Serial.println();
      free(savedLayout.ptr);
    } else if (
      requestType == um_from("getDefaultLayout") || requestType == um_from("get default layout")) {
      Serial.println("getting default Layout");
      Serial.write((uint8_t *)defaultLayout_um.ptr, defaultLayout_um.width);
    } else if (requestType == um_from("disableStrokes")) {
      fakeSenderEnabled = false;
    } else if (requestType == um_from("enableStrokes")) {
      Serial.println("enabling keycode report");
      fakeSenderEnabled = true;
    }
  }
}
void loop() {
  if (Serial.available()) {
    unsigned long now = millis();
    listPlus<uint8_t> readBuf;
    while (Serial.available()) {
      uint8_t b = Serial.read();
      readBuf.append(b);
    }
    um_fp layo = { .ptr = (void *)readBuf.self(), .width = readBuf.length() };
    um_fp requestLength = findKey(layo, um_from("requestLength"));
    unsigned int msgLength = um_asUint(requestLength);
    if (requestLength.ptr) {
      while (layo.width < msgLength + 13 + requestLength.width && (millis() - now) < 5000) {
        while (Serial.available()) {
          uint8_t b = Serial.read();
          readBuf.append(b);
        }
      }
    }


    layo = { .ptr = (void *)readBuf.self(), .width = readBuf.length() };

    serialRequestManager(layo);
    readBuf.unmake();
  }
  fillKeyStates();
  keyMap::pressKeys(state_presskeys, rm);
  rm.send();
}
#endif
