#include "Adafruit_TinyUSB.h"
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include "arduino/hid/Adafruit_USBD_HID.h"
#include "my-list.h"
#include "my-list.hpp"
#include <SerialTransfer.h>
#include <bluefruit.h>
#include "key.hpp"

#include <SoftwareSerial.h>
#define RATE 115200
constexpr uint8_t rowGpios[] = { 10, 11, 12, 13 };
// constexpr uint8_t colGpios[] = { 16, 15, 14, 9, 8, 7 };
constexpr uint8_t colGpios[] = { 7, 8, 9, 14, 15, 16 };  // reversed from right
SoftwareSerial comms(2, 3);
SerialTransfer myTransfer;
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

BLEHidAdafruit blehid;
Adafruit_USBD_HID device(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);



#define FILENAME "/adafruit.txt";
#define CONTENTS "Adafruit Little File System test file contents";
using namespace Adafruit_LittleFS_Namespace;
File file(InternalFS);


// clang-format off
void testFunction(void){
  SerialTinyUSB.println("test function call");
}
using namespace KeyMapDec;
void (*keyboardFunctions[255])(void) = {testFunction,0}; // initialize all to NULL
KeyItem matrix[12 * 4] = {
    KEY_TAB, C('q'), C('w'), C('e'), C('r'), C('t'),    C('y'), C('u'), C('i'), C('o'), C('p'),  KEY_BACKSPACE,
    M('s') , C('a'), C('s'), C('d'), C('f'), C('g'),    C('h'), C('j'), C('k'), C('l'), C(';'),  KEY_LEFTBRACE,
    M('c') , C('z'), C('x'), C('c'), C('v'), C('b'),    C('n'), C('m'), C(','), C('.'), C('/'),  KEY_ENTER,
    0      , 0     , 0     , M('a'), C(' '),  TD(0),    L(1)  ,  FN(0),      0,      0,      0,  0,
};
KeyItem matrix2[12 * 4] = {
    KEY_ESC, C('1'), C('2'), C('3'), C('4'), C('5'),    C('6'), C('7'), C('8'), C('9'), C('0'),  KEY_DELETE,
    0      , 0     , 0     , 0     , 0     , 0     ,    0     , 0     , 0     , 0     , 0     ,  KEY_LEFTBRACE,
    0      , 0     , 0     , 0     , 0     , 0     ,    0     , 0     , 0     , 0     , 0     ,  KEY_ENTER,
    0      , 0     , 0     , M('a'), C(' '), C(' '),    L(1)  , 0     , 0     , 0     , 0     ,  0,
};

KeyItem* layers[10] = {matrix,matrix2,0};
bool state[12 * 4] = { 0 };

// clang-format on

reportManager rm;


void setup() {
  InternalFS.begin();

  // USB
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin();
  }
  // BLEHID
  Bluefruit.begin();
  blehid.begin();



  // HID
  device.begin();

  while (!TinyUSBDevice.mounted()) {
    delay(10);
  }

  // GPIO
  for (size_t i = 0; i < sizeof(rowGpios) / sizeof(uint8_t); i++) {
    pinMode(rowGpios[i], INPUT_PULLDOWN);
  }
  for (size_t i = 0; i < sizeof(colGpios) / sizeof(uint8_t); i++) {
    pinMode(colGpios[i], OUTPUT);
  }
  comms.begin(RATE);
  myTransfer.begin(comms, false);
  SerialTinyUSB.begin(RATE);
  startAdv();


  rm = reportManager(blehid, device);
}
void startAdv(void) {
  Bluefruit.setName("NRF");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  // Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);    // number of seconds in fast mode
  Bluefruit.Advertising.start(0);              // 0 = Don't stop advertising after n seconds
}

struct coord {
  uint8_t from;
  uint8_t to;
  //connection between wires from high to low
};


uint8_t otherHalfData[4] = { 0 };
void loop() {
  // while(comms.available()){
  //   uint8_t data = comms.read();
  //   otherHalfData[data>>6] = data;
  // }

  for (size_t i = 0; i < 6; i++) {
    digitalWrite(colGpios[i], HIGH);
    for (size_t j = 0; j < 4; j++) {
      state[j * 12 + i] = digitalRead(rowGpios[j]);
      state[j * 12 + i + 6] = otherHalfData[j] & ((uint8_t)1 << i);
    }
    digitalWrite(colGpios[i], LOW);
  }
  while (!myTransfer.available()) { delayMicroseconds(1); }
  uint16_t recSize = 0;
  recSize = myTransfer.rxObj(otherHalfData, recSize);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++)
      SerialTinyUSB.print((otherHalfData[i] & ((uint8_t)1 << j)) ? 'X' : ' ');
  }
  SerialTinyUSB.println();

  keyMap::pressKeys(12 * 4, layers, state, rm);
  rm.send();
}
