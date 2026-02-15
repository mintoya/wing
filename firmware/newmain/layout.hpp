#pragma once
#include "fileSystemInterface.hpp"
#include "hid_keys_names.h"
#include "key.hpp"
#include "my-lib/my-list.h"
#include "my-lib/mytypes.h"
#include "my-lib/vason.h"
#include "stdint.h"
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
#include "my-lib/print.h"
static void prettyPrintLayers() {
  println_("=== Keyboard Layers ===");
  println_("{} layers", keyMapLayers.len);
  for (slice(KeyItem) layer : keyMapLayers) {
    if (layer.ptr) {
      for (KeyItem key : layer)
        print_("{},", key);
      println_();
    } else
      println_("null layer?");
  }
  println_("=======================");
}
void addLayers(vason in) {
  auto layerCount = in.countChildren();
  if (in.tag() != vason_ARR || layerCount == 0)
    return;

  keyMapLayers = {
      .len = 0,
      .ptr = aCreate(stdAlloc, slice(KeyItem), layerCount)
  };
  println_("parsing {} layers", layerCount);

  for (usize i = 0; i < layerCount; i++) {
    auto layerVason = in[i];
    if (layerVason.tag() != vason_ARR) {
      println_(
          "parsing interrupted by non-array :\n"
          " it:({vason_container})\n"
          "parent:({vason_container})",
          layerVason,
          in
      );
      break;
    }
    auto thisLayerLen = layerVason.countChildren();
    println_("layer has {} keys", thisLayerLen);
    slice(KeyItem) thisLayer = {
        .len = thisLayerLen,
        .ptr = aCreate(stdAlloc, KeyItem, thisLayerLen)
    };
    for (usize k = 0; k < thisLayerLen; k++)
      thisLayer[k] = kn_Match(layerVason[k].asString());
    keyMapLayers[keyMapLayers.len++] = thisLayer;
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
