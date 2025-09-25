
#ifdef ISRIGHT
static const unsigned int rowGpios[] = { 2, 3, 4, 5 };
static const unsigned int colGpios[] = {10,11,9,8,7,6};
#else //ISLEFT
static const unsigned int rowGpios[] = { 2, 3, 4, 5 };
static const unsigned int colGpios[] = { 6, 7, 8, 9, 11, 10 } ;
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

#define MY_LIST_C
#include "my-list/my-list.h"

#define SLAVE_ADDR 0x42

dataChecker dc = cSum_new();

listPlus<listPlus<KeyItem>> keyMapLayers;
listPlus<tapDance> tapDances;
bool state[nrowGpios*ncolGpios*2] = {0};
void (*keyboardFunctions[255])(void) = {};


reportManager rm;
void printBits(uint8_t *pinData, size_t nrowGpios) {
    for (size_t i = 0; i < nrowGpios; i++) {
      Serial.print('|');
        for (int bit = 7; bit >= 0; bit--) {
            if (pinData[i] & (1u << bit)) {
                Serial.print('x');
            } else {
                Serial.print(' ');
            }
        }
        Serial.print('|');
    }
    Serial.println();
}
void fakeSender(uint8_t mod, uint8_t* keys) {
    Serial.printf("%02X|%02X %02X %02X %02X %02X %02X\n",
                  mod,
                  keys[0], keys[1], keys[2],
                  keys[3], keys[4], keys[5]);
}

uint8_t pinData[nrowGpios] = {0};
uint8_t*oPinData = nullptr;

static inline bool getKeyState(unsigned int row, unsigned int col){ uint8_t res = (col<ncolGpios)?( pinData [row] & ( ( (uint8_t)1 )<<col) ):( oPinData?( oPinData[row] & ( ( (uint8_t)1 )<<(col-ncolGpios)) ):0 ) ; return res; }

static inline void fillKeyStates(bool *state) {
    unsigned int totalCols = ncolGpios * 2;  // primary + optional
    for (unsigned int r = 0; r < nrowGpios; r++) {
        for (unsigned int c = 0; c < totalCols; c++) {
            state[r * totalCols + c] = getKeyState(r, c);
        }
    }
}



void pab(){
  for(int i = 0;i<nrowGpios;i++){
    for(int ii=0;ii<ncolGpios*2;ii++){
      Serial.write(getKeyState(i,ii)?'X':' ');
    }
    Serial.println();
  }
}

void updatePins(){
  memset(pinData,0,sizeof(uint8_t)*nrowGpios);
  for(int i = 0;i<ncolGpios;i++){
    digitalWrite(colGpios[i],HIGH);
      for(int ii = 0;ii<nrowGpios;ii++){
        pinData[ii] |= (digitalRead(rowGpios[ii])<<i);
      }
    digitalWrite(colGpios[i],LOW);
  }
}

#ifdef ISRIGHT
void onRequest() {
  updatePins();
  checkData cd = cSum_toSum(dc,( um_fp ){.ptr = pinData,.width = sizeof(pinData) });
  Serial.print( cd.data.width);
  Serial.println(" bytes;");
  Wire.write(cd.data.width);
  Wire.write((uint8_t*)cd.data.ptr,cd.data.width);
  printBits(pinData,nrowGpios);
}
#endif
void wireSetup(){
#ifdef ISRIGHT
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(onRequest);
#else
  Wire.begin();
#endif
}
void setup() {
  Serial.begin(115200);
  wireSetup();
  for(int i =0;i<ncolGpios;i++){
    pinMode(colGpios[i],OUTPUT);
    digitalWrite(colGpios[i],LOW);
  }
  for(int i =0;i<nrowGpios;i++){
    pinMode(rowGpios[i],INPUT_PULLDOWN);
  }
  parseLayout();
  prettyPrintLayers();
  delay(10000);
  rm = reportManager(fakeSender,nullptr);
}
listPlus<uint8_t> retBuf;
#ifdef ISRIGHT
void loop() { }
#else


void loop() {
  int rSize = Wire.requestFrom(SLAVE_ADDR,0xFF,true);

  if(!Wire.available()){
    // Serial.println("na?");
  }else{
    rSize = Wire.read();
    retBuf.clear();
    for(int i = 0;i<rSize;i++){
      if(Wire.available())
        retBuf.append((uint8_t)Wire.read());
    }
  }


  checkData cd = (checkData){
    .data = (um_fp){
      .ptr = retBuf.self(),
      .width = retBuf.length()
    },
  };
  um_fp result = cSum_fromSum(cd);
  if(!result.ptr){
    // Serial.println("left sent no data");
    oPinData = nullptr;
  }else{
    oPinData = (uint8_t*)result.ptr;
  }
  if(Serial.available()){
    while(Serial.available()){
      Serial.read();
    }
    parseLayout();
    prettyPrintLayers();
    delay(30000);
  }
  updatePins();
  fillKeyStates(state);

  keyMap::pressKeys(nrowGpios*ncolGpios*2, keyMapLayers , state, rm);

  rm.send();
  // pab();
}
#endif
