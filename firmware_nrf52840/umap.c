#include "list.h"
#include <cstddef>
#include <cstring>
#include <stdlib.h>
#include <string.h>
struct _umapListElement {
  unsigned int index;
  unsigned int width;
};
typedef struct umap {
  List *keyReg;
  List *keys;
  List *valReg;
  List *vals;
} UMap;

UMap *UMap_new() {
  UMap *res = malloc(sizeof(UMap));
  *res = (UMap){.keyReg = $List(struct _umapListElement),
                .keys = $List(char),
                .valReg = $List(struct _umapListElement),
                .vals = $List(char)};
  return res;
}
typedef struct fat_pointer {
  void *ptr;
  unsigned int length;
} um_fp;

um_fp UMap_getKeyAtIndex(UMap *map, unsigned int index) {
  struct _umapListElement *el = List_gst(map->keyReg, index);
  return (um_fp){.length = el->width,
                 .ptr = map->keyReg->head + el->index * sizeof(char)};
}
// um_fp UMap_getValAtIndex(UMap *map, unsigned int index) {
//   struct _umapListElement *el = List_gst(map->valReg, index);
//   return (um_fp){.length = el->width,
//                  .ptr = map->vals->head + el->index * sizeof(char)};
// }
unsigned int UMap_binarySearch(UMap *map, um_fp key) {
  unsigned int res = map->keyReg->length / 2;
  unsigned int cs = res / 2;

  um_fp current = UMap_getKeyAtIndex(map, res);
  int cmp =
      strncmp(current.ptr, key.ptr,
              (key.length < current.length) ? (key.length) : (current.length));
  while(!cmp)
}
void UMap_set(um_fp key, um_fp val) {}
