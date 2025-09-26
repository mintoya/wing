#pragma once
#include "fileSystemInterface.hpp"
#include "hid_keys.h"
#include "hid_keys_names.h"
#include "key.hpp"
#include "my-list/my-list.h"
#include "my-list/my-list.hpp"
#include "stdint.h"

#include "kml/kml.h"
#define um_Serial_Print(um) Serial.printf("%.*s\n", um.width, (char *)um.ptr)
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
          M( ls ) , KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,        KEY_H, KEY_J, KEY_K, KEY_L, KEY_COMMA,  KEY_LEFTBRACE,
          M( lc ) , KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,        KEY_N, KEY_M, KEY_COMMA , KEY_DOT, KEY_SLASH,  KEY_ENTER,
          0      , 0    , M( la ),KEY_LEFTMETA,KEY_SPACE,  0,        L(1)  ,   L(2),      0,      0,      0,  0,
      ],
    ]
  }
)d");

extern const unsigned int nrowGpios;
extern const unsigned int ncolGpios;

extern listPlus<listPlus<KeyItem>> keyMapLayers;

static void prettyPrintLayers() {
  // Print header
  Serial.println("=== Keyboard Layers ===");

  // Iterate through each layer
  for (int layerIndex = 0; layerIndex < keyMapLayers.length(); layerIndex++) {
    Serial.print("Layer ");
    Serial.print(layerIndex);
    Serial.print(" (");
    Serial.print(keyMapLayers.get(layerIndex).length());
    Serial.println(" keys):");

    // Get the current layer
    listPlus<KeyItem> currentLayer = keyMapLayers.get(layerIndex);

    // Print each key in the layer
    for (int keyIndex = 0; keyIndex < currentLayer.length(); keyIndex++) {
      Serial.print("  Key ");
      Serial.print(keyIndex);
      Serial.print(": ");

      // Print the individual key item
      KeyItem k = currentLayer.get(keyIndex);
      printKeyItem(k); // Assuming this function exists
    }
    Serial.println(); // Empty line between layers
  }
  Serial.println("=======================");
}

static void parseLayers(um_fp layersBuf) {
  keyMapLayers.clear();

  um_fp thisLayerBuf;

  for (unsigned int i = 0;; i++) {
    thisLayerBuf = findIndex(layersBuf, i);

    if (thisLayerBuf.ptr == nullptr)
      break;

    listPlus<KeyItem> thisLayer(nullptr);
    if (keyMapLayers.length() > i) {
      thisLayer = keyMapLayers.get(i);
    } else {
      thisLayer = listPlus<KeyItem>();
    }
    thisLayer.pad(2 * nrowGpios * ncolGpios); // force fill with enough keys
    thisLayer.clear();

    um_fp thisKeyBuf;
    for (unsigned int ii = 0; ii < 2 * nrowGpios * ncolGpios; ii++) {
      thisKeyBuf = findIndex(thisLayerBuf, ii);
      thisLayer.append(KeyItem(kn_Match(thisKeyBuf))); // can handle null
    }
    keyMapLayers.append(thisLayer);
  }
  if (keyMapLayers.length() < 1) {
    listPlus<KeyItem> thisLayer;
    thisLayer.pad(2 * nrowGpios * ncolGpios);
    keyMapLayers.append(thisLayer);
  }
}
static void parseLayout(um_fp layoutBuf = readFile("/lay.kml")) {
  if(!layoutBuf.ptr){
    Serial.println("layout not available, loading default layout");
    layoutBuf = defaultLayout_um;
  }
  um_fp keyboard =
      findKey(layoutBuf, (um_fp){.ptr = (char *)"keyboard", .width = 8});
  um_fp layersBuf =
      findKey(keyboard, (um_fp){.ptr = (char *)"layers", .width = 6});
  parseLayers(layersBuf);
  writeFile("/lay.kml", layoutBuf);
}
