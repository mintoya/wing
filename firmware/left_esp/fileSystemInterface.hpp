#pragma once
#include "FFat.h"
#include "FS.h"
#include "my-list/my-list.h"
#include "string-List/um_fp.h"
#include <stdint.h>

// somewhat adapted from the FFat test

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

// You only need to format FFat the first time you run a test

inline bool fsActive() { return FFat.totalBytes() > 0; }

um_fp readFile(const char *path) {
  if (!fsActive()) {
    Serial.println("fs isnt active, reads not available");
    return nullUmf;
  }
  File file = FFat.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("file not available");
    return nullUmf;
  }

  List *buffer = List_new(sizeof(uint8_t));
  while (file.available()) {
    uint8_t r = file.read();
    List_append(buffer, &r);
  }
  file.close();
  um_fp res = {.ptr = buffer->head, .width = buffer->length};
  free(buffer);
  return res;
}

void writeFile(const char *path, um_fp buffer) {
  if (!fsActive()) {
    Serial.println("fs isnt active, writes not available");
    return;
  }
  File file = FFat.open(path, FILE_WRITE);

  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }

  if (file.write((uint8_t *)buffer.ptr, buffer.width) < buffer.width) {
    Serial.println("- write failed");
  } else {
    Serial.print("saved file: ");
    Serial.println(path);
  }
  file.close();
}
void listDir(const char *dirname, uint8_t levels = 10) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = FFat.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void deleteFile(const char *path) {
  if (!fsActive()) {
    Serial.println("fs isnt active, deletes not available");
    return;
  }
  Serial.printf("Deleting file: %s\r\n", path);
  if (FFat.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}
void FSISetup() {
    if (!FFat.begin()) {
        Serial.println("FFat Mount Failed, formatting file system");
        FFat.format();
        if (!FFat.begin()) {
            Serial.println("FFat mount failed even after format");
        } else {
            Serial.println("FFat mounted successfully");
        }
    }
}
