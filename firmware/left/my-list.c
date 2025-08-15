
#ifdef __cplusplus
extern "C" {
#endif
#include "my-list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
/*#define FAIL exit(1)*/
// not using stdlib for now
#define FAIL printf("possible seg fault");

List *List_new(unsigned long bytes) {
  List *l = (List *)calloc(1, sizeof(List));
  l->width = bytes;
  l->length = 0;
  l->head = calloc(1, bytes);
  l->size = 1;
  return l;
}
void List_prettyPrint(const List *list) {
  if (list == NULL) {
    printf("List is NULL.\n");
    return;
  }
  wprintf(L"List Details:\n");
  wprintf(L"  Width:  %lu\n", list->width);
  wprintf(L"  Length: %u\n", list->length);
  wprintf(L"  Size:   %u\n", list->size);
  wprintf(L"  Head:   %p\n", list->head);
  fflush(stdout);
}
void List_delete(List *l) {
  free(l->head);
  l->head = NULL;
  free(l);
}
#ifdef DEBUGRANGE
void errPrint(char *message) {
  // just prints stuff in red and exits out
  printf(KRED "%s" RESET, message);
  FAIL;
}
void warnPrint(char *message) {
  // prints things in orange
  printf(KYEL "%s" RESET, message);
}
#else

void errPrint(char *message) {}
void warnPrint(char *message) {}
#endif /* ifdef DEBUGRANGE */
void List_resize(List *l, unsigned int newSize) {
  if (newSize < 1) {
    newSize = 1;
  }

  void *newPlace = realloc(l->head, newSize * l->width);
  if (newPlace == NULL) {
    free(l->head);
    errPrint("ran out of space for some realloc, in List_resize\n");
  }
  l->head = newPlace;
  l->size = newSize;
  l->length = (l->length < l->size) ? (l->length) : (l->size);
}
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
void List_set(List *l, unsigned int i, const void *element) {
  checkBounds(l, i);
  memcpy(l->head + i * l->size, element, l->width);
}
// expands list to
void *List_gst(List *l, unsigned int i) {
  checkBounds(l, i);
  return l->head + i * l->width;
}
void List_zeroOut(List *l) {
  // "calloc"
  for (int i = 0; i < l->size; i++) {
    memset(List_gst(l, i), 0, l->width);
  }
}
void List_append(List *l, const void *element) {
  if (l->length + 1 >= l->size) {
    unsigned int nl = l->length + (l->length >> 1) + 1;
    List_resize(l, nl);
  }
  memcpy(l->head + l->width * l->length, element, l->width);
  l->length++;
}
void List_remove(List *l, unsigned int i) {
  if (checkBounds(l, i)) {
    memcpy(l->head + i * l->width, l->head + (i + 1) * l->width,
           (l->length - i) * l->width);
    l->length--;
  }
}
void List_insert(List *l, unsigned int i, void *element) {
  if (i == l->length) {
    List_append(l, element);
    return;
  }
  if (l->size <= l->length + 1) {
    List_resize(l, l->length + 1);
  }
  l->length++;
  memcpy(l->head + (i + 1) * l->width, l->head + i * l->width,
         l->width * (l->length - i));
  List_set(l, i, element); // seg fault
}
void List_print_as_string(List *l) {
  for (unsigned int i = 0; i < l->length; i++) {
    char c = *(char *)List_gst(l, i); // Access each char
    printf("%c", c);                  // Print the character
  }
  printf("\n");
}
int List_forEach(List *l, int (*function)(void *)) {
  int result = 1;
  for (int i = 0; i < l->length; i++) {
    result = result && function(List_gst(l, i));
  }
  return result;
}
void List_fromArr(List *l, const void *source, unsigned int i) {
  // helper function for turning arrays into lists
  l->length = i;
  List_resize(l, i);
  memcpy(l->head, source, l->width);
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
void List_appendFromArr(List *l, const void *source, unsigned int index) {
  for (unsigned int i = 0; i < index; i++) {
    List_append(l, source + (l->width * i));
  }
}
#ifdef __cplusplus
}
#endif
