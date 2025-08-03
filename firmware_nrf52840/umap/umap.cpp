#ifndef UMAPPLUS_H
#define UMAPPLUS_H
#include "umap.h"
#include <string>
using namespace std;
struct UMapPlus {
  UMap *ptr;
  UMapPlus() { ptr = UMap_new(); }
  void get(string key) {}
  void set(string key, string val) {
    // clang-format off
    UMap_set(
        ptr, 
        (um_fp){.ptr = (void*)key.c_str(),.length = (unsigned int)key.length()},
        (um_fp) { .ptr = (void*)val.c_str(),.length = (unsigned int)val.length()}
    );
    // clang-format on
  }
  void remake() { UMap_remake(&ptr); }
  void getIndex(unsigned int index) { UMap_getKeyAtIndex(ptr, index); }
  void destroy() { UMap_delete(ptr); }
};
#endif
