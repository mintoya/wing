#pragma once
#include "fileSystemInterface.hpp"
#include "hid_keys_names.h"
#include "key.hpp"
#include "my-lib/my-list.h"
#include "my-lib/types.h"
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

extern mList(mList(KeyItem)) keyMapLayers;
extern mList(tapDance) tapDances;
#include "my-lib/print.h"
static void prettyPrintLayers() {
  println_("=== Keyboard Layers ===");
  // println_("{} layers", mList_len(keyMapLayers));
  for (auto lI = 0; lI < mList_len(keyMapLayers); lI++) {
    println_("layer {}", lI);
    mList(KeyItem) layer = *mList_get(keyMapLayers, lI);
    if (layer) {
      println_("layer {}", lI);
      for (auto i = 0; i < mList_len(layer); i++)
        print_("{},", *mList_get(layer, i));
      println_();
    } else {
      println_("null layer? exiting ");
      delay(1000);
      exit(1);
    }
  }
  println_("=======================");
}
extern mList(mList(KeyItem)) keyMapLayers;
void addLayers(vason in) {
  if (in.tag() == vason_ARR && in.countChildren() > 0) {
    auto layerCount = in.countChildren();
    for (auto layerIdx = 0; layerIdx < layerCount; layerIdx++) {
      mList(KeyItem) layerList = mList_init(stdAlloc, KeyItem);
      mList_push(keyMapLayers, layerList);
      auto layerVason = in[layerIdx];
      for (auto i = 0; i < nrowGpios * ncolGpios * 2; i++)
        mList_push(layerList, kn_Match(in[i].asString()));
    }
  } else {
    mList(KeyItem) stub = mList_init(stdAlloc, KeyItem);
    mList_push(keyMapLayers, stub);
    for (auto i = 0; i < nrowGpios * ncolGpios * 2; i++)
      mList_push(stub, KeyItem{});
  }
}
extern mList(tapDance) tapDances;
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

  println_("Layout parsed. Layers: {}, TapDances: {}", mList_len(keyMapLayers),
           mList_len(tapDances));

  writeFile("/lay.kml", layoutBuf);
}
