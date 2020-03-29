#include "multiple_structs.h"

int main(int argc, const char** args) {
  // We're satisfied if this compiles.
  Street street;
  street.yards[0].ants[0].color.g = 1.0F;
  street.yards[0].barn.door_color.r = 1.0F;

  return 0;
}
