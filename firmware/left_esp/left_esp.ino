// #define noAssertMessage
// #define ISRIGHT
#define PRINTER_NOC32TOMB

#include "my-lib/types.h"
#include "my-lib/print.h"
#undef print
#undef println
#undef print_
#undef println_
#include "my-lib/printers/vason.c"

void (serialOutputFunction)(
    const c32 *c,
    void *_,
    unsigned int len,
    bool flush
){
  for(u32 i = 0;i<len;i++)
  Serial.write((char)( c[i] ));
  if(flush)Serial.flush();
}
#define print_(fmt,...) print_wfO(serialOutputFunction,0,fmt,__VA_ARGS__)
#define println_(fmt,...) print_(fmt"\n",__VA_ARGS__);
#ifdef ISRIGHT
static const uint rowGpios[] = { 2, 3, 4, 5 };
static const uint colGpios[] = { 10, 11, 9, 8, 7, 6 };
#else  // ISLEFT
static const uint rowGpios[] = { 2, 3, 4, 5 };
static const uint colGpios[] = { 6, 7, 8, 9, 11, 10 };
#endif

const uint nrowGpios = countof(rowGpios);
const uint ncolGpios = countof(colGpios);

#include <Wire.h>
#define cSum_REDUNDANCY_AMMOUNT ((unsigned int)2)
#include "my-lib/cSum.h"
#include "key.hpp"
#include "layout.hpp"
#include "debounce.hpp"
#include "fileSystemInterface.hpp"
#include <USB.h>
#include <USBHID.h>
#include <USBHIDKeyboard.h>


#define SLAVE_ADDR 0x42

USBHIDKeyboard Keyboard;
dataChecker dc = cSum_new(stdAlloc);
reportManager rm;

mList(mList(KeyItem)) keyMapLayers = mList_init(stdAlloc, mList(KeyItem));
mList(tapDance) tapDances = mList_init(stdAlloc, tapDance);
u8 pinData[nrowGpios] = { 0 };
u8 *oPinData = nullptr;
dbool state[nrowGpios * ncolGpios * 2] = {};
bool state_presskeys[nrowGpios * ncolGpios * 2] = { false };
void (*keyboardFunctions[10])(void) = {};

bool fakeSenderEnabled = true;
void fakeSender(u8 mod, u8 *keys) {
  if (!fakeSenderEnabled)
    return;
  println_("{x}|{x} {x} {x} {x} {x} {x}", mod, keys[0], keys[1],
                keys[2], keys[3], keys[4], keys[5]);
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

// TODO flip these around
static inline bool getKeyState(uint row, uint col) {
  u8 res =
    (col < ncolGpios)
      ? (pinData[row] & (((u8)1) << col))
      : (oPinData ? (oPinData[row] & (((u8)1) << (col - ncolGpios)))
                  : 0);
  return res;
}

void updatePins() {
  memset(pinData, 0, sizeof(u8) * nrowGpios);
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
  checkData cd =
    cSum_toSum(dc, (fptr){ .ptr = pinData, .width = sizeof(pinData) });
  Wire.write(cd.data.width);
  Wire.write((u8 *)cd.data.ptr, cd.data.width);
  for (int i = 0; i < sizeof(pinData); i++)
    printf_("{x}", pinData[i]);
  println_();
}
#endif
void wireSetup() {
#ifdef ISRIGHT
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(onRequest);
#else
  Wire.begin();
  Wire.setTimeout(500);
#endif
  Wire.setClock(400000);
}
#ifndef ISRIGHT
mList(u8) retBuf = mList_init(stdAlloc, u8);
void wireUpdate() {
  // Wire.clearTimeoutFlag();//not on twowire?
  int rSize = Wire.requestFrom(SLAVE_ADDR, 0xFF, true);

  if (!Wire.available()) {
  } else {
    // println_("recieved");
    int lSize = Wire.read();
    mList_clear(retBuf);
    for (int i = 0; i < lSize; i++) {
      if (Wire.available())
        mList_push(retBuf, (u8)Wire.read());
    }
  }

  checkData cd = (checkData){
    .data = (fptr){ mList_len(retBuf), mList_arr(retBuf) },
  };

  fptr result = cSum_fromSum(cd);
  if (!result.ptr) {
    // println_("recieve fail");
    oPinData = nullptr;
  } else {
    oPinData = (u8 *)result.ptr;
    // for (int i = 0; i < result.width; i++)
    //   printf_("%02X", oPinData[i]);
    // println_();
  }
}
static inline void fillKeyStates() {
  uint totalCols = ncolGpios * 2;  // primary + optional
  for (uint r = 0; r < nrowGpios; r++) {
    for (uint c = 0; c < totalCols; c++) {
      state[r * totalCols + c].set(getKeyState(r, c));
    }
  }
  for (uint r = 0; r < nrowGpios; r++) {
    for (uint c = 0; c < totalCols; c++) {
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
#endif
}

#ifdef ISRIGHT
void loop() {}
#else
#include "my-lib/arenaAllocator.h"
void seerialRequestManager(fptr layo) {
  Arena_scoped* local = arena_new_ext(stdAlloc,512);
  vason v = { parseStr(local, (slice(c8)){ layo.width,(c8*)layo.ptr,}) };

  // 2. Use the vason object 'v' for lookups
  fptr requestType = v["request"].asString();

  if (!requestType.ptr) {
    println_("status:fail;");
    println_("no requestType provided");
    // Serial.write((u8 *)layo.ptr, layo.width); 
    println_();
    println_(
      "examples\n\n"
      "request:setLayout;keyboard:{layers:{{KEY_A}}}\t\t-- sets layout\n"
      "request:getLayout;\t\t-- prints layout\n"
      "request:getDefaultLayout;\t\t-- prints default layout \n"
      "request:disableStrokes;\t\t-- disables the keycode report\n"
      "request:enableStrokes;\t\t-- enables them \n");
  } else {

    println_("status:sucess;");
    
    // 3. Compare using fp()
    if (requestType == fp("setLayout")) {
      // Pass 'v' (the parsed object) or 'layo' (raw string) depending on parseLayout's signature
      parseLayout(layo); //TODO change sig
    } else if (requestType == fp("format")) {
      println_("{}",esp_reset_reason());
      println_("Unmounting FFat ...");
      FFat.end();
      println_("Formatting...");
      if (FFat.format()) {
        println_("FFat formatted successfully. Attempting to remount...");
        if (FFat.begin()) {
          println_(
            "FFat remounted successfully. Filesystem is now empty.");
        } else {
          println_("ERROR: FFat failed to remount after format! Please "
                         "reset the device.");
        }
      } else {
        println_("ERROR: FFat format failed!");
      }
    } else if (requestType == fp("printFile")) {
      // Use vason syntax for lookup
      fptr fileName = v["fileName"].asString();
      if (fileName.ptr) {
        println_("{}",readFile(fileName));
      }
    } else if (requestType == fp("ls")) {
      listDir("/");
    } else if (requestType == fp("setDefaultLayout") || requestType == fp("set default layout")) {
      println_("setting default Layout");
      // Assuming parseLayout can handle the default layout object/string
      parseLayout();
    } else if (requestType == fp("getLayout") || requestType == fp("get layout")) {
      fptr t = readFile("/lay.kml");
      println_("{}",t);
      free(t.ptr);
    } else if (requestType == fp("getDefaultLayout") || requestType == fp("get default layout")) {
      println_("getting default Layout \n {}\n",defaultLayout_chars);
    } else if (requestType == fp("disableStrokes")) {
      fakeSenderEnabled = false;
    } else if (requestType == fp("enableStrokes")) {
      println_("enabling keycode report");
      fakeSenderEnabled = true;
    }
  }
}
void loop() {

  wireUpdate();
  updatePins();
  fillKeyStates();

  keyMap::pressKeys(state_presskeys, rm);

  rm.send();

  if (Serial.available()) {
    unsigned long now = millis();
    mList_scoped(u8) readBuf = mList_init(stdAlloc,u8);
    while (Serial.available())
      mList_push(readBuf, (u8)Serial.read());
    auto layo = (fptr){
      mList_len(readBuf),
      mList_arr(readBuf),
    };

    // fptr requestLength = findKey(layo, um_from("requestLength"));
    // uint msgLength = um_asUint(requestLength);
    // if (requestLength.ptr) {
    //   while (layo.width < msgLength + 13 + requestLength.width && (millis() - now) < 5000) {
    //     while (Serial.available())
    //       mList_push(readBuf, (u8)Serial.read());
    //   }
    // }

    seerialRequestManager(layo);
  }
}
#endif
#include "my-lib/wheels.h"
