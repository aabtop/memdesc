#ifndef MEMDESC_AST_TEST_HELPERS_H_
#define MEMDESC_AST_TEST_HELPERS_H_

#include <optional>
#include <string>

#include "parser_wrapper.h"

ParseResultsOrError ParseTestInput(std::string str);

ParseResultsOrError ParseTestInputWithDefaultPrimitives(const std::string& str);

#endif  // MEMDESC_AST_TEST_HELPERS_H_
