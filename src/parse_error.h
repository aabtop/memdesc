#ifndef MEMDESC_PARSE_ERROR_H_
#define MEMDESC_PARSE_ERROR_H_

#include <optional>
#include <string>
#include <variant>

#include "ast.h"

struct UndeclaredTypeReference {
  std::string type_name;
};

struct TypeRedefinition {
  std::string type_name;
  SourceLocation original_definition_location;
};

struct GenericError {
  std::string message;
};

struct SyntaxError {
  std::string message;
};

struct FileOpenError {
  std::string filename;
};

using ParseError = std::variant<
    UndeclaredTypeReference, TypeRedefinition, GenericError, SyntaxError,
    FileOpenError>;
std::string ToString(const ParseError& error);

struct ParseErrorWithLocation {
  ParseError error;
  SourceLocation location;
};
std::string ToString(const ParseErrorWithLocation& error);

#endif  // MEMDESC_PARSE_ERROR_H_
