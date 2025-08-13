bhln#pragma once
extern "C" {
#include "umap.h"
}
#include <string>
using namespace std;
struct UMapPlus {
  UMap *ptr;
  UMapPlus() { ptr = UMap_new(); }
  template <typename T> T get(string key) {
    // clang-format off
    return *(T*)UMap_getValAtKey(ptr,
        (um_fp){.ptr = (void *)key.c_str(), .length = (unsigned int)key.length()}).ptr;

    // clang-format on
  }
  template <typename T> void set(string key, T &val) {
    void *pointer = &val;
    // clang-format off
    UMap_set(
        ptr, 
        (um_fp){.ptr = (void*)key.c_str(),.length = (unsigned int)key.length()},
  
        (um_fp) {.ptr = pointer,.length = sizeof(T)}
    );
    // clang-format on
  }

  void remake() { UMap_remake(&ptr); }
  void getIndex(unsigned int index) { UMap_getKeyAtIndex(ptr, index); }
  void destroy() { UMap_delete(ptr); }
};
