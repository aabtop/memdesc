#include "ast_sizing.h"

#include <cassert>

#include "ast.h"
#include "ast_functions.h"

namespace {
// Rounds up |x| to the given |multiple|.
int RoundUp(int x, int multiple) {
  return ((x + multiple - 1) / multiple) * multiple;
}

int AlignOf(const BaseType& base_type) {
  if (const Primitive* primitive = AsPrimitive(base_type)) {
    return primitive->alignment;
  }

  const Struct* s = AsStruct(base_type);
  assert(s);
  return AlignOf(*s);
}

int SizeOf(const BaseType& base_type) {
  if (const Primitive* primitive = AsPrimitive(base_type)) {
    return primitive->size;
  }

  const Struct* s = AsStruct(base_type);
  assert(s);
  return SizeOf(*s);
}

}  // namespace

int AlignOf(const Struct& s) {
  // https://en.cppreference.com/w/cpp/language/alignof
  int max_alignment = 1;
  for (int i = 0; i < s.fields.size(); ++i) {
    max_alignment = std::max(max_alignment, AlignOf(s.fields[i].type));
  }
  return max_alignment;
}

int SizeOf(const Struct& s) {
  if (s.fields.empty()) {
    // https://en.cppreference.com/w/cpp/language/sizeof
    return 1;
  }

  int last_field_index = static_cast<int>(s.fields.size()) - 1;
  return RoundUp(OffsetOfField(s, last_field_index) +
                     SizeOf(s.fields[last_field_index].type),
                 AlignOf(s));
}

int OffsetOfField(const Struct& owning_struct, int field_index) {
  assert(field_index < owning_struct.fields.size());
  int offset = 0;
  for (int i = 0; i < field_index; ++i) {
    offset += RoundUp(SizeOf(owning_struct.fields[i].type),
                      AlignOf(owning_struct.fields[i + 1].type));
  }
  return offset;
}

int AlignOf(const Type& type) { return AlignOf(type.base_type); }

int SizeOf(const Type& type) {
  return SizeOf(type.base_type) * type.array_count.value_or(1);
}
