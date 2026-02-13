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
  File file = FFat.open(path, FILE_READ);
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

void listDir(const char *dirname, uint8_t levels = 10) {
  if (!fsActive())
    return;

  File root = FFat.open(dirname);
  if (!root)
    return;

  if (!root.isDirectory()) {
    root.close();
    return;
  }

  File file = root.openNextFile();
  while (file && levels > 0) {
    if (file.isDirectory()) {
      println_("DIR {cstr}", file.path());
      if (levels > 1) {
        listDir(file.path(), levels - 1);
      }
    } else {
      println_("FILE {}",file.path());
    }
    file = root.openNextFile();
  }
  root.close();
}

void deleteFile(const char *path) {
  if (!fsActive())
    return;
  FFat.remove(path);
}

void FSISetup() {
  // Try to mount existing filesystem first
  if (FFat.begin()) {
    return;
  }

  // If mounting fails, format and create new filesystem
  FFat.format();
  FFat.begin();
}
