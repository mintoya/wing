#pragma once
#include "fileSystemInterface.hpp"
#include "hid_keys_names.h"
#include "key.hpp"
#include "my-lib/my-list.h"
#include "my-lib/mytypes.h"
#include "my-lib/vason.h"
#include "stdint.h"
extern void delay(usize);
auto defaultLayout_chars = R"d(
  {keyboard:{
    tapdances:[
      {
        taps:[ KEY_SPACE, KEY_B, KEY_C, KEY_D ];
        holds:[ M( la ), M( ls ), KEY_C, KEY_D ];
      }
    ];
    functions:[
      printDebug,
      bootLoader,
      reset 
    ];
    layers:[
       [
          KEY_TAB , KEY_Q, KEY_W  ,  KEY_E,  KEY_R, KEY_T,           KEY_Y,  KEY_U, KEY_I    ,  KEY_O,     KEY_P,  KEY_BACKSPACE,
          M( ls ) , KEY_A, KEY_S  ,  KEY_D,  KEY_F, KEY_G,           KEY_H,  KEY_J, KEY_K    ,  KEY_L, KEY_COMMA,  KEY_LEFTBRACE,
          M( lc ) , KEY_Z, KEY_X  ,  KEY_C,  KEY_V, KEY_B,           KEY_N,  KEY_M, KEY_COMMA,KEY_DOT, KEY_SLASH,  KEY_ENTER    ,
                  ,      ,  M(la) ,  L(1) ,  T(0) ,      ,                ,  L(1) ,  L(2)    ,       ,          ,               ,
      ],
      [    KEY_ESC, KEY_0, ]
    ];
  }}
)d";

extern const unsigned int nrowGpios;
extern const unsigned int ncolGpios;

extern slice(slice(KeyItem)) keyMapLayers;
extern slice(tapDance) tapDances;
#include "my-lib/print.h"
static void prettyPrintLayers() {
  println_("=== Keyboard Layers ===");
  // println_("{} layers", mList_len(keyMapLayers));
  for (slice(KeyItem) layer : keyMapLayers) {
    if (layer.ptr) {
      for (KeyItem key : layer)
        print_("{},", key);
      println_();
    } else {
      println_("null layer? exiting ");
      delay(10000);
      exit(1);
    }
  }
  println_("=======================");
}
void addLayers(vason in) {
  auto layerCount = in.countChildren();
  keyMapLayers = {.len = 0, .ptr = aCreate(stdAlloc, slice(KeyItem), layerCount)};
  if (in.tag() == vason_ARR && layerCount > 0)
    return;
  for (; keyMapLayers.len < layerCount; keyMapLayers.len++) {
    auto layerVason = in[keyMapLayers.len];
    auto thisLayerLen = layerVason.countChildren();
    slice(KeyItem) thisLayer = {.len = 0, .ptr = aCreate(stdAlloc, KeyItem, thisLayerLen)};
    if (layerVason.tag() != vason_ARR)
      break;
    else
      for (; thisLayer.len < thisLayerLen; thisLayer.len++)
        thisLayer[thisLayer.len] = kn_Match(layerVason[thisLayer.len].asString());
    keyMapLayers[keyMapLayers.len] = thisLayer;
  }
}
extern slice(tapDance) tapDances;
void addDances(vason in) {}
#include "my-lib/arenaAllocator.h"
static void parseLayout(fptr string = {}, vason parsed = {}) {
  Arena_scoped *local = arena_new_ext(stdAlloc, 512);
  fptr layoutBuf;
  if (!parsed.self.objects.ptr) {
    if (string.ptr)
      layoutBuf = string;
    else
      layoutBuf = fp(defaultLayout_chars);
    println_("parsing \n{}", layoutBuf);
    parsed = {parseStr(local, {layoutBuf.width, layoutBuf.ptr})};
  }

  println_("top( {vason_container} )", parsed.self);

  vason layers = parsed["keyboard"]["layers"];
  vason tapdances = parsed["keyboard"]["tapdances"];
  println_("layers   ( {vason_container} )", layers.self);
  println_("tapdances( {vason_container} )", tapdances.self);
  addLayers(layers);
  addDances(tapdances);

  println_("Layout parsed. Layers: {}, TapDances: {}", keyMapLayers.len, tapDances.len);

  writeFile("/lay.kml", layoutBuf);
}
