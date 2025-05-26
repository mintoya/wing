#ifndef KBSTATE_H
#define KBSTATE_H

#include <Arduino.h>
#include "BleKeyboard.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <string.h>

typedef struct keyAction {
  char kind;
  unsigned char data;
}keyAction ;

typedef enum wich {
    isA,
    isB
} which;

typedef struct readData {
    char* a;
    char* b;
    which active;
} readData;

typedef struct kbs {
    unsigned int* pinCols;
    unsigned int* pinRows;
    unsigned int colsLength;
    unsigned int rowsLength;
    keyAction* keys;
    char* modes[10];
    readData keysState;
} kbs;

readData readData_init(unsigned int numrows, unsigned int numcols);

char* readData_getActive(readData r);
char* readData_getInActive(readData r);

void readData_flip(readData* r);

typedef struct fatPointer {
  void* ptr;
  unsigned int len;
} fatPointer;

// Initialize keyboard state (allocates pins and buffers)
kbs kbs_init(unsigned int numrows, unsigned int numcols, unsigned int* pnRows,unsigned int* pnCols,keyAction* KBinds);

// Scan and update key state into the active buffer
void kbs_Update(kbs* currentState);

// Compare buffers and send key events via BLE Keyboard
void kbs_compareAndSend(kbs* currentState, BleKeyboard keyboard);
fatPointer bytesKb(kbs state);
void readKb(void* a,kbs* result);
void kbsFree(kbs currentState);
void kbsPinsInit(kbs currentState);
void readSerial(kbs*,DynamicJsonDocument);
#endif // KBSTATE_H
