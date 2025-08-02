#ifndef LISTPLUS
#define LISTPLUS
#include "list.h"

template <typename T> struct listPlus {
  List *ptr;

  listPlus() {
    ptr = $List(T); // creates new list with sizeof(T)
  }
  listPlus(T arr[], unsigned int length) {
    ptr = $List(T);
    List_fromArr(ptr, arr, length);
  }
  T *autoCast() { return (T *)ptr->head; }

  ~listPlus() {
    List_delete(ptr); // clean up
  }
  void resize(unsigned int newSize) { List_resize(ptr, newSize); }

  void append(const T &value) { List_append(ptr, (void *)&value); }

  T get(unsigned int i) const { return *((T *)List_gst(ptr, i)); }

  void set(unsigned int i, const T &value) { List_set(ptr, i, &value); }

  void print() const { List_prettyPrint(ptr); }
  unsigned int length() const { return ptr->length; }
};

#endif
