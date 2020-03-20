#ifndef MEMDESC_AST_TEST_HELPERS_H_
#define MEMDESC_AST_TEST_HELPERS_H_

#include <optional>
#include <string>

#include "parse_results.h"

ParseResults ParseTestInput(std::string str);

ParseResults ParseTestInputWithDefaultPrimitives(std::string str);

#endif  // MEMDESC_AST_TEST_HELPERS_H_
