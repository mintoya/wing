#pragma once
#include "my-list.h"

template <typename T> struct listPlus {
  List *ptr;

  listPlus() { ptr = List_new(sizeof(T)); }
  listPlus(void *p) { ptr = (List *)p; }
  listPlus(List *p) { ptr = p; }
  listPlus(T arr[], unsigned int length) {
    ptr = List_new(sizeof(T));
    List_fromArr(ptr, arr, length);
  }
  T *autoCast() { return (T *)ptr->head; }
  const unsigned int length() { return ptr->length; }

  // ~listPlus() {
  //   List_delete(ptr); // clean up
  // }
  void resize(unsigned int newSize) { List_resize(ptr, newSize); }
  void unmake() { List_delete(ptr); }

  void append(const T &value) { List_append(ptr, (void *)&value); }

  T get(unsigned int i) const { return *((T *)List_gst(ptr, i)); }

  void set(unsigned int i, const T &value) { List_set(ptr, i, &value); }
  int searchFor(const T &value) { return List_search(ptr, &value); }

  void print() const { List_prettyPrint(ptr); }
  unsigned int length() const { return ptr->length; }
};
