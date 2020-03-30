#ifndef MEMDESC_PARSE_RESULTS_H_
#define MEMDESC_PARSE_RESULTS_H_

// This file defines data structures that will exist and be updated while
// a parse is taking place.  For example it will track all currently defined
// types.  It is returned as the output of a parse.

#include <filesystem>
#include <memory>
#include <set>
#include <unordered_map>

#include "ast.h"
#include "parse_error.h"

class ParseResults {
 public:
  ParseResults() = default;
  explicit ParseResults(const std::optional<std::filesystem::path>& from_file)
      : ParseResults() {
    from_file_ = from_file;
  }
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

  const std::set<std::filesystem::path>& dependencies() const {
    return dependencies_;
  }

  const std::optional<std::filesystem::path>& from_file() const {
    return from_file_;
  }

 private:
  // If provided, indicates which file these parse results were derived from.
  std::optional<std::filesystem::path> from_file_;

  // The structures that own the primitive and struct definitions.  The
  // key is the name of the type definition so that they can be easily
  // looked up by name.
  std::unordered_map<std::string, std::unique_ptr<Primitive>> primitives_;
  std::unordered_map<std::string, std::unique_ptr<Struct>> structs_;

  // The declaration order of the above primitives and structs.
  std::vector<BaseType> declaration_order_;

  // List of files that these parse results depend on, so that build systems
  // can know to re-build this file if any of these dependent files are
  // modified.
  std::set<std::filesystem::path> dependencies_;
};

std::string ToString(const ParseResults& parse_results);

std::optional<BaseType> LookupBaseType(const ParseResults& parse_results,
                                       const std::string& name);

#endif  // MEMDESC_PARSE_RESULTS_H_
