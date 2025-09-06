#ifndef STRING_LIST_H
#define STRING_LIST_H
#define initialBufferSize

#include "../my-list/my-list.h"
#include <string.h>

typedef struct stringMetaData {
  unsigned int index;
  unsigned int width;
  unsigned int _size;
} stringMetaData;

typedef struct {
  List List_char;
  List List_stringMetaData;
} stringList;

typedef struct fat_pointer {
  void *ptr;
  size_t width;
} um_fp;
int um_fp_cmp(um_fp a, um_fp b);
stringList *stringList_new();
um_fp stringList_get(stringList *l, unsigned int index);
void stringList_insert(stringList *l, um_fp, unsigned int index);
void stringList_set(stringList *l, um_fp, unsigned int index);
void stringList_append(stringList *l, um_fp);
stringList *stringList_remake(stringList *);
void stringList_free(stringList *l);
static inline unsigned int stringList_length(stringList *l) {
  return l->List_stringMetaData.length;
}
static inline char um_eq(um_fp a, um_fp b) {
  return a.width == b.width && !um_fp_cmp(a, b);
}
#define nullUmf ((um_fp){.ptr = NULL, .width = 0})
#define um_fromExplicit(type, val)                                             \
  ((um_fp){.ptr = (type[1]){type}, .width = sizeof(type)})
#define um_from(val)                                                           \
  _Generic((val),                                                              \
      char *: (um_fp){.ptr = (val), .width = strlen(val)},                     \
      const char *: (um_fp){.ptr = (val), .width = strlen(val)},               \
      default: (um_fp){.ptr = (typeof(val)[1]){val},                           \
                       .width = sizeof(typeof(val))}) // structs

#endif // STRING_LIST_H


#ifdef STRING_LIST_C

#include <stdlib.h>
#include <string.h>
#define STRING_LIST_minSize 20
stringList *stringList_new() {
  stringList *res = malloc(sizeof(stringList));
  // clang-format off
  *res = (stringList) {
    .List_stringMetaData = {
        .width = sizeof(stringMetaData),
        .length = 0,
        .size = 1,
        .head = malloc(sizeof(stringMetaData)),
    },

    .List_char = {
      .width = sizeof(char),
      .length = 0,
      .size = 1,
      .head = malloc(sizeof(char)),
    }
  };
  // clang-format on
  return res;
}
#define max(a, b) ((a < b) ? (b) : (a))

int um_fp_cmp(um_fp a, um_fp b) {
  if (a.width != b.width) {
    return (a.width < b.width) ? -1 : 1;
  }
  if (a.width == 0) {
    return 0;
  }
  return memcmp(a.ptr, b.ptr, a.width);
}
um_fp stringList_get(stringList *l, unsigned int index) {
  if (index > l->List_stringMetaData.length)
    return nullUmf;
  stringMetaData this =
      mList_get(&(l->List_stringMetaData), stringMetaData, index);
  return ((um_fp){.ptr = (List_gst(&(l->List_char), this.index)),
                  .width = this.width});
}
stringList *stringList_remake(stringList *origional) {
  stringList *res = stringList_new();
  for (unsigned int i = 0; i < stringList_length(origional); i++) {
    stringList_append(res, stringList_get(origional, i));
  }
  return res;
}
void stringList_append(stringList *l, um_fp value) {
  unsigned int trueSize = max(value.width, STRING_LIST_minSize);
  stringMetaData this = {
      .index = l->List_char.length,
      .width = value.width,
      ._size = trueSize,
  };
  mList_add(&(l->List_stringMetaData), this);
  List_appendFromArr(&(l->List_char), value.ptr, value.width);
  List_pad(&(l->List_char), trueSize - value.width);
}
void stringList_insert(stringList *l, um_fp value, unsigned int index) {
  unsigned int trueSize = max(value.width, STRING_LIST_minSize);
  stringMetaData this = {
      .index = l->List_char.length,
      .width = value.width,
      ._size = trueSize,
  };
  mList_insert(&(l->List_stringMetaData), this, index);
  List_appendFromArr(&(l->List_char), value.ptr, value.width);
  List_pad(&(l->List_char), trueSize - value.width);
}
void stringList_set(stringList *l, um_fp value, unsigned int index) {
  stringMetaData this =
      mList_get(&(l->List_stringMetaData), stringMetaData, index);
  if (this._size < value.width) {
    stringMetaData *ref = List_gst(&(l->List_stringMetaData), index);
    ref->width = value.width;
    ref->_size = value.width;
    ref->index = l->List_char.length;
    List_appendFromArr(&(l->List_char), value.ptr, value.width);
  } else {
    stringMetaData *ref = List_gst(&(l->List_stringMetaData), index);
    ref->width = value.width;
    memcpy(List_gst(&(l->List_char), ref->index), value.ptr, value.width);
  }
}

void stringList_free(stringList *l) {
  free(l->List_char.head);
  free(l->List_stringMetaData.head);
  free(l);
}
#endif // STRING_LIST_C
