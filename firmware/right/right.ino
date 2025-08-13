#include "Adafruit_TinyUSB.h"
#include <SerialTransfer.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
constexpr uint8_t rowGpios[] = {10,11,12,13};
constexpr uint8_t colGpios[] = {16,15,14,9,8,7};
// wired from col 2 row 
uint8_t bitData[4] = {0};
#define RATE 115200
SoftwareSerial comms(3,2);
SerialTransfer myTransfer;
void setup() {

  // USB
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin();
  }

  // Serial
  comms.begin(RATE);
  while (!comms) delay(10);
  myTransfer.begin(comms);

  // GPIO
  for (size_t i = 0; i < sizeof(rowGpios) / sizeof(uint8_t); i++) {
    pinMode(rowGpios[i], INPUT_PULLDOWN);
  }
    for (size_t i = 0; i < sizeof(colGpios) / sizeof(uint8_t); i++) {
    pinMode(colGpios[i], OUTPUT);
  }

  SerialTinyUSB.begin(RATE);
  SerialTinyUSB.println("READY");
}
// doesnt work if cols is more than 4 
void printActiveKeys(uint8_t*bitData) {

  bitData[0] = 0;
  bitData[1] = 0;
  bitData[2] = 0;
  bitData[3] = 0;
  
  for (size_t i = 0; i < 6; i++) {
    digitalWrite(colGpios[i],HIGH);
    for (size_t j = 0; j < 4; j++) {
      if (digitalRead(rowGpios[j])) {
        bitData[j] |= ((uint8_t)1)<<i ;
      }
      bitData[j]|=(uint8_t)j<<6;
    }
    digitalWrite(colGpios[i],LOW);
  }
}

void loop() {
    printActiveKeys(bitData);
    // comms.write(bitData[0]);
    // comms.write(bitData[1]);
    // comms.write(bitData[2]);
    // comms.write(bitData[3]);
    // comms.flush();

    unsigned long  sendSize = 0;
    sendSize = myTransfer.txObj(bitData, sendSize);
    myTransfer.sendData(sendSize);

    delayMicroseconds(100);
    if(SerialTinyUSB){
    for (int i = 0; i < 4; i++) {
      for(int j =0;j<8;j++)
        SerialTinyUSB.print((bitData[i]&((uint8_t)1<<j)) ? 'X' : ' ');
    }
    SerialTinyUSB.println();
    }

}


