#include "ast_test_helpers.h"

#include "parser_wrapper.h"

ParseResultsOrError ParseTestInput(std::string str) {
  str.push_back('\0');
  str.push_back('\0');

  return ParseFromBuffer(str.data(), str.length());
}

ParseResultsOrError ParseTestInputWithDefaultPrimitives(
    const std::string& str) {
  std::string initial_primitives_defs =
      "primitive float(4, 4);"
      "primitive int32_t(4, 4);"
      "primitive double(8, 8);\n";

  return ParseTestInput(initial_primitives_defs + str);
}
