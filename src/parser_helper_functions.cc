#include "parser_helper_functions.h"

#include <string>

#include "ast_functions.h"
#include "parser_wrapper.h"

std::optional<int64_t> ParseIntInRange(const TokenText& token, int64_t lowest,
                                       int64_t highest) {
  std::string number_as_text(token.text, token.length);

  const int BASE_DECIMAL = 10;
  int64_t as_long_int = strtol(number_as_text.c_str(), NULL, BASE_DECIMAL);

  return (as_long_int < lowest || as_long_int > highest)
             ? std::nullopt
             : std::optional<int64_t>(as_long_int);
}
