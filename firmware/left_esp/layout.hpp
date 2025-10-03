#pragma once
#include "fileSystemInterface.hpp"
#include "hid_keys.h"
#include "hid_keys_names.h"
#include "key.hpp"
#include "kml/kml.h"
#include "my-list/my-list.h"
#include "my-list/my-list.hpp"
#include "stdint.h"
#define um_Serial_Print(um) Serial.printf("%.*s\n", um.width, (char *)um.ptr)
um_fp defaultLayout_um = um_from(R"d(
  keyboard:{
    tapdances:[
      {
        taps:[ KEY_A, KEY_B, KEY_C, KEY_D ]
        holds:[ M( la ), M( ls ), KEY_C, KEY_D ]
      }
    ]
    functions:[
      printDebug,
      bootLoader,
      reset 
    ]
    layers:[
       [
          KEY_TAB , KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,        KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,  KEY_BACKSPACE,
          M( ls ) , KEY_A, KEY_S, KEY_D, KEY_F, KEY_G,        KEY_H, KEY_J, KEY_K, KEY_L, KEY_COMMA,  KEY_LEFTBRACE,
          M( lc ) , KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,        KEY_N, KEY_M, KEY_COMMA,KEY_DOT,KEY_SLASH,  KEY_ENTER,
          _       , _    ,M(la) ,L( 1 ),T( 0 ),     _,        _    , L( 1 ), L( 2 )  ,      _,        _,          _,
      ],
      [
        KEY_ESC, KEY_0,
      ]
    ]
  }
)d");

extern const unsigned int nrowGpios;
extern const unsigned int ncolGpios;

extern listPlus<listPlus<KeyItem>> keyMapLayers;
extern listPlus<tapDance> tapDances;

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
static void parseTapDances(um_fp tapDancesBuf) {
  um_fp tapdanceum;
  for (unsigned int i = 0; (tapdanceum = findIndex(tapDancesBuf, i)).ptr; i++) {
    tapDance t = {
        .pressActions = {0},
        .holdActions = {0},
        .state = 0,
        .currentCountDown = 0,
        .keystate = KeyState::HELDUP,
        .heldActionTriggered = false,
    };
    um_fp taps = findKey(tapdanceum, um_from("taps"));
    um_fp holds = findKey(tapdanceum, um_from("holds"));
    um_fp tap;
    um_fp hold;
    for (unsigned int ii = 0; (tap = findIndex(taps, ii)).ptr; ii++) {
      t.pressActions[ii] = kn_Match(tap);
    }
    for (unsigned int ii = 0; (hold = findIndex(holds, ii)).ptr; ii++) {
      t.holdActions[ii] = kn_Match(hold);
    }
    tapDances.append(t);
  }
}
static void parseLayout(um_fp layoutBuf = readFile("/lay.kml")) {
  if (!layoutBuf.ptr) {
    Serial.println("layout not available, loading default layout");
    layoutBuf = defaultLayout_um;
  }
  um_fp keyboard = findKey(layoutBuf, um_from("keyboard"));
  um_fp layersBuf = findKey(keyboard, um_from("layers"));
  um_fp tapDancesBuf = findKey(keyboard, um_from("tapdances"));
  parseLayers(layersBuf);
  parseTapDances(tapDancesBuf);
  writeFile("/lay.kml", layoutBuf);
}
