#include "parse_error.h"

#include <string>

#include "ast_functions.h"

namespace {
std::string ToString(const UndeclaredTypeReference& e) {
  return "Reference to undeclared type \"" + e.type_name + "\".";
}

std::string ToString(const TypeRedefinition& e) {
  return "Redefinition of type \"" + BaseTypeName(e.original_definition) +
         "\", originally defined at " +
         ToString(DefinedAt(e.original_definition)) + ".";
}

std::string ToString(const GenericError& e) {
  return e.message;
}

std::string ToString(const SyntaxError& e) {
  return e.message;
}

};

std::string ToString(const ParseError& error) {
  return std::visit([](auto& x) -> std::string { return ToString(x); }, error);
}
