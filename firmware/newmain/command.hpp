#include "fileSystemInterface.hpp"
#include "my-lib/omap.h"
#include "my-lib/vason.h"
#include <cstdio>
#include <memory>

extern void prettyPrintLayers(void);
namespace commands {
typedef void (*command)(AllocatorV, vason *args);
typedef fptr commandName;
typedef struct {
  commandName key;
  command value;
  usize nargs;
  char **args;
} kv;

template <usize N, usize I>
kv makeCommand(const char (&strname)[N], command fn, char *(&args)[I]) {
  return kv{
      .key = {N - 1, (u8 *)strname},
      .value = fn,
      .nargs = I,
      .args = args,
  };
}
template <usize N>
kv makeCommand(const char (&strname)[N], command fn) {
  return kv{
      .key = {N - 1, (u8 *)strname},
      .value = fn,
      .nargs = 0,
      .args = nullptr,
  };
}
void openfile(AllocatorV _, vason filename) {
  fptr fileName = filename.asString();
  if (!fileName.width) {
    println_("supply a filename");
    return;
  }
  FILE *file = getFilePtr(fileName, "r");
  println_("<start>");
  if (file) {
    int c = 0;
    while ((c = fgetc(file)) != EOF) {
      print_("{c8}", (c8)c);
    }
  }
  println_("\n<end>");
  fclose(file);
}
void listdirectories(AllocatorV _, vason dirname) {
  fptr fileName = dirname.asString();
  char path[fileName.width + 1];
  memcpy(path, fileName.ptr, fileName.width);
  path[fileName.width] = '\0';
  const char *target = path;

  listDir(target);
}
void writeFile(AllocatorV _, vason filename, vason content) {
  fptr fileNameStr = filename.asString();
  fptr contentStr = content.asString();
  if (!fileNameStr.width) {
    println_("supply a filename");
    return;
  }
  if (!contentStr.width) {
    println_("supply a some content");
    return;
  }
  FILE *file = getFilePtr(fileNameStr, "w");
  if (!file) {
    println_("couldnt open file!! : {}", fileNameStr);
    return;
  }
  fwrite(contentStr.ptr, 1, contentStr.width, file);
  fclose(file);
}
void removeFile(AllocatorV _, vason filename) { deleteFile(filename.asString()); }
static kv basicMap[] =
    {
        makeCommand(
            "save",
            [](AllocatorV a, vason *args) {
              writeFile(a, args[0], args[1]);
            },
            (char *[]){"filename", "content"}
        ),
        makeCommand(
            "rm",
            [](AllocatorV a, vason *args) {
              removeFile(a, args[0]);
            },
            (char *[]){"filename"}
        ),
        makeCommand(
            "pretty-layers",
            [](AllocatorV a, vason *args) {
              prettyPrintLayers();
            }
        ),
        makeCommand(
            "ls",
            [](AllocatorV a, vason *args) {
              listdirectories(a, args[0]);
            },
            (char *[]){"dirname"}
        ),
        makeCommand(
            "open",
            [](AllocatorV a, vason *args) {
              openfile(a, args[0]);
            },
            (char *[]){"filename"}
        ),
};

/*
     example command
     {
        `command' : `save';
        `args'    : {
          `filename' : `/file.txt'
          `text'     : `hello world'
        };
     }

 */
#include "my-lib/arenaAllocator.h"

static void execute(fptr commandText) {
  Arena_scoped *local = arena_new_ext(stdAlloc, 1024);
  vason v = {parseStr(local, {commandText.width, commandText.ptr})};

  auto executeSingle = [](vason v, AllocatorV allocator) {
    fptr command = v["command"].asString();
    if (!command.width) {
      println_("supply a command please");
      return;
    }

    for (auto pair : basicMap) {
      if (pair.key == command) {
        vason *args = aCreate(allocator, vason, pair.nargs);

        for (auto i = 0; i < pair.nargs; i++) {
          args[i] = v["args"][((char *)pair.args[i])];
          if (args[i].tag() == vason_INVALID) {
            println_("missing required arg {cstr}", pair.args[i]);
            goto freeargs;
          }
        }
        pair.value(allocator, args);
        {
        freeargs:
          aFree(allocator, args);
        }
        return;
      }
    }
    print_("command not found : {}\navailable commands:\n", command);
    for (auto pair : basicMap)
      println_("\t{}", pair.key);
  };

  if (v.tag() == vason_ARR) {
    Arena_scoped *single = arena_new_ext(local, 1024);
    for (int i = 0; i < v.countChildren(); i++) {
      executeSingle(v[i], single);
      arena_clear(single);
    }
  } else
    executeSingle(v, local);
}
} // namespace commands
