#include "parse_error.h"

#include <sstream>
#include <string>

#include "ast_functions.h"

namespace {
std::string ToString(const UndeclaredTypeReference& e) {
  return "Reference to undeclared type '" + e.type_name + "'.";
}

std::string ToString(const RedefinitionError& e) {
  return "Redefinition of type '" + e.name + "' originally defined at " +
         ToString(e.original_definition_location) + ".";
}

std::string ToString(const GenericError& e) { return e.message; }

std::string ToString(const SyntaxError& e) { return e.message; }

std::string ToString(const FileOpenError& e) {
  return "Error opening file '" + e.filename.string() + "'.";
}

std::string ToString(const ImportError& e) {
  return "Error while importing file '" + e.import_filename.string() +
         "':\n\t" + ToString(*e.sub_error);
}

}  // namespace

std::string ToString(const ParseError& error) {
  return std::visit([](auto& x) -> std::string { return ToString(x); }, error);
}

std::string ToString(const ParseErrorWithLocation& error) {
  std::ostringstream oss;

  oss << "Error(" << ToString(error.location) << ", \"" << ToString(error.error)
      << "\")";

  return oss.str();
}
