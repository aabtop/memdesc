#ifndef MEMDESC_PARSE_RESULTS_H_
#define MEMDESC_PARSE_RESULTS_H_

// This file defines data structures that will exist and be updated while
// a parse is taking place.  For example it will track all currently defined
// types.  It is returned as the output of a parse.

#include <memory>
#include <unordered_map>

#include "ast.h"
#include "parse_error.h"

struct ParseResults {
  std::unordered_map<std::string, std::unique_ptr<Primitive>> primitives;
  std::unordered_map<std::string, std::unique_ptr<Struct>> structs;
};

std::string ToString(const ParseResults& parse_results);

std::optional<BaseType> LookupBaseType(
    const ParseResults& parse_results, const std::string& name);

#endif  // MEMDESC_PARSE_RESULTS_H_
