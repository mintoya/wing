#pragma once
#include "fileSystemInterface.hpp"
#include "hid_keys_names.h"
#include "key.hpp"
#include "my-lib/my-list.h"
#include "my-lib/mytypes.h"
#include "my-lib/shortList.h"
#include "my-lib/vason_arr.h"
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
          {},    {},      {},        {},     {},     {},        {},       {},      {},      {},      {},    K('`'),
          {},    {},      {},        {},     {},     {},        {},   K('-'),  K('='),      {}, K('\\'),  K("BKS"),
  },
  {
        {},KEY_F1,  KEY_F2,    KEY_F3, KEY_F4, KEY_F5,    KEY_F6,   KEY_F7,   KEY_F8,    KEY_F9, KEY_F1 ,  KEY_F11 ,
        {},    {},      {},        {},     {},     {},        {},       {},  K("UP"),        {},      {},        {},
        {},    {},      {},        {},     {},     {},        {},K("LEFT"),K("DOWN"),K("RIGHT"),      {},        {},
  }
};
tapDance default_tapDances[] = {
  {
    .pressActions = {K("TAB")},
    .holdActions = {K("ESC")}
  }
};

// clang-format on
static c8 defaultLayout_chars[] =
    R"d(
{ 
    keyboard:{
      tapdances:{
        { taps:{ K:A }, holds:{M:la , M:ls }, }
        { taps:{ K:B }, holds:{M:lc , M:rs }, }
      },
      layers:{ 
        { 
          K:ESC,  K:Q,  K:W, K:E,   K:R, K:T,        K:Y, K:U,   K:I,  K:O,   K:P,  K:"[",
           M:ls,  K:A,  K:S, K:D,   K:F, K:G,        K:H, K:J,   K:K,  K:L, K:";",  K:"'", 
           M:lc,  K:Z,  K:X, K:C,   K:V, K:B,        K:N, K:M, K:",",  K:.,  K:\/,  K:ENT,
               ,     , M:la, L:1, K:SPC,    ,           , L:2,   L:1, M:lm,      ,    
        },
        {
          K:TAB,K:1,  K:2, K:3, K:4, K:5,     K:6, K:7, K:8, K:9,  K:0, K:"]",
               ,   ,     ,    ,    , K:5,        ,    ,    ,    ,     ,   K:`,
               ,   ,     ,    ,    ,    ,        , K:-, K:=,    , K:\\, K:BKS
        },
        {
                , K:F1, K:F2, K:F3, K:F4, K:F5,    K:F6,   K:F7,   K:F8,    K:F9, K:F10,  K:F11,
                ,     ,     ,     ,     ,     ,        ,       ,   K:UP,        ,      ,       ,
                ,     ,     ,     ,     ,     ,        , K:LEFT, K:DOWN, K:RIGHT,      ,        
        },
      },
    },
}
)d";

#include "my-lib/print.h"

extern slice(slice(KeyItem)) keyMapLayers;
extern slice(tapDance) tapDances;
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
KeyItem KeyItem_fromVaso(vason in) {
  switch (in.tag()) {
    case vason_STRING: {
      return K(in.asString());
    } break;
    case vason_PAIR: {
      fptr kind = in[0].asString();
      fptr specific = in[1].asString();
      if (kind.width == 1 && specific.width) {
        return kind.ptr[0] == 'K'
                   ? K(specific)
               : kind.ptr[0] == 'T'
                   ? T((u8)um_asUint(specific))
               : kind.ptr[0] == 'L'
                   ? L((u8)um_asUint(specific))
               : kind.ptr[0] == 'M'
                   ? M(specific)
                   : KeyItem{};
      } else
        return KeyItem{};
    } break;
    case vason_TABLE:
    case vason_INVALID:
    case vason_UNPARSED:
      return KeyItem{};
      break;
  }
}
void addLayers(vason in) {
  slice(KeyItem) *keyLayersList = msList_init(stdAlloc, slice(KeyItem));
  for (usize klCount = 0; klCount < in.countChildren(); klCount++) {
    KeyItem *keyLayer = msList_init(stdAlloc, KeyItem);
    vason thisLayer = in[klCount];
    for (usize kCount = 0; kCount < thisLayer.countChildren(); kCount++)
      msList_push(stdAlloc, keyLayer, KeyItem_fromVaso(thisLayer[kCount]));
    msList_push(
        stdAlloc,
        keyLayersList,
        (slice(KeyItem))slice_stat(*msList_vla(keyLayer))
    );
  }
  keyMapLayers = (slice(slice(KeyItem)))slice_stat(*msList_vla(keyLayersList));
}

void addDances(vason in) {}
#include "my-lib/arenaAllocator.h"
static void parseLayout(fptr string = {}, vason parsed = {nullptr, 0}) {
  if (string.ptr || parsed) {
    vason_container c;
    My_allocator *local = arena_new_ext(stdAlloc, 1024);
    defer { arena_cleanup(local); };

    if (string.ptr) {
      c = vason_parseString(local, slice(c8){string.width, string.ptr});
      parsed = {c};
    }
    parsed = parsed["keyboard"];
    println_("{vason_container}",(vason_container)parsed)
    addLayers(parsed["layers"]);
    addDances(parsed["tapDances"]);
  } else {
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
  }
}
