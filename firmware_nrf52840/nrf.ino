

#include "Adafruit_TinyUSB.h"
#include "list/list.cpp"
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>


constexpr uint8_t gpios[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
#define FILENAME  "/adafruit.txt";
#define CONTENTS  "Adafruit Little File System test file contents";

using namespace Adafruit_LittleFS_Namespace;
File file(InternalFS);

void setup() {
  InternalFS.begin();

  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  for (size_t i = 0; i < sizeof(gpios) / sizeof(uint8_t); i++) {
    pinMode(gpios[i], INPUT_PULLDOWN);
  }

}

static listPlus<uint8_t> l;

void loop() {
  for(size_t i=0;i<sizeof(gpios)/sizeof(uint8_t);i++){
    char state = digitalRead(gpios[i]);
    if(state){
    SerialTinyUSB.print("pin: ");
    SerialTinyUSB.print(i);
    SerialTinyUSB.println("is high");
    }
  }
}
