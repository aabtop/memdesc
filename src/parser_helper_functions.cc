#include "parser_helper_functions.h"

#include <string>

std::optional<long long int> ParseIntInRange(
    const TokenText& token, long long int lowest, long long int highest) {
  std::string number_as_text(token.text, token.length);
  long long int as_long_int = strtol(number_as_text.c_str(), NULL, 10);

  return (as_long_int < lowest || as_long_int > highest) ?
             std::nullopt : std::optional<long long int>(as_long_int);
}
