#ifndef MEMDESC_AST_FUNCTIONS_H_
#define MEMDESC_AST_FUNCTIONS_H_

#include <string>

#include "ast.h"

std::string ToString(const SourceLocation& location);
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

const SourceLocation& DefinedAt(const BaseType& base_type);

// Helper function to quickly get access to the underlying type of a BaseType.
const Primitive* AsPrimitive(const BaseType& base_type);
// Helper function to quickly get access to the underlying type of a BaseType.
const Struct* AsStruct(const BaseType& base_type);

inline bool operator==(const SourceLocation& a, const SourceLocation& b) {
  return a.line_number == b.line_number && a.column_number == b.column_number &&
         a.filename == b.filename;
}

#endif  // MEMDESC_AST_FUNCTIONS_H_
