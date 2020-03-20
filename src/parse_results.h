#ifndef MEMDESC_PARSE_RESULTS_H_
#define MEMDESC_PARSE_RESULTS_H_

// This file defines data structures that will exist and be updated while
// a parse is taking place.  For example it will track all currently defined
// types.  It is returned as the output of a parse.

#include <memory>
#include <unordered_map>

#include "ast.h"
#include "parse_error.h"

struct Error {
  SourceLocation location;
  ParseError error;
};

std::string ToString(const Error& error);

struct ParseResults {
  std::unordered_map<std::string, std::unique_ptr<Primitive>> primitives;
  std::unordered_map<std::string, std::unique_ptr<Struct>> structs;

  std::optional<Error> error;

  // Is the parse completed?  True implies not only completion but also success.
  bool complete = false;
};

std::string ToString(const ParseResults& parse_results);

std::optional<BaseType> LookupBaseType(
    const ParseResults& parse_results, std::string name);

#endif  // MEMDESC_PARSE_RESULTS_H_
