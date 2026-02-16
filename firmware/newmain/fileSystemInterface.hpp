#pragma once
#include "FFat.h"
#include "FS.h"
#include "my-lib/fptr.h"
#include "my-lib/my-list.h"
#include <cstring>

inline bool fsActive() { return FFat.begin(); }
#include "my-lib/print.h"

fptr readFile(const char *path) {
  if (!fsActive())
    return nullFptr;

  println_("reading {cstr}", path);
  File file = FFat.open(path, "r");
  if (!file) {
    println_("file doesnt exist");
    return nullFptr;
  }

  size_t size = file.size();
  void *buffer = malloc(size);
  if (!buffer) {
    file.close();
    return nullFptr;
  }

  file.readBytes((char *)buffer, size);
  file.close();
  println_("sucessful file read");
  return (fptr){size, (u8 *)buffer};
}
fptr readFile(fptr path) {
  char *fname = (char *)calloc(sizeof(char), path.width + 1);
  memcpy(fname, path.ptr, path.width);
  fptr res = readFile(fname);
  free(fname);
  return res;
}

void writeFile(const char *path, fptr buffer) {
  if (!fsActive())
    return;

  File file = FFat.open(path, FILE_WRITE);
  if (!file)
    return;

  file.write((uint8_t *)buffer.ptr, buffer.width);
  file.close();
}

void listDir(const char *dirname, uint8_t levels = -1) {
  if (!fsActive())
    return;
  const char *target = (dirname == nullptr || strlen(dirname) == 0) ? "/" : dirname;

  File root = FFat.open(target);
  if (!root || !root.isDirectory()) {
    if (root && !root.isDirectory())
      println_("FILE: {cstr}", target);
    if (root)
      root.close();
    return;
  }

  File file = root.openNextFile();
  if (!file)
    println_("(Directory is empty)");

  while (file) {
    if (file.isDirectory()) {
      println_("DIR  {cstr}", file.name());
      if (levels > 1)
        listDir(file.path(), levels - 1);
    } else {
      println_("FILE {cstr}  ({} bytes)", file.name(), file.size());
    }
    file = root.openNextFile();
  }
  root.close();
}
void listDir(const fptr dirname, uint8_t levels = 10) {
  char name[dirname.width + 1];
  name[dirname.width] = 0;
  memcpy(name, dirname.ptr, dirname.width);
  listDir(name, levels);
}

void deleteFile(const char *path) {
  if (!fsActive())
    return;
  FFat.remove(path);
}
void deleteFile(const fptr ptr) {
  char name[ptr.width + 1];
  name[ptr.width] = 0;
  memcpy(name, ptr.ptr, ptr.width);
  deleteFile(name);
}
FILE *getFilePtr(const char *path, const char mode[]) {
  char fullPath[64];
  snprintf(fullPath, sizeof(fullPath), "/ffat%s", path);

  FILE *f = fopen(fullPath, mode);
  return f;
}
FILE *getFilePtr(const fptr path, const char mode[]) {
  char temp[path.width + 1];
  temp[path.width] = 0;
  memcpy(temp, path.ptr, path.width);
  return getFilePtr(temp, mode);
}

void FSISetup() {
  if (FFat.begin()) {
    return;
  }
  FFat.format();
  FFat.begin();
}
