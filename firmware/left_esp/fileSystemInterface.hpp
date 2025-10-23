#pragma once
#include "FFat.h"
#include "FS.h"
#include "my-list/my-list.h"
#include "string-List/um_fp.h"
#include <cstring>

inline bool fsActive() { return FFat.begin(); }

um_fp readFile(const char *path) {
  if (!fsActive())
    return nullUmf;

  Serial.print("reading ");
  Serial.println(path);
  File file = FFat.open(path, FILE_READ);
  if (!file)
    return nullUmf;

  size_t size = file.size();
  void *buffer = malloc(size);
  if (!buffer) {
    file.close();
    return nullUmf;
  }

  file.readBytes((char *)buffer, size);
  file.close();
  return (um_fp){.ptr = buffer, .width = size};
}
um_fp readFile(um_fp path) {
  char *fname = (char *)calloc(sizeof(char) , path.width + 1);
  memcpy(fname, path.ptr, path.width);
  um_fp res = readFile(fname);
  free(fname);
  return res;
}

void writeFile(const char *path, um_fp buffer) {
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
      Serial.print("DIR ");
      Serial.println(file.path());
      if (levels > 1) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.print("FILE ");
      Serial.println(file.path());
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
