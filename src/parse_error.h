#ifndef MEMDESC_PARSE_ERROR_H_
#define MEMDESC_PARSE_ERROR_H_

#include <string>
#include <variant>

#include "ast.h"

struct UndeclaredTypeReference {
  std::string type_name;
};

struct TypeRedefinition {
  BaseType original_definition;
};

struct GenericError {
  std::string message;
};

struct SyntaxError {
  std::string message;
};

using ParseError = std::variant<
    UndeclaredTypeReference, TypeRedefinition, GenericError, SyntaxError>;

std::string ToString(const ParseError& error);

#endif  // MEMDESC_PARSE_ERROR_H_
