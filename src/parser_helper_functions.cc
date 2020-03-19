#include "parser_helper_functions.h"

#include <string>

#include <iostream>

std::optional<long long int> ParseIntInRange(
    const TokenText& token, long long int lowest, long long int highest) {
  std::string number_as_text(token.text, token.length);
  long long int as_long_int = strtol(number_as_text.c_str(), NULL, 10);

  std::cerr << "ParseIntRange(" << number_as_text << ", " << lowest << ", " << highest << ") = " << as_long_int << std::endl;

  return (as_long_int < lowest || as_long_int > highest) ?
             std::nullopt : std::optional<long long int>(as_long_int);
}
