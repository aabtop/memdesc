![Build and Test](https://github.com/aabtop/memdesc/workflows/Build%20and%20Test/badge.svg)

# memdesc
Make C struct definitions available in other languages to facilitate bidirectional 0-copy cross-language in-or-out-of-process communication.

An example input is given in [src/examples/importer_with_preamble.mdesc](src/examples/importer_with_preamble.mdesc):

```c
#import "importee_with_preamble.mdesc"

struct ImporterStruct {
  float a;
  ImporteeStruct b;
};
```

which refers to [src/examples/importee_with_preamble.mdesc](src/examples/importee_with_preamble.mdesc):

```c
struct ImporteeStruct {
  float a;
  char b;
};
```

When run with the command:

```bash
memdesc src/examples/importer_with_preamble.mdesc -t c
```

the following output is produced for consumption by a C (or C++) compiler:

```c
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  float a;
  char b;
} ImporteeStruct;

#if __cplusplus && (__cplusplus >= 201103L || _MSC_VER >= 1900)       
// Static assertions to verify memdesc's assumptions.
static_assert(sizeof(ImporteeStruct) == 8);
static_assert(alignof(ImporteeStruct) == 4);
static_assert(offsetof(ImporteeStruct, a) == 0);
static_assert(offsetof(ImporteeStruct, b) == 4);
#endif  // __cplusplus && (__cplusplus >= 201103L || _MSC_VER >= 1900)

typedef struct {
  float a;
  ImporteeStruct b;
} ImporterStruct;

#if __cplusplus && (__cplusplus >= 201103L || _MSC_VER >= 1900)
// Static assertions to verify memdesc's assumptions.
static_assert(sizeof(ImporterStruct) == 12);
static_assert(alignof(ImporterStruct) == 4);
static_assert(offsetof(ImporterStruct, a) == 0);
static_assert(offsetof(ImporterStruct, b) == 4);
#endif  // __cplusplus && (__cplusplus >= 201103L || _MSC_VER >= 1900)

#ifdef __cplusplus
}  // extern "C"
#endif
```

In the command line above, `-t c` specifies that the target output language should be C.  Other languages are also available (but actually not yet because this project is very young and incomplete right now).

## Building

If you have Docker installed, you can just run `./build.sh` which will setup a Docker Image capable of building memdesc, use it to perform a build, and place the output in `out/`.

Alternatively, the project can be built with [CMake](https://cmake.org/).  Even if you don't have Docker installed, you can refer to [the build Dockerfile](src/build/docker/linux/Dockerfile) for instructions on how to setup your environment.

