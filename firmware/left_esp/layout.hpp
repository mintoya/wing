#pragma once
#include "../my-lib/my-list.h"
#include "../my-lib/vason.h"
#include "fileSystemInterface.hpp"
#include "hid_keys.h"
#include "hid_keys_names.h"
#include "key.hpp"
#include "kml/kml.h"
#include "stdint.h"
#define um_Serial_Print(um) Serial.printf("%.*s\n", um.width, (char *)um.ptr)
fptr defaultLayout_um = fp(R"d(
  keyboard:{
    tapdances:[
      {
        taps:[ KEY_SPACE, KEY_B, KEY_C, KEY_D ]
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
          KEY_TAB , KEY_Q, KEY_W  ,  KEY_E,  KEY_R, KEY_T,           KEY_Y,  KEY_U, KEY_Iv   ,  KEY_O,     KEY_P,  KEY_BACKSPACE,
          M( ls ) , KEY_A, KEY_S  ,  KEY_D,  KEY_F, KEY_G,           KEY_H,  KEY_J, KEY_Kv   ,  KEY_L, KEY_COMMA,  KEY_LEFTBRACE,
          M( lc ) , KEY_Z, KEY_X  ,  KEY_C,  KEY_V, KEY_B,           KEY_N,  KEY_M, KEY_COMMA,KEY_DOT, KEY_SLASH,  KEY_ENTER    ,
                  ,      ,  M(la) ,  L(1) ,  T(0) ,      ,                ,  L(1) ,  L(2)    ,       ,          ,               ,
      ],
      [    KEY_ESC, KEY_0, ]
    ]
  }
)d");

extern const unsigned int nrowGpios;
extern const unsigned int ncolGpios;

extern mList(mList(KeyItem)) keyMapLayers;
extern mList(tapDance) tapDances;

static void prettyPrintLayers() {
  // Print header
  Serial.println("=== Keyboard Layers ===");

  // Iterate through each layer
  for (int layerIndex = 0; layerIndex < mList_len(keyMapLayers); layerIndex++) {
    Serial.print("Layer ");
    Serial.print(layerIndex);
    Serial.print(" (");
    Serial.print(mList_len(*mList_get(keyMapLayers, layerIndex)));
    Serial.println(" keys):");

    // Get the current layer
    mList(KeyItem) currentLayer = *mList_get(keyMapLayers, layerIndex);

    // Print each key in the layer
    for (int keyIndex = 0; keyIndex < mList_len(currentLayer); keyIndex++) {
      Serial.print("  Key ");
      Serial.print(keyIndex);
      Serial.print(": ");

      // Print the individual key item
      KeyItem k = mList_getOr(currentLayer, keyIndex, (KeyItem){});

      printKeyItem(k); // Assuming this function exists
    }
    Serial.println(); // Empty line between layers
  }
  Serial.println("=======================");
}

// static void parseLayers(fptr layersBuf) {
//   ((List *)keyMapLayers)->length = 0;
//
//   fptr thisLayerBuf;
//
//   for (unsigned int i = 0;; i++) {
//     thisLayerBuf = findIndex(layersBuf, i);
//
//     if (thisLayerBuf.ptr == nullptr)
//       break;
//
//     mList(KeyItem) thisLayer(nullptr);
//     if (keyMapLayers.length() > i) {
//       thisLayer = keyMapLayers.get(i);
//     } else {
//       thisLayer = mList(KeyItem)();
//     }
//     thisLayer.pad(2 * nrowGpios * ncolGpios); // force fill with enough keys
//     thisLayer.clear();
//
//     fptr thisKeyBuf;
//     for (unsigned int ii = 0; ii < 2 * nrowGpios * ncolGpios; ii++) {
//       thisKeyBuf = findIndex(thisLayerBuf, ii);
//       thisLayer.append(KeyItem(kn_Match(thisKeyBuf))); // can handle null
//     }
//     keyMapLayers.append(thisLayer);
//   }
//   if (keyMapLayers.length() < 1) {
//     mList(KeyItem) thisLayer;
//     thisLayer.pad(2 * nrowGpios * ncolGpios);
//     keyMapLayers.append(thisLayer);
//   }
// }
// static void parseTapDances(fptr tapDancesBuf) {
//   fptr tapdanceum;
//   for (unsigned int i = 0; (tapdanceum = findIndex(tapDancesBuf, i)).ptr;
//   i++) {
//     tapDance t = {
//         .pressActions = {0},
//         .holdActions = {0},
//         .state = 0,
//         .currentCountDown = 0,
//         .keystate = KeyState::HELDUP,
//         .heldActionTriggered = false,
//     };
//     fptr taps = findKey(tapdanceum, fp("taps"));
//     fptr holds = findKey(tapdanceum, fp("holds"));
//     fptr tap;
//     fptr hold;
//     for (unsigned int ii = 0; (tap = findIndex(taps, ii)).ptr; ii++) {
//       t.pressActions[ii] = kn_Match(tap);
//     }
//     for (unsigned int ii = 0; (hold = findIndex(holds, ii)).ptr; ii++) {
//       t.holdActions[ii] = kn_Match(hold);
//     }
//     tapDances.append(t);
//   }
// }
// static void parseLayout(fptr layoutBuf = readFile("/lay.kml")) {
//   if (!layoutBuf.ptr) {
//     Serial.println("layout not available, loading default layout");
//     layoutBuf = defaultLayout_um;
//   }
//   fptr keyboard = findKey(layoutBuf, fp("keyboard"));
//   fptr layersBuf = findKey(keyboard, fp("layers"));
//   fptr tapDancesBuf = findKey(keyboard, fp("tapdances"));
//   parseLayers(layersBuf);
//   parseTapDances(tapDancesBuf);
//   Serial.printf("layoutBuf.ptr=%p, width=%u\n", layoutBuf.ptr,
//   layoutBuf.width); delay(100);
//
//   writeFile("/lay.kml", layoutBuf);
//   Serial.printf("write finished");
// }

// adapted old thingamajigger with Gemini 3

// Helper to get a fptr from a vason_object (STR or ID)
fptr vason_to_fptr(vason_container &c, vason_object &obj) {
  switch (obj.tag) {
  case vason_STR:
  case vason_ID:
    return {obj.span.len, c.string.ptr + obj.span.offset};
  default:
    return nullFptr;
  }
}

// Helper to find a key in a vason MAP
vason_object vason_find_key(vason_container &c, vason_object &mapObj,
                            fptr keyName) {
  if (mapObj.tag != vason_MAP)
    return {.tag = vason_INVALID};

  // Maps contain pairs in the 'objects' slice
  vason_object *entries = c.objects.ptr + mapObj.span.offset;
  for (u32 i = 0; i < mapObj.span.len; i += 2) {
    vason_object &keyObj = entries[i];
    if (vason_to_fptr(c, keyObj) == keyName) {
      return entries[i + 1];
    }
  }
  return {.tag = vason_INVALID};
}
// Helper to get an index from a vason ARR or MAP
vason_object vason_at(vason_container &c, vason_object &parent, u32 index) {
  if (index >= parent.span.len)
    return {.tag = vason_INVALID};
  return c.objects.ptr[parent.span.offset + index];
}
static void parseLayers(vason_container &c, vason_object layersObj) {
  mList_clear(keyMapLayers);
  u32 layerCount = layersObj.span.len;

  for (u32 i = 0; i < layerCount; i++) {
    vason_object thisLayerObj = vason_at(c, layersObj, i);

    // Initialize/Get the mList for this layer
    mList(KeyItem) thisLayer = mList_init(stdAlloc, KeyItem);
    u32 keyCount = 2 * nrowGpios * ncolGpios;

    for (u32 ii = 0; ii < keyCount; ii++) {
      vason_object keyObj = vason_at(c, thisLayerObj, ii);
      // kn_Match likely takes a fptr, so we convert the vason object
      mList_push(thisLayer, KeyItem(kn_Match(vason_to_fptr(c, keyObj))));
    }
    mList_push(keyMapLayers, thisLayer);
  }

  // Ensure at least one layer exists
  if (mList_len(keyMapLayers) < 1) {
    mList(KeyItem) emptyLayer = mList_init(stdAlloc, KeyItem);
    // Pad logic...
    mList_push(keyMapLayers, emptyLayer);
  }
}
static void parseTapDances(vason_container &c, vason_object tdBufObj) {
  mList_clear(tapDances);
  u32 tdCount = tdBufObj.span.len;

  for (u32 i = 0; i < tdCount; i++) {
    vason_object tdItem = vason_at(c, tdBufObj, i);
    tapDance t = {
        .state = 0,
        .currentCountDown = 0,
        .keystate = KeyState::HELDUP,
        .heldActionTriggered = false,
    };

    vason_object tapsObj = vason_find_key(c, tdItem, fp("taps"));
    vason_object holdsObj = vason_find_key(c, tdItem, fp("holds"));

    // Fill Press Actions
    for (u32 ii = 0; ii < tapsObj.span.len && ii < 10; ii++) {
      vason_object act = vason_at(c, tapsObj, ii);
      t.pressActions[ii] = kn_Match(vason_to_fptr(c, act));
    }
    // Fill Hold Actions
    for (u32 ii = 0; ii < holdsObj.span.len && ii < 10; ii++) {
      vason_object act = vason_at(c, holdsObj, ii);
      t.holdActions[ii] = kn_Match(vason_to_fptr(c, act));
    }

    mList_push(tapDances, t);
  }
}
static void parseLayout(fptr layoutBuf = readFile("/lay.kml")) {
  if (!layoutBuf.ptr) {
    Serial.println("layout not available, loading default layout");
    layoutBuf = defaultLayout_um;
  }

  // Step 1: Parse the string into the container structure
  vason_container parsed = parseStr(stdAlloc, {layoutBuf.width, layoutBuf.ptr});

  // Step 2: Navigate to the "keyboard" root object
  vason_object keyboard = vason_find_key(parsed, parsed.top, fp("keyboard"));

  // Step 3: Extract the "layers" and "tapdances"
  vason_object layersBuf = vason_find_key(parsed, keyboard, fp("layers"));
  vason_object tapDancesBuf = vason_find_key(parsed, keyboard, fp("tapdances"));

  // Step 4: Run specialized parsers
  parseLayers(parsed, layersBuf);
  parseTapDances(parsed, tapDancesBuf);

  Serial.printf("Layout parsed. Layers: %d, TapDances: %d\n",
                mList_len(keyMapLayers), mList_len(tapDances));

  // Note: Writing back layoutBuf assumes it's still valid/modified elsewhere
  writeFile("/lay.kml", layoutBuf);
}
