// LIST_5d3bd19_C needs to be defined for the inplementation

#ifndef LIST_5d3bd19_H
#define LIST_5d3bd19_H
#ifdef __cplusplus
extern "C" {
#endif

#ifdef useMalloc_LIST
#include <stdlib.h>
#define clearAllocate(length, size) calloc(length, size)
#define regularAllocate(size) malloc(size)
#define reAllocate(source, newsize) realloc(source, newSize)
#define freAllocate(ptr) free(ptr)
#else
#include "../alloc.h"
#endif

typedef struct List {
  unsigned long width;
  unsigned int length;
  unsigned int size;
  char *head;
} List;

List *List_new(unsigned long bytes);
void List_set(List *l, unsigned int i, const void *element);
void *List_gst(const List *l, unsigned int i);
void List_resize(List *l, unsigned int newSize);
void List_append(List *l, const void *element);
void List_pad(List *l, unsigned int ammount);
List *List_fromArr(const void *source, unsigned int size, unsigned int length);
void List_appendFromArr(List *l, const void *source, unsigned int i);
int List_search(List *l, const void *value);
void List_insert(List *l, unsigned int i, void *element);
void List_remove(List *l, unsigned int i);
void List_zeroOut(List *l);
void List_free(List *l);
void List_print(const List *l);
int List_filter(List *l, int (*function)(void *));
void *List_toBuffer(List *l);
/*
 * fromBuffer is the inverse of toBuffer
 * cannot be list_freed
 * does not allocate memory
 */
void *List_fromBuffer(void *ref);
List *List_deepCopy(List *l);
List *List_combine(List *l, List *l2);
#define mList_forEach(list, type, item, scope)                                 \
  {                                                                            \
    type item;                                                                 \
    for (unsigned int _i = 0; _i < (list)->length; _i++) {                     \
      item = *((type *)List_gst((list), _i));                                  \
      scope                                                                    \
    }                                                                          \
  }

#define mList_get(list, type, index) *(type *)List_gst(list, index)
#define mList_add(list, value) List_append(list, (typeof(value)[1]){value})
#define mList_insert(list, value, index)                                       \
  List_insert(list, index, (typeof(value)[1]){value})

// Actual implementations
#define mList_fromType(type) List_new(sizeof(type))
#define mList_fromList(type, array)                                            \
  List_fromArr(array, sizeof(type), sizeof(array) / sizeof(type))

// Macro to select correct macro based on argument count
#define _GET_MLIST_MACRO(_1, _2, NAME, ...) NAME

// Dispatcher macro
#define mList(...)                                                             \
  _GET_MLIST_MACRO(__VA_ARGS__, mList_fromList, mList_fromType)(__VA_ARGS__)

#endif
#ifdef LIST_5d3bd19_C

#ifdef DEBUGLISTS
#define errprint(str)                                                          \
  printf("%s\n", str);                                                         \
  exit();
int checkBounds(List *l, unsigned int i) {
  /*printf("chekcing %i, length: %i size: %i\n",i,l->length,l->size);*/
  int inBounds = 1;
  if (i >= l->length) {
    if (i > l->size) {
      errPrint("Out of bounds\n");
      inBounds = 0;
    } else {
      warnPrint("Out of range\n");
    }
  }
  return inBounds;
}
#else
#define checkBounds(a, b) 1
#define errPrint(str) 1
#endif

List *List_new(unsigned long bytes) {
  List *l = (List *)clearAllocate(1, sizeof(List));
  l->width = bytes;
  l->length = 0;
  l->head = clearAllocate(1, bytes);
  l->size = 1;
  return l;
}
#define LIST_GROW_EQ(uint) ((uint + (uint << 1)) + 1)

//
// prins  hex representatins of list data
//
void List_print(const List *l) {
  printf("{");
  for (int i = 0; i < l->length; i++) {
    const char *refw = List_gst(l, i);
    printf("0x");
    for (int ii = l->width / sizeof(char) - 1; ii >= 0; ii--) {
      printf("%02x", *((unsigned char *)refw + ii));
    }
    printf(", ");
  }
  printf("}\n");
}
void List_free(List *l) {
  freAllocate(l->head);
  freAllocate(l);
}
void List_resize(List *l, unsigned int newSize) {
  if (newSize < 1) {
    newSize = 1;
  }

  void *newPlace = reAllocate(l->head, newSize * l->width);
  if (newPlace == NULL) {
    freAllocate(l->head);
    errPrint("ran out of space for some realloc, in List_resize\n");
  }
  l->head = newPlace;
  l->size = newSize;
  l->length = (l->length < l->size) ? (l->length) : (l->size);
}
// expands list to
void *List_gst(const List *l, unsigned int i) {
  checkBounds(l, i);
  return l->head + i * l->width;
}
void List_zeroOut(List *l) {
  for (int i = 0; i < l->size; i++) {
    memset(List_gst(l, i), 0, l->width);
  }
}
void List_append(List *l, const void *element) {
  if (l->length + 1 >= l->size) {
    unsigned int nl = l->length + (l->length >> 1) + 1;
    // unsigned int nl = 2 * l->length;
    List_resize(l, nl);
  }
  memcpy(l->head + l->width * l->length, element, l->width);
  l->length++;
}
void List_pad(List *l, unsigned int ammount) {
  List_resize(l, l->length + ammount);
  // memset(l->head+l->width*l->length, 0, ammount*l->width);
  l->length += ammount;
}
void List_remove(List *l, unsigned int i) {
  if (checkBounds(l, i)) {
    memmove(l->head + i * l->width, l->head + (i + 1) * l->width,
            (l->length - i) * l->width);
    l->length--;
  }
}

void List_set(List *l, unsigned int i, const void *element) {
  checkBounds(l, i);
  memcpy(l->head + i * l->size, element, l->width);
}

void List_insert(List *l, unsigned int i, void *element) {

  if (i == l->length) {
    List_append(l, element);
    return;
  }

  if (l->length + 1 > l->size) {
    List_resize(l, l->size * 2);
  }

  memmove((char *)l->head + (i + 1) * l->width, (char *)l->head + i * l->width,
          l->width * (l->length - i));

  memcpy((char *)l->head + i * l->width, element, l->width);

  l->length++;
}
int List_forEach(List *l, int (*function)(void *)) {
  int result = 1;
  for (int i = 0; i < l->length; i++) {
    result = result && function(List_gst(l, i));
  }
  return result;
}
List *List_fromArr(const void *source, unsigned int size, unsigned int length) {
  // helper function for turning arrays into lists
  List *l = List_new(size);
  memcpy(l->head + l->length * l->width, source, length * l->width);
  l->length = length;
  return l;
}
int List_filter(List *l, int (*function)(void *)) {
  int totalRemoved = 0;
  for (int i = 0; i < l->length; i++) {
    if (!function(List_gst(l, i))) {
      List_remove(l, i);
      totalRemoved++;
      i--;
    }
  }
  return totalRemoved;
}
int List_search(List *l, const void *value) {
  // search entire list for value
  for (size_t i = 0; i < l->length; i++) {
    if (!memcmp(value, List_gst(l, i), l->width)) {
      return i;
    }
  }
  return -1;
}
void List_appendFromArr(List *l, const void *source, unsigned int size) {
  if (l->size < l->length + size) {
    List_resize(l, l->length + size + 1);
  }
  memcpy(l->head + l->length * l->width, source, size * l->width);
  l->length += size;
  // for (unsigned int i = 0; i < index; i++) {
  //   List_append(l, source + (l->width * i));
  // }
}
// buffer containing list and its data (head is meaningles)
void *List_toBuffer(List *l) {
  void *res = regularAllocate(sizeof(List) + l->length * l->width);
  memcpy(res + sizeof(List), l->head, l->length * l->width);
  *(List *)res = *l;
  return res;
}
void *List_fromBuffer(void *buffer) {
  List *l = buffer;
  l->head = buffer + sizeof(List);
  return l;
}
List *List_deepCopy(List *l) {
  List *res = regularAllocate(sizeof(List));
  *res = *l;
  res->size = l->length * l->width;
  res->head = regularAllocate(l->length * l->width);
  memcpy(res->head, l->head, l->length * l->width);
  return res;
}
List *List_combine(List *l, List *l2) {
  List *l3 = List_fromArr(l->head, l->size, l->length);
  List_appendFromArr(l3, l2->head, l2->length);
  return l3;
}
#endif
#ifdef __cplusplus
}
#endif
