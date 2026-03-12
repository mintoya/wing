#include "fileSystemInterface.hpp"
#include "layout.hpp"
#include "my-lib/vason_arr.h"
#include <cstdio>

extern void prettyPrintLayers(void);
extern void parseLayout(vason);
extern volatile bool logEnabled;

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
    while ((c = fgetc(file)) != EOF)
      print_("{c8}", (c8)c);
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
void writefile(AllocatorV _, vason filename, vason content) {
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

static kv basicMap[] =
    {

        makeCommand(
            "pretty-layers",
            [](AllocatorV a, vason *args) { prettyPrintLayers(); }
        ),
        makeCommand(
            "toggle-log",
            [](AllocatorV a, vason *args) { logEnabled = !logEnabled; }
        ),
        makeCommand(
            "disable-log",
            [](AllocatorV a, vason *args) { logEnabled = false; }
        ),
        makeCommand(
            "reset-layout",
            [](AllocatorV a, vason *args) {
              vason_container c = vason_parseString(a, (slice(c8))slice_stat(defaultLayout_chars));
              parseLayout(c);
            }
        ),
        makeCommand(
            "get-layout",
            [](AllocatorV a, vason *args) { fptr f = readFile("/lay.kml");println_("{}",f);free(f.ptr); }
        ),
        makeCommand(
            "set-layout",
            [](AllocatorV a, vason *args) { parseLayout(args[0]); },
            (char *[]){"layout"}
        ),
        makeCommand(
            "rm",
            [](AllocatorV a, vason *args) { deleteFile(args[0].asString()); },
            (char *[]){"filename"}
        ),
        makeCommand(
            "open",
            [](AllocatorV a, vason *args) { openfile(a, args[0]); },
            (char *[]){"filename"}
        ),
        makeCommand(
            "ls",
            [](AllocatorV a, vason *args) { listdirectories(a, args[0]); },
            (char *[]){"dirname"}
        ),
        makeCommand(
            "save",
            [](AllocatorV a, vason *args) {
              writefile(a, args[0], args[1]);
            },
            (char *[]){"filename", "content"}
        ),
};

#include "my-lib/arenaAllocator.h"

static void execute(vason_container vc) {
  vason v = {vc};
  Arena_scoped *local = arena_new_ext(stdAlloc, 512);
  auto executeSingle = [](vason v, AllocatorV allocator) {
    fptr command = v["command"].asString();
    if (!command.width)
      return println_("supply a command please");

    for (auto pair : basicMap) {
      if (pair.key == command) {
        vason *args = aCreate(allocator, vason, pair.nargs);
        defer { aFree(allocator, args); };
        auto inargs = v["args"];
        if (!inargs && pair.nargs > 0)
          return println_("missing args table");
        for (auto i = 0; i < pair.nargs; i++) {
          args[i] = inargs[fp(pair.args[i])];
          if (!args[i])
            return println_("missing required arg {cstr}", pair.args[i]);
        }
        pair.value(allocator, args);
        return;
      }
    }
    print_("command not found : {}\navailable commands:\n", command);
    for (auto pair : basicMap) {
      print_("\t{}/{", pair.key);
      for (char *arg : *(typeof(typeof(char *(*)[pair.nargs])))(pair.args))
        print_("{cstr},", arg);
      println_("}");
    }
  };

  if (v.simpleArray()) {
    Arena_scoped *single = arena_new_ext(stdAlloc, 1024);
    for (int i = 0; i < v.countChildren(); i++) {
      executeSingle(v[i], single);
      arena_clear(single);
    }
  } else
    executeSingle(v, local);
}

} // namespace commands
