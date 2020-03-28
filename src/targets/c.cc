#include <sstream>

#include "ast_functions.h"
#include "ast_sizing.h"
#include "target_registry.h"

std::optional<TargetError> OutputC(const ParseResults& parse_results,
                                   std::ostream& out) {
  out << "#include <stddef.h>" << std::endl;
  out << "#include <stdint.h>" << std::endl;
  out << std::endl;

  out << "#ifdef __cplusplus" << std::endl;
  out << "extern \"C\" {" << std::endl;
  out << "#endif" << std::endl;
  out << std::endl;

  for (const auto& item : parse_results.structs) {
    const auto& parsed_struct = *item.second;

    out << "typedef struct {" << std::endl;
    for (const auto& field : parsed_struct.fields) {
      out << "  " << TypeName(field.type) << " " << field.name;
      if (field.type.array_count) {
        out << "[" << *field.type.array_count << "]";
      }
      out << ";" << std::endl;
    }

    out << "} " << parsed_struct.name << ";" << std::endl;

    out << std::endl;

    out << "#if __cplusplus && (__cplusplus >= 201103L || _MSC_VER >= 1900)"
        << std::endl;
    out << "// Static assertions to verify memdesc's assumptions." << std::endl;
    out << "static_assert(sizeof(" << parsed_struct.name
        << ") == " << SizeOf(parsed_struct) << ");" << std::endl;
    out << "static_assert(alignof(" << parsed_struct.name
        << ") == " << AlignOf(parsed_struct) << ");" << std::endl;
    for (int i = 0; i < parsed_struct.fields.size(); ++i) {
      const auto& field = parsed_struct.fields[i];
      out << "static_assert(offsetof(" << parsed_struct.name << ", "
          << field.name << ") == " << OffsetOfField(parsed_struct, i) << ");"
          << std::endl;
    }
    out << "#endif  // __cplusplus && (__cplusplus >= 201103L || _MSC_VER >= "
           "1900)"
        << std::endl;
  }

  out << "#ifdef __cplusplus" << std::endl;
  out << "}  // extern \"C\"" << std::endl;
  out << "#endif" << std::endl;
  out << std::endl;

  return std::nullopt;
}
