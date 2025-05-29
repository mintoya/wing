#include "KBState.h"
#include "list.h"
#include <ArduinoJson.h>
readData readData_init(unsigned int numrows, unsigned int numcols) {
  char* a = (char*)calloc(numcols * numrows, sizeof(char));
  char* b = (char*)calloc(numcols * numrows, sizeof(char));
  readData dat = {
    .a = a,
    .b = b,
    .active = isA,
  };
  return dat;
}
char* readData_getInActive(readData r) {
  if (r.active == isA) {
    return r.b;
  } else {
    return r.a;
  }
}
char* readData_getActive(readData r) {
  if (r.active == isA) {
    return r.a;
  } else {
    return r.b;
  }
}
void readData_flip(readData* r) {
  if (r->active == isA) {
    r->active = isB;
  } else {
    r->active = isA;
  }
}
kbs kbs_init(unsigned int numrows, unsigned int numcols, unsigned int* pnRows, unsigned int* pnCols, keyAction* KBinds) {
  unsigned int* pinRows = (unsigned int*)calloc(numrows, sizeof(unsigned int));
  unsigned int* pinCols = (unsigned int*)calloc(numcols, sizeof(unsigned int));
  memcpy(pinRows, pnRows, sizeof(unsigned int) * numrows);
  memcpy(pinCols, pnCols, sizeof(unsigned int) * numcols);
  kbs res = {
    .pinCols = pinCols,
    .pinRows = pinRows,
    .colsLength = numcols,
    .rowsLength = numrows,
  };
  res.keys = (keyAction*)malloc(numrows * numcols * sizeof(keyAction));
  memcpy(res.keys, KBinds, sizeof(unsigned int) * numcols * numrows);
  res.keysState = readData_init(numrows, numcols);
  for (size_t i = 0; i < 10; i++) {
    res.modes[i] = (keyAction*)calloc(numcols * numrows, sizeof(keyAction));
  }

  //set pin modes
  kbsPinsInit(res);
  return res;
}
void kbsPinsInit(kbs currentState) {
  for (int i = 0; i < currentState.colsLength; i++) {
    Serial.print("pulling down ");
    Serial.println(currentState.pinCols[i]);
    pinMode(currentState.pinCols[i], INPUT_PULLDOWN);
  }
  for (int i = 0; i < currentState.rowsLength; i++) {
    Serial.print("Setting ");
    Serial.print(currentState.pinRows[i]);
    Serial.println(" to output");
    pinMode(currentState.pinRows[i], OUTPUT);
  }
}
void kbsFree(kbs currentState) {
  free(currentState.pinRows);
  free(currentState.pinCols);
  free(currentState.keys);
  free(currentState.keysState.a);
  free(currentState.keysState.b);
}
unsigned int rowColumn(unsigned int row, unsigned int col, kbs currentState) {
  if (row >= currentState.rowsLength || col >= currentState.colsLength)
    return 0;  // Safe default
  return row * currentState.colsLength + col;
}
void kbs_Update(kbs* currentState) {
  for (int row = 0; row < currentState->rowsLength; row++) {
    digitalWrite(currentState->pinRows[row], HIGH);
    delayMicroseconds(10);
    for (int col = 0; col < currentState->colsLength; col++) {
      unsigned int i = rowColumn(row, col, *currentState);
      char* current = readData_getActive(currentState->keysState) + i;
      int pState = digitalRead(currentState->pinCols[col]);
      if (pState == HIGH) {
        *current = 1;
      } else {
        *current = 0;
      }
    }
    digitalWrite(currentState->pinRows[row], LOW);
  }
}
void kbs_compareAndSend(kbs* currentState, BleKeyboard keyboard) {
  for (int i = 0; i < currentState->colsLength * currentState->rowsLength; i++) {
    char current = readData_getActive(currentState->keysState)[i];
    char last = readData_getInActive(currentState->keysState)[i];
    if (not last and current) {
      keyboard.press(currentState->keys[i].data);
      Serial.print("pressing");
      Serial.println(currentState->keys[i].data);
    } else if (last and not current) {
      keyboard.release(currentState->keys[i].data);
      Serial.print("releasing");
      Serial.println(currentState->keys[i].data);
    }
  }
  readData_flip(&currentState->keysState);
}
void* mcpys(void* source, unsigned int length) {
  void* res = malloc(length);
  memcpy(res, source, length);
  return res;
}

/*
void readKb(void* a,kbs* result){
  unsigned int colsLength = *((unsigned int*)a);
  a += sizeof(unsigned int);
  unsigned int rowsLength = *((unsigned int*)a);
  a += sizeof(unsigned int);
  unsigned int bigLength = colsLength*rowsLength;

  if(colsLength!=result->colsLength){
    free(result->pinCols);
    result->pinCols = (unsigned int*)mcpys(a, colsLength*sizeof(unsigned int));
  }else{
    memcpy(result->pinCols,a,colsLength*sizeof(unsigned int));
  }
  a+=sizeof(unsigned int)*colsLength;
  if (rowsLength != result->rowsLength) {
    free(result->pinRows);
    result->pinRows = (unsigned int*)mcpys(a, rowsLength*sizeof(unsigned int));
  }else{
    memcpy(result->pinRows,a,rowsLength*sizeof(unsigned int));
  }
  a+=sizeof(unsigned int)*rowsLength;
  if((colsLength == result->colsLength) and (rowsLength == result->rowsLength)){
    memcpy(result->keys,a,sizeof(keyAction)*bigLength);
    memset(result->keysState.a,0,sizeof(keyAction)*bigLength);
    memset(result->keysState.b,0,sizeof(keyAction)*bigLength);
    for (size_t i = 0; i<10; i++) {
      memcpy(result->modes[i],a,sizeof(char)*bigLength);
      a+=sizeof(char)*bigLength;
    }
  }else {
    free(result->keys);
    result->keys = (keyAction*)mcpys(a, sizeof(keyAction)*bigLength);
    free(result->keysState.a);
    free(result->keysState.b);
    result->keysState.a= (char*)calloc(bigLength, sizeof(char));
    result->keysState.b = (char*)calloc(bigLength, sizeof(char));
    for (size_t i = 0; i<10; i++) {
      free(result->modes[i]);
      result->modes[i] =(char*)mcpys(a,sizeof(char)*bigLength);
      a+=sizeof(char)*bigLength;
    }
}
*/
void readKb(void* a_ptr, kbs* result) {
  char* a = (char*)a_ptr;

  // Read colsLength and rowsLength
  unsigned int colsLength = *((unsigned int*)a);
  a += sizeof(unsigned int);
  unsigned int rowsLength = *((unsigned int*)a);
  a += sizeof(unsigned int);
  unsigned int bigLength = colsLength * rowsLength;

  // Process pinCols
  if (colsLength != result->colsLength) {
    free(result->pinCols);
    result->pinCols = (unsigned int*)malloc(colsLength * sizeof(unsigned int));
    memcpy(result->pinCols, a, colsLength * sizeof(unsigned int));
  } else {
    memcpy(result->pinCols, a, colsLength * sizeof(unsigned int));
  }
  a += colsLength * sizeof(unsigned int);

  // Process pinRows
  if (rowsLength != result->rowsLength) {
    free(result->pinRows);
    result->pinRows = (unsigned int*)malloc(rowsLength * sizeof(unsigned int));
    memcpy(result->pinRows, a, rowsLength * sizeof(unsigned int));
  } else {
    memcpy(result->pinRows, a, rowsLength * sizeof(unsigned int));
  }
  a += rowsLength * sizeof(unsigned int);

  // Process keys
  if (colsLength == result->colsLength && rowsLength == result->rowsLength) {
    memcpy(result->keys, a, sizeof(keyAction) * bigLength);
    a += sizeof(keyAction) * bigLength;  // Advance past keys

    // Correct memset for char arrays
    memset(result->keysState.a, 0, bigLength * sizeof(char));
    memset(result->keysState.b, 0, bigLength * sizeof(char));

    // Process modes
    for (size_t i = 0; i < 10; i++) {
      memcpy(result->modes[i], a, bigLength * sizeof(keyAction));
      a += bigLength * sizeof(keyAction);
    }
  } else {
    free(result->keys);
    result->keys = (keyAction*)malloc(bigLength * sizeof(keyAction));
    memcpy(result->keys, a, bigLength * sizeof(keyAction));
    a += bigLength * sizeof(keyAction);  // Advance past keys

    free(result->keysState.a);
    free(result->keysState.b);
    result->keysState.a = (char*)calloc(bigLength, sizeof(char));
    result->keysState.b = (char*)calloc(bigLength, sizeof(char));

    for (size_t i = 0; i < 10; i++) {
      free(result->modes[i]);
      result->modes[i] = (keyAction*)malloc(bigLength * sizeof(keyAction));
      memcpy(result->modes[i], a, bigLength * sizeof(keyAction));
      a += bigLength * sizeof(keyAction);
    }

    result->colsLength = colsLength;
    result->rowsLength = rowsLength;
  }
}

fatPointer bytesKb(kbs state) {
  unsigned int total_size = 2 * sizeof(unsigned int)
                            + state.colsLength * sizeof(unsigned int)
                            + state.rowsLength * sizeof(unsigned int)
                            + (state.colsLength * state.rowsLength) * sizeof(keyAction)
                            + (state.colsLength * state.rowsLength) * 10 * sizeof(keyAction);
  fatPointer result;
  result.len = total_size;
  void* buffer = malloc(total_size);
  result.ptr = buffer;
  if (!buffer) return result;
  char* ptr = (char*)buffer;
  memcpy(ptr, &state.colsLength, sizeof(unsigned int));
  ptr += sizeof(unsigned int);
  memcpy(ptr, &state.rowsLength, sizeof(unsigned int));
  ptr += sizeof(unsigned int);
  memcpy(ptr, state.pinCols, state.colsLength * sizeof(unsigned int));
  ptr += state.colsLength * sizeof(unsigned int);
  memcpy(ptr, state.pinRows, state.rowsLength * sizeof(unsigned int));
  ptr += state.rowsLength * sizeof(unsigned int);
  memcpy(ptr, state.keys, state.colsLength * state.rowsLength * sizeof(keyAction));
  ptr += state.colsLength * state.rowsLength * sizeof(keyAction);
  for (int i = 0; i < 10; i++) {
    memcpy(ptr, state.modes[i], state.colsLength * state.rowsLength * sizeof(keyAction));
    ptr += state.colsLength * state.rowsLength * sizeof(keyAction);
  }

  return result;
}
keyAction fromUint16(uint16_t src) {
  keyAction r;
  memcpy(&r, &src, sizeof(uint16_t));
  return r;
}
void readSerial(kbs* currentState, DynamicJsonDocument doc) {
  unsigned int mode = doc["mode"].as<unsigned int>();
  if (mode == 0) {
    JsonArray r = doc["rows"];
    JsonArray c = doc["cols"];
    JsonArray k = doc["keys"];

    unsigned int* rows = (unsigned int*)malloc(sizeof(unsigned int) * r.size());
    unsigned int* cols = (unsigned int*)malloc(sizeof(unsigned int) * c.size());
    keyAction* keys = (keyAction*)malloc(sizeof(keyAction) * k.size());
    char* a = (char*)calloc(k.size(), sizeof(char));
    char* b = (char*)calloc(k.size(), sizeof(char));
    keyAction* mH[10] = {
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
      (keyAction*)calloc(r.size() * c.size(), sizeof(keyAction)),
    };

    for (size_t i = 0; i < r.size(); i++) {
      rows[i] = r[i].as<unsigned int>();
    }
    for (size_t i = 0; i < c.size(); i++) {
      cols[i] = c[i].as<unsigned int>();
    }
    for (size_t i = 0; i < k.size(); i++) {
      const char* str = k[i].as<const char*>();
      keys[i].data = (str && strlen(str) > 0) ? str[0] : 0;
      //keys[i].data = k[i].as<unsigned char>();
      keys[i].kind = 0x0;
    }
    free(currentState->pinCols);
    free(currentState->pinRows);
    free(currentState->keys);
    free(currentState->keysState.a);
    free(currentState->keysState.b);
    for (size_t i = 0; i < 10; i++) {
      free(currentState->modes[i]);
    }
    currentState->keys = keys;
    currentState->keysState.a = a;
    currentState->keysState.b = b;
    currentState->pinCols = cols;
    currentState->pinRows = rows;
    currentState->colsLength = c.size();
    currentState->rowsLength = r.size();
    for (size_t i = 0; i < 10; i++) {
      currentState->modes[i] = mH[i];
    }
  } else {
    currentState->modes[mode-1] =(keyAction*)calloc( currentState->rowsLength * currentState->colsLength, sizeof(keyAction));
    keyAction* current = currentState->modes[mode-1];
    JsonArray keys = doc["keys"];
    if(keys.size() != currentState->rowsLength*currentState->colsLength){
      Serial.println("wrong key number");
      return;
    }
    for (size_t i = 0; i < keys.size(); i++) {
      const char* str = keys[i].as<const char*>();
      current[i].data = (str && strlen(str) > 0) ? str[0] : 0;
      current[i].kind = 0x0;
    }
  }
}
