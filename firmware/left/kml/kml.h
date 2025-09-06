#ifndef KML_PARSER_H
#define KML_PARSER_H

#include "../string-List/stringList.h"
#include <alloca.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define min(a, b) ((a < b) ? (a) : (b))
#define max(a, b) ((a > b) ? (a) : (b))
// index after end if c not present
// unsigned int um_indexOf(um_fp string, char c) ;
// um_fp inside(char limits, um_fp string);
// um_fp around(char limits[2], um_fp string);
//
// um_fp until(char delim, um_fp string);
// um_fp behind(char delim, um_fp string);
// um_fp after(um_fp main, um_fp slice) ;
// #define stack_split(result, string, ...) \
//   result = alloca( \
//       sizeof(um_fp) * \
//       (sizeof((unsigned int[]){__VA_ARGS__}) / sizeof(unsigned int) + 1)); \
//   do { \
//     void *last; \
//     unsigned int args[] = {__VA_ARGS__}; \
//     for (int i = 0; i < sizeof(args) / sizeof(unsigned int); i++) { \
//       args[i] = (i == 0) ? (min(args[i], string.width)) \
//                          : (min(string.width, max(args[i], args[i - 1]))); \
//       result[i] = (um_fp){ \
//           .ptr = (i == 0) ? (string.ptr) : (last), \
//           .width = (i == 0) ? (args[0]) : (args[i] - args[i - 1]), \
//       }; \
//       last = ((char *)result[i].ptr) + result[i].width; \
//     } \
//     result[sizeof(args) / sizeof(unsigned int)] = \
//         (um_fp){.ptr = last, .width = string.width - (last - string.ptr)}; \
//   } while (0);
// char isSkip(char c) ;
// um_fp removeSpacesPadding(um_fp in) ;
// um_fp skipComments(um_fp source) ;
typedef struct {
  um_fp key;
  um_fp value;
  um_fp footprint;
} kVf;
// kVf parseNext(um_fp string);
um_fp findIndex(um_fp str, unsigned int index);
um_fp findKey(um_fp str, um_fp key);
typedef struct {
  enum {
    NONE = 0,
    STRING = 1,
    INDEX = 2,
  } type;
  union {
    um_fp pArg;
    unsigned int index;
  } data;
} parseArg;

um_fp findAny(um_fp str, parseArg ki);
#define fIndex(val) ((parseArg){.type = INDEX, .data.index = val})
#define fKey(val) ((parseArg){.type = STRING, .data.pArg = um_from(val)})

// must be none terminated
um_fp find_many(um_fp str, ...);
#define find(um, ...) find_many(um, __VA_ARGS__, ((parseArg){.type = NONE}))
#endif
#define KML_PARSER_C
#ifdef KML_PARSER_C

// index after end if c not present
static unsigned int um_indexOf(um_fp string, char c) {
  int i;
  for (i = 0; i < string.width; i++) {
    if (((char *)string.ptr)[i] == c) {
      return i;
    }
  }
  return i;
}
static um_fp inside(char limits[2], um_fp string) {
  char front = limits[0];
  char back = limits[1];

  for (int i = 0; i < string.width; i++) {
    if (((char *)string.ptr)[i] == front) {
      unsigned int counter = 1;
      for (int ii = 1; ii + i < string.width; ii++) {
        if (((char *)string.ptr)[i + ii] == front) {
          counter++;
        } else if (((char *)string.ptr)[i + ii] == back) {
          counter--;
        }
        if (!counter)
          return (um_fp){.ptr = ((char *)string.ptr) + i + 1, .width = ii - 1};
        if (i + ii == string.width - 1)
          return (um_fp){.ptr = ((char *)string.ptr) + i + 1, .width = ii - 1};
      }
    }
  }
  return nullUmf;
}
static um_fp around(char limits[2], um_fp string) {

  char front = limits[0];
  char back = limits[1];

  for (int i = 0; i < string.width; i++) {
    if (((char *)string.ptr)[i] == front) {
      unsigned int counter = 1;
      for (int ii = 1; ii + i < string.width; ii++) {
        if (((char *)string.ptr)[i + ii] == front) {
          counter++;
        } else if (((char *)string.ptr)[i + ii] == back) {
          counter--;
        }
        if (!counter)
          return (um_fp){.ptr = ((char *)string.ptr) + i, .width = ii + 1};
        if (i + ii == string.width - 1)
          return (um_fp){.ptr = ((char *)string.ptr) + i, .width = ii + 1};
      }
    }
  }
  return nullUmf;
}

static um_fp until(char delim, um_fp string) {
  int i = 0;
  while (i < string.width && ((char *)string.ptr)[i] != delim) {
    i++;
  }
  string.width = i;
  return string;
}
static um_fp behind(char delim, um_fp string) {
  int i = 0;
  while (i < string.width && ((char *)string.ptr)[i] != delim) {
    i++;
  }
  string.width = min(i + 1, string.width);
  return string;
}
static um_fp after(um_fp main, um_fp slice) {
  if (!(main.ptr && main.width))
    return nullUmf;
  if (!(slice.ptr && slice.width))
    return main;
  char *mainStart = main.ptr;
  char *mainEnd = mainStart + main.width;
  char *sliceStart = slice.ptr;
  char *sliceEnd = sliceStart + slice.width;

  // assert(sliceStart >= mainStart && sliceEnd <= mainEnd);

  return (um_fp){.ptr = sliceEnd, .width = mainEnd - sliceEnd};
}
#define stack_split(result, string, ...)                                       \
  result = alloca(                                                             \
      sizeof(um_fp) *                                                          \
      (sizeof((unsigned int[]){__VA_ARGS__}) / sizeof(unsigned int) + 1));     \
  do {                                                                         \
    void *last;                                                                \
    unsigned int args[] = {__VA_ARGS__};                                       \
    for (int i = 0; i < sizeof(args) / sizeof(unsigned int); i++) {            \
      args[i] = (i == 0) ? (min(args[i], string.width))                        \
                         : (min(string.width, max(args[i], args[i - 1])));     \
      result[i] = (um_fp){                                                     \
          .ptr = (i == 0) ? (string.ptr) : (last),                             \
          .width = (i == 0) ? (args[0]) : (args[i] - args[i - 1]),             \
      };                                                                       \
      last = ((char *)result[i].ptr) + result[i].width;                        \
    }                                                                          \
    result[sizeof(args) / sizeof(unsigned int)] =                              \
        (um_fp){.ptr = last, .width = string.width - (last - string.ptr)};     \
  } while (0);
static char isSkip(char c) {
  switch (c) {
  case ' ':
    return 1;
  case '\n':
    return 1;
  case '\0':
    return 1;
  case '\t':
    return 1;
  default:
    return 0;
  }
}
static um_fp removeSpacesPadding(um_fp in) {
  um_fp res = in;
  int front = 0;
  int back = in.width - 1;
  while (front < in.width && isSkip(((char *)in.ptr)[front])) {
    front++;
  }
  while (back > front && isSkip(((char *)in.ptr)[back])) {
    back--;
  }
  um_fp *splits = stack_split(splits, in, front, back + 1);
  res = splits[1];
  return res;
}
static kVf parseNext(um_fp string) {
  if (!(string.ptr && string.width)) {
    return (kVf){nullUmf, nullUmf};
  }
  um_fp name = until(':', string);
  name = removeSpacesPadding(name);
  if (name.ptr == string.ptr && name.width == string.width) {
    return (kVf){nullUmf, nullUmf};
  }

  um_fp next = after(string, behind(':', string));

  next = removeSpacesPadding(next);

  if (!(next.ptr && next.width)) {
    return (kVf){nullUmf, nullUmf};
  }

  um_fp toParse;
  um_fp value;

  switch (((char *)next.ptr)[0]) {
  case '[':
    toParse = around("[]", next);
    value = removeSpacesPadding(inside("[]", next));
    break;
  case '{':
    toParse = around("{}", next);
    value = removeSpacesPadding(inside("{}", next));
    break;
  default:
    toParse = behind(';', next);
    value = removeSpacesPadding(until(';', next));
    break;
  }
  // if (name.width == 2 && ((char *)name.ptr)[1] == '/' &&
  //     ((char *)name.ptr)[0] == '/')
  //   usePrintln(char *, "key found a comment");
  return (kVf){name, value, toParse};
}
um_fp findIndex(um_fp str, unsigned int index) {
  if (!(str.ptr && str.width))
    return nullUmf;
  str = removeSpacesPadding(str);
  um_fp thisValue;
  char isobj = 0, isComment = 0;
  switch (*(char *)str.ptr) {
  case ('['):
    thisValue = around("[]", str);
    isobj = 1;
    break;
  case ('/'):
    isComment = 1;
  case ('{'):
    thisValue = around("{}", str);
    isobj = 2;
    break;
  default:
    thisValue = until(',', str);
  }
  if (isComment) {
    um_fp next = after(str, thisValue);
    // usePrintln(char *, "index found a comment");
    return findIndex(next, index);
  }
  if (!index) {
    if (isobj) {
      if (isobj == 1)
        return inside("[]", thisValue);
      if (isobj == 2)
        return inside("{}", thisValue);
    }
    return thisValue;
  } else {
    um_fp next = after(str, thisValue);
    if (um_indexOf(next, ',') < next.width) {
      next = after(str, behind(',', next));
      return findIndex(next, index - 1);
    } else {
      return nullUmf;
    }
  }
}
um_fp findKey(um_fp str, um_fp key) {
  key = removeSpacesPadding(key);
  while (str.ptr) {
    kVf read = parseNext(str);
    if (!(read.key.ptr && read.key.width)) {
      // usePrintln(char * ,"null");
      return nullUmf;
    } else if (um_eq(key, read.key)) {
      return read.value;
    }
    str = after(str, read.footprint);
  }
  return nullUmf;
}

um_fp findAny(um_fp str, parseArg ki) {
  switch (ki.type) {
  case STRING:
    return findKey(str, ki.data.pArg);
    break;
  case INDEX:
    return findIndex(str, ki.data.index);
    break;
  default:
    return nullUmf;
  }
}

um_fp find_many(um_fp str, ...) {
  va_list ap;
  va_start(ap, str);
  parseArg key;
  while ((key = va_arg(ap, parseArg)).type) {
    if (key.type == STRING)
      str = findKey(str, key.data.pArg);
    if (key.type == INDEX)
      str = findIndex(str, key.data.index);
  }

  va_end(ap);
  return str;
}
#endif // KML_PARSER_C

#undef max
#undef min
