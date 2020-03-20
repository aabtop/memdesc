#include "parse_results.h"

#include <optional>
#include <sstream>
#include "ast.h"
#include "ast_functions.h"

std::string ToString(const SourceLocation& location) {
  std::ostringstream oss;
  
  oss << "(";

  if (location.filename) {
    oss << *location.filename;
  }

  oss << " : " << location.line_number << " : " << location.column_number
      << ")";
  
  return oss.str();
}

std::string ToString(const Error& error) {
  std::ostringstream oss;

  oss << "Error(" << ToString(error.location) << ", " << error.message << ")";
  
  return oss.str();
}

std::string ToString(const ParseResults& parse_results) {
  std::ostringstream oss;
  
  oss << "Primitives:" << std::endl;
  for (const auto& type : parse_results.primitives) {
    oss << "  " << ToString(*type.second) << std::endl;
  }
  oss << std::endl;
  oss << "Structs:" << std::endl;
  for (const auto& type : parse_results.structs) {
    oss << "  " << ToString(*type.second) << std::endl;
  }
  oss << std::endl;
  
  oss << "Parse error: ";
  if (!parse_results.error) {
    oss << "None";
  } else {
    oss << ToString(*parse_results.error);
  }
  oss << std::endl;

  oss << "Parse complete: " << parse_results.complete << std::endl;

  return oss.str();
}

std::optional<BaseType> LookupBaseType(
    const ParseResults& parse_results, std::string name) {
  auto found_primitive = parse_results.primitives.find(name);
  if (found_primitive != parse_results.primitives.end()) {
    return BaseType(found_primitive->second.get());
  }

  auto found_struct = parse_results.structs.find(name);
  if (found_struct != parse_results.structs.end()) {
    return BaseType(found_struct->second.get());
  }

  return std::nullopt;
}