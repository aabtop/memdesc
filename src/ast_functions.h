#ifndef MEMDESC_AST_FUNCTIONS_H_
#define MEMDESC_AST_FUNCTIONS_H_

#include "ast.h"

std::string ToString(const Primitive& primitive);
std::string ToString(const BaseType& base_type);
std::string ToString(const Type& type);
std::string ToString(const Field& field);
std::string ToString(const Struct& s);

// Returns the name of a given base type.
const std::string& BaseTypeName(const BaseType& base_type);

// Returns the name of a given type.
inline const std::string& TypeName(const Type& type) {
  return BaseTypeName(type.base_type);
}

#endif  // MEMDESC_AST_FUNCTIONS_H_
