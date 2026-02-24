#pragma once
#include "fileSystemInterface.hpp"
#include "hid_keys_names.h"
#include "key.hpp"
#include "my-lib/my-list.h"
#include "my-lib/mytypes.h"
#include "my-lib/vason.h"
#include "stdint.h"
extern const unsigned int nrowGpios;
extern const unsigned int ncolGpios;
using namespace KeyItem_ititiazizers;
// clang-format off
KeyItem default_layers[3][48] = {
  {
      K("ESC"), K('Q'),  K('W'),  K('E'),    K('R'), K('T'),    K('Y'),  K('U'), K('I'), K('O'), K('P'),   K('['),
       M("LS"), K('A'),  K('S'),  K('D'),    K('F'), K('G'),    K('H'),  K('J'), K('K'), K('L'), K(';'),  K('\''),
       M("LC"), K('Z'),  K('X'),  K('C'),    K('V'), K('B'),    K('N'),  K('M'), K(','), K('.'), K('/'), K("ENT"),
            {},     {}, M("LA"),    L(1),  K("SPC"),     {},        {},    L(2),   L(1),M("LM"),     {},   {},
  },
  {
  K("TAB"),K('1'),  K("2"),    K('3'), K('4'), K('5'),    K('6'),   K('7'),  K('8'),  K('9'),  K('0'),    K(']'),
        {},    {},      {},        {},     {},     {},        {},       {},      {},      {},      {},        {},
        {},    {},      {},        {},     {},     {},        {},   K("-"),  K("="),      {}, K('\\'),  K("BKS"),
  },
  {
        {},KEY_F1,  KEY_F2,    KEY_F3, KEY_F4, KEY_F5,    KEY_F6,   KEY_F7,  KEY_F8,   KEY_F9, KEY_F10,  KEY_F11 ,
        {},    {},      {},        {},     {},     {},        {},       {},  KEY_UP,       {},      {},        {},
        {},    {},      {},        {},     {},     {},        {}, KEY_LEFT,KEY_DOWN,KEY_RIGHT,      {},        {},
  }
};
tapDance default_tapDances[] = {
  {
    .pressActions = {K("TAB")},
    .holdActions = {K("ESC")}
  }
};

// clang-format on
static char defaultLayout_chars[] = {};

extern slice(slice(KeyItem)) keyMapLayers;

#include "my-lib/print.h"
static void prettyPrintLayers() {
  println_("=== Keyboard Layers ===");
  println_("{} layers", keyMapLayers.len);
  for (slice(KeyItem) layer : keyMapLayers) {
    if (layer.ptr) {
      for (auto i = 0; i < layer.len; i++) {
        i % (ncolGpios * 2)
            ? print_(" {},", layer[i])
            : print_("\n\t{}", layer[i]);
      }
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
  keyMapLayers = {
      countof(default_layers),
      aCreate(stdAlloc, slice(KeyItem), countof(default_layers))
  };
  for (int i = 0; i < keyMapLayers.len; i++)
    keyMapLayers[i] = {
        countof(default_layers[i]),
        default_layers[i]
    };
  tapDances = {
      countof(default_tapDances),
      aCreate(stdAlloc, tapDance, countof(default_tapDances))
  };
  for (auto i = 0; i < tapDances.len; i++)
    tapDances[i] = {
        default_tapDances[i]
    };
  // Arena_scoped *local = arena_new_ext(stdAlloc, 512);
  // fptr layoutBuf;
  // if (!parsed.self.objects.ptr) {
  //   if (string.ptr)
  //     layoutBuf = string;
  //   else
  //     layoutBuf = fp(defaultLayout_chars);
  //   println_("parsing \n{}", layoutBuf);
  //   parsed = {parseStr(local, {layoutBuf.width, layoutBuf.ptr})};
  // }
  //
  // println_("top( {vason_container} )", parsed.self);
  //
  // vason layers = parsed["keyboard"]["layers"];
  // vason tapdances = parsed["keyboard"]["tapdances"];
  // println_("layers   ( {vason_container} )", layers.self);
  // println_("tapdances( {vason_container} )", tapdances.self);
  // addLayers(layers);
  // addDances(tapdances);
  //
  // println_("Layout parsed. Layers: {}, TapDances: {}", keyMapLayers.len, tapDances.len);
  // writeFile("/lay.kml", layoutBuf);
}
