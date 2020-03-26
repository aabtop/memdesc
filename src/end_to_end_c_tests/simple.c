#include "simple_desc.h"

int main(int argc, const char** args) {
  // We're satisfied if this compiles.
  Foo foo;
  foo.a = 1;     // NOLINT(readability-magic-numbers)
  foo.b = 0.5F;  // NOLINT(readability-magic-numbers)
  foo.c = 'a';   // NOLINT(readability-magic-numbers)
  foo.d = -5;    // NOLINT(readability-magic-numbers)

  return 0;
}
