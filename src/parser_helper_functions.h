#ifndef MEMDESC_PARSER_HELPER_FUNCTIONS_H_
#define MEMDESC_PARSER_HELPER_FUNCTIONS_H_

#include <optional>
#include <string>

#include "parse_results.h"

struct ParseContext {
  const std::optional<std::string> filename;

  ParseResults results;

  std::optional<ParseErrorWithLocation> error;

  // Is the parse completed?  True implies not only completion but also success.
  bool complete = false;
};

using ParseResultsOrError = std::variant<ParseResults, ParseErrorWithLocation>;

struct TokenText {
  const char* text;
  int length;
};

std::optional<long long int> ParseIntInRange(
    const TokenText& token, long long int lowest, long long int highest);

#endif  // MEMDESC_PARSER_HELPER_FUNCTIONS_H_
