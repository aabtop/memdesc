#ifndef MEMDESC_PARSE_ERROR_H_
#define MEMDESC_PARSE_ERROR_H_

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "ast.h"

struct UndeclaredTypeReference {
  std::string type_name;
};

struct RedefinitionError {
  std::string name;
  SourceLocation original_definition_location;
};

struct GenericError {
  std::string message;
};

struct SyntaxError {
  std::string message;
};

struct FileOpenError {
  std::filesystem::path filename;
};

struct ParseErrorWithLocation;

// Error occurred within an #import call, so track the "sub error" as well
// as the file that we were importing at the time.
struct ImportError {
  std::filesystem::path import_filename;
  std::unique_ptr<ParseErrorWithLocation> sub_error;
};

using ParseError =
    std::variant<UndeclaredTypeReference, RedefinitionError, GenericError,
                 SyntaxError, FileOpenError, ImportError>;

std::string ToString(const ParseError& error);

struct ParseErrorWithLocation {
  ParseError error;
  SourceLocation location;
};
std::string ToString(const ParseErrorWithLocation& error);

#endif  // MEMDESC_PARSE_ERROR_H_
