#ifndef MEMDESC_PARSER_HELPER_FUNCTIONS_H_
#define MEMDESC_PARSER_HELPER_FUNCTIONS_H_

#include <optional>

struct TokenText {
  const char* text;
  int length;
};

std::optional<long long int> ParseIntInRange(
    const TokenText& token, long long int lowest, long long int highest);

#endif  // MEMDESC_PARSER_HELPER_FUNCTIONS_H_
