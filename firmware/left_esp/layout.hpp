#pragma once
#include "hid_keys.h"
#include "hid_keys_names.h"
#include "key.hpp"
#include "my-list/my-list.h"
#include "my-list/my-list.hpp"
#include "stdint.h"

#include "kml/kml.h"
#define um_Serial_Print(um) Serial.printf("%.*s\n",um.width,(char*)um.ptr)
um_fp defaultLayout_um = um_from(R"d(
  keyboard:{
    tapDances:[
      {
        taps:[ KEY_A, KEY_B, KEY_C, KEY_D ]
        holds:[ M( a ), M( s ), KEY_C, KEY_D ]
      }
    ]
    functions:[
      printDebug,
      bootLoader,
      reset 
    ]
    layers:[
       [
          KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,        KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,  KEY_BACKSPACE,
          M( s ) , KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,        KEY_H, KEY_J, KEY_K, KEY_L, KEY_COMMA,  KEY_LEFTBRACE,
          M( c ) , KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,        KEY_N, KEY_M, KEY_COMMA , KEY_DOT, KEY_SLASH,  KEY_ENTER,
          0      , 0    , 0   , KEY_MOD_ALT,KEY_SPACE,  0,        L(1)  ,   L(2),      0,      0,      0,  0,
      ],
    ]
  }
)d");

extern const unsigned int nrowGpios;
extern const unsigned int ncolGpios;

extern listPlus<listPlus<KeyItem>> keyMapLayers;

static void parseLayers(um_fp layersBuf) {
  keyMapLayers.resize(10);
  um_fp thisLayerBuf;

  for (unsigned int i = 0;; i++) {
    thisLayerBuf = findIndex(layersBuf, i);

    if (thisLayerBuf.ptr == nullptr)
      break;

    listPlus<KeyItem> thisLayer;
    thisLayer.pad(nrowGpios * ncolGpios); // force fill with enough keys
    thisLayer.clear();

    um_fp thisKeyBuf;
    for (unsigned int ii = 0;; ii++) {
      thisKeyBuf = findIndex(thisLayerBuf, ii);

      if (thisKeyBuf.ptr == nullptr) {
        Serial.print("parsed");
        Serial.print(ii);
        Serial.println(" keys");
        break;
      }

      Serial.print("key ");
      um_Serial_Print(thisKeyBuf);
      Serial.println();
      thisLayer.append(KeyItem(kn_Match(thisKeyBuf)));
    }
    while (thisLayer.length() < ncolGpios * nrowGpios) {
      thisLayer.append(KeyItem());
    }
    keyMapLayers.append(thisLayer);
  }
  Serial.print("parsed ");
  Serial.print(keyMapLayers.length());
  Serial.println("layers");
  if (keyMapLayers.length() < 1) {
    listPlus<KeyItem> thisLayer;
    thisLayer.pad(nrowGpios * ncolGpios);
    keyMapLayers.append(thisLayer);
  }
  if (keyMapLayers.length() < 10) {
    for (unsigned int i = keyMapLayers.length(); i < 10; i++) {
      keyMapLayers.append(keyMapLayers.get(0));
    }
  }
  Serial.println("done");
}
static void parseLayout(um_fp layoutBuf = defaultLayout_um) {
  Serial.println("outer: ");
  um_Serial_Print(layoutBuf);
  parseArg p;
  p.type = parseArg::STRING;
  p.data.pArg = (um_fp){.ptr = (char *)"keyboard", .width = 8};
  um_fp keyboard = findAny(defaultLayout_um, p);
  Serial.println("keyboard: ");
  um_Serial_Print(keyboard);
  parseArg arg;
  arg.type = parseArg::STRING; // if fKey returns a string key
  arg.data.pArg = (um_fp){.ptr = (char *)"layers", .width = 6};
  um_fp layersBuf = findAny(keyboard, arg);
  parseLayers(layersBuf);
}
static void prettyPrintLayers() {
  Serial.println("=== Keymap Layers ===");
  for (unsigned int layer = 0; layer < keyMapLayers.length(); layer++) {
    Serial.print("Layer ");
    Serial.println(layer);
    for (unsigned int r = 0; r < nrowGpios; r++) {
      for (unsigned int c = 0; c < ncolGpios; c++) {
        unsigned int idx = r * ncolGpios + c;
        KeyItem k = keyMapLayers.get(layer).get(idx);
        printKeyItem(k);
        Serial.print("\t"); // column spacing
      }
      Serial.println();
    }
    Serial.println();
  }
}
