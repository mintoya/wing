#ifdef __cplusplus
extern "C" {
#endif
#include "umap.h"

struct _umapListElement {
  unsigned int index; // position of string in Map reg
  unsigned int width;
};
UMap *UMap_new() {
  UMap *res = malloc(sizeof(UMap));
  *res = (UMap){.keyReg = $List(struct _umapListElement),
                .keys = $List(char),
                .valReg = $List(struct _umapListElement),
                .vals = $List(char)};
  return res;
}

um_fp UMap_getKeyAtIndex(UMap *map, unsigned int index) {
  struct _umapListElement *el = List_gst(map->keyReg, index);
  return (um_fp){.length = el->width,
                 .ptr = map->keys->head + el->index * sizeof(char)};
}
um_fp UMap_getValAtIndex(UMap *map, unsigned int index) {
  struct _umapListElement *el = List_gst(map->valReg, index);
  return (um_fp){.length = el->width,
                 .ptr = map->vals->head + el->index * sizeof(char)};
}
int keyCmp(um_fp a, um_fp b) {
  int res =
      (strncmp(a.ptr, b.ptr, (a.length < b.length) ? a.length : b.length));
  if (res)
    return res;
  return a.length - b.length;
}
int UMap_binarySearch(UMap *map, um_fp key) {
  int len = map->keyReg->length;
  if (len == 0) {
    return 0;
  }

  int left = 0;
  int right = len;

  while (left < right) {
    int mid = left + (right - left) / 2;
    um_fp midKey = UMap_getKeyAtIndex(map, mid);
    if (keyCmp(key, midKey) < 0) {
      left = mid + 1; // Search later part
    } else {
      right = mid; // Search earlier part
    }
  }

  return left;
}

int UMap_linearSearch(UMap *map, um_fp key) {
  if (map->keyReg->length == 0) {
    return 0;
  }
  int res = 0;
  um_fp current = UMap_getKeyAtIndex(map, res);
  while (keyCmp(key, current) < 0 && res < map->keyReg->length) {
    res++;
    current = UMap_getKeyAtIndex(map, res);
  }
  return res;
}
um_fp UMap_getValAtKey(UMap *map, um_fp key) {
  unsigned int index = UMap_binarySearch(map, key);
  if (!keyCmp(key, UMap_getKeyAtIndex(map, index))) {
    return UMap_getValAtIndex(map, UMap_binarySearch(map, key));
  } else {
    return nullUmf;
  }
}
void UMap_set(UMap *map, um_fp key, um_fp val) {
  unsigned int index = UMap_binarySearch(map, key);
  if (!keyCmp(key, UMap_getKeyAtIndex(map, index))) {
    *(struct _umapListElement *)List_gst(map->valReg, index) =
        (struct _umapListElement){.index = map->vals->length,
                                  .width = val.length};
    List_appendFromArr(map->vals, val.ptr, val.length);
  } else {
    struct _umapListElement keyElement = {.index = map->keys->length,
                                          .width = key.length};
    struct _umapListElement valElement = {.index = map->vals->length,
                                          .width = val.length};
    List_appendFromArr(map->keys, key.ptr, key.length);
    List_appendFromArr(map->vals, val.ptr, val.length);
    List_insert(map->keyReg, index, &keyElement);
    List_insert(map->valReg, index, &valElement);
  }
}

um_fp UMap_get(UMap *map, char *key) {
  return UMap_getValAtKey(map, um_fromStr(key));
}

void UMap_remake(UMap **map) {
  UMap *res = UMap_new();
  for (unsigned int i = 0; i < (*map)->keyReg->length; i++) {
    um_fp key = UMap_getKeyAtIndex(*map, i);
    um_fp val = UMap_getValAtIndex(*map, i);
    if (val.length > 1)
      UMap_set(res, key, val);
  }
  UMap_delete(*map);
  *map = res;
}
void UMap_delete(UMap *map) {
  List_delete((map)->keyReg);
  List_delete((map)->keys);
  List_delete((map)->valReg);
  List_delete((map)->vals);
  free(map);
}
UMap *UMap_fromBuf(um_fp mapRasterized) {
  UMap *res = UMap_new();
  unsigned int place = 0;
  while (place < mapRasterized.length) {
    um_fp key;
    um_fp val;

    memcpy(&key.length, mapRasterized.ptr + place, sizeof(unsigned int));
    place += sizeof(unsigned int);
    key.ptr = mapRasterized.ptr + place;
    place += key.length;

    memcpy(&val.length, mapRasterized.ptr + place, sizeof(unsigned int));
    place += sizeof(unsigned int);
    val.ptr = mapRasterized.ptr + place;
    place += val.length;
    UMap_set(res, key, val);
  }
  return res;
}
um_fp UMap_toBuf(UMap *map) {
  /*
   * stores binary data/probably shouldnt be printed
   * keyLength key valueLength value
   */
  List *data = $List(char);
  for (unsigned int i = 0; i < (map)->keyReg->length; i++) {
    // clang-format off
    um_fp key = UMap_getKeyAtIndex(map, i);
    um_fp val = UMap_getValAtIndex(map, i);
    List_appendFromArr(data,&key.length,sizeof(unsigned int));
    List_appendFromArr(data,key.ptr,key.length);
    List_appendFromArr(data,&val.length,sizeof(unsigned int));
    List_appendFromArr(data,val.ptr,val.length);
    // clang-format on
  }
  void *ptr = data->head;
  unsigned int length = data->length;
  free(data);
  return (um_fp){.length = length, .ptr = ptr};
}

#ifdef __cplusplus
}
#endif
