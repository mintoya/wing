#ifndef UMAP_H
#define UMAP_H
#ifdef __cplusplus
extern "C" {
#endif
#include "../list/list.h"
#include <stdlib.h>
#include <string.h>

typedef struct fat_pointer {
  void *ptr;
  unsigned int length;
} um_fp;
typedef struct umap {
  List *keyReg; // umapListElements
  List *keys;   // chars
  List *valReg; // umapListElements
  List *vals;   // chars
} UMap;

UMap *UMap_new();
um_fp UMap_getValAtKey(UMap *map, um_fp key);
um_fp UMap_getKeyAtIndex(UMap *map, unsigned int index);
um_fp UMap_get(UMap *map, char *key);
void UMap_set(UMap *map, um_fp key, um_fp val);
void UMap_remake(UMap **map);
void UMap_delete(UMap *map);

UMap *UMap_fromBuf(um_fp mapRasterized);
um_fp UMap_toBuf(UMap *map);

#define nullUmf ((um_fp){.ptr = NULL, .length = 0})
#define um_fromStr(str)                                                        \
  (um_fp) { str, strlen(str) }

#ifdef __cplusplus
}
#endif
#endif
