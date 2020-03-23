#ifndef MEMDESC_PARSER_HELPER_FUNCTIONS_H_
#define MEMDESC_PARSER_HELPER_FUNCTIONS_H_

#include <filesystem>
#include <optional>
#include <string>

#include "parse_results.h"

struct ParseContext {
  const std::optional<std::filesystem::path> filename;

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

std::optional<int64_t> ParseIntInRange(
    const TokenText& token, int64_t lowest, int64_t highest);

bool ProcessImportStatement(
    ParseContext* parse_context, const std::filesystem::path& filename,
    const SourceLocation& source_location);

#endif  // MEMDESC_PARSER_HELPER_FUNCTIONS_H_
