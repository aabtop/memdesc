#include "ast.h"

#include <sstream>

std::string ToString(const Field& field) {
  std::ostringstream oss;
  oss << "Field(" << field.type << ", " << field.name;
  if (field.count) {
    oss << ", " << *field.count;
  }
  oss << ")";
  return oss.str();
}

std::string ToString(const Struct& s) {
  std::ostringstream oss;
  oss << "Struct(" << s.name << ", {";
  for (const auto& field : s.fields) {
    oss << ToString(field) << ", ";
  }
  oss << "})";
  return oss.str();
}

std::string ToString(const StructDeclarationList& list) {
  std::ostringstream oss;
  oss << "StructDeclarationList({";
  for (const auto& s : list) {
    oss << ToString(*s) << ", ";
  }
  oss << "})";
  return oss.str();
}
