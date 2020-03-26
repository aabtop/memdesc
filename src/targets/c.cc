#include "ast_functions.h"
#include "target_registry.h"

std::optional<TargetError> OutputC(const ParseResults& parse_results,
                                   std::ostream& out) {
  out << "#include <stdint.h>" << std::endl;

  for (const auto& item : parse_results.structs) {
    const auto& parsed_struct = *item.second;

    if (parsed_struct.defined_at.filename) {
      out << "#line " << parsed_struct.defined_at.line_number << " "
          << *parsed_struct.defined_at.filename << std::endl;
    }

    out << "typedef struct {" << std::endl;
    for (const auto& field : parsed_struct.fields) {
      out << "  " << TypeName(field.type) << " " << field.name;
      if (field.type.array_count) {
        out << "[" << *field.type.array_count << "]";
      }
      out << ";" << std::endl;
    }

    out << "} " << parsed_struct.name << ";" << std::endl;
  }

  return std::nullopt;
}
