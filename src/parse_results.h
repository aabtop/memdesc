#ifndef MEMDESC_PARSE_RESULTS_H_
#define MEMDESC_PARSE_RESULTS_H_

// This file defines data structures that will exist and be updated while
// a parse is taking place.  For example it will track all currently defined
// types.  It is returned as the output of a parse.

#include <memory>
#include <unordered_map>

#include "ast.h"
#include "parse_error.h"

class ParseResults {
 public:
  ParseResults() = default;
  ParseResults(const ParseResults& rhs);
  ParseResults& operator=(ParseResults&& rhs) = default;
  ParseResults& operator=(const ParseResults& rhs) {
    *this = ParseResults(rhs);
    return *this;
  }

  std::optional<ParseErrorWithLocation> Merge(ParseResults&& src);

  void AddPrimitive(std::unique_ptr<Primitive> primitive);
  void AddStruct(std::unique_ptr<Struct> s);

  const std::unordered_map<std::string, std::unique_ptr<Primitive>>&
  primitives() const {
    return primitives_;
  }
  const std::unordered_map<std::string, std::unique_ptr<Struct>>& structs()
      const {
    return structs_;
  }

  const std::vector<BaseType>& declaration_order() const {
    return declaration_order_;
  }

 private:
  std::unordered_map<std::string, std::unique_ptr<Primitive>> primitives_;
  std::unordered_map<std::string, std::unique_ptr<Struct>> structs_;

  std::vector<BaseType> declaration_order_;
};

std::string ToString(const ParseResults& parse_results);

std::optional<BaseType> LookupBaseType(const ParseResults& parse_results,
                                       const std::string& name);

#endif  // MEMDESC_PARSE_RESULTS_H_
