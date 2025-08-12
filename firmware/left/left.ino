#include "Adafruit_TinyUSB.h"
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include "arduino/hid/Adafruit_USBD_HID.h"
#include "my-list.h"
#include "my-list.hpp"
#include <bluefruit.h>
#include "key.hpp"

#include <SoftwareSerial.h>
#define RATE 1200
constexpr uint8_t rowGpios[] = { 10, 11, 12, 13 };
constexpr uint8_t colGpios[] = { 16, 15, 14, 9, 8, 7 };
SoftwareSerial comms(2, 3);

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
KeyItem matrix[12 * 4] = {
    KEY_A, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,    KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_B,
    KEY_A, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,    KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_B,
    KEY_A, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,    KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_A, KEY_B,
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_A, KEY_B,    KEY_A, KEY_B, KEY_C, KEY_D, KEY_A, KEY_B,
};
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

  while(comms.available()){
    uint8_t data = comms.read();
    otherHalfData[data>>6] = data;
  }


  for (size_t i = 0; i < 6; i++) {
    digitalWrite(colGpios[i],HIGH);
    for (size_t j = 0; j < 4; j++) {
      state[j*12+i] = digitalRead(rowGpios[j]);
      state[j*12+i+6] = otherHalfData[j] & ((uint8_t)1 << i);
    }
    digitalWrite(colGpios[i],LOW);
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++)
      SerialTinyUSB.print((otherHalfData[i] & ((uint8_t)1 << j)) ? 'X' : ' ');
  }
  delay(1);
  SerialTinyUSB.println();

  keyMap::pressKeys(12*4,matrix,state,rm);
  rm.send();
}
