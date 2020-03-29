#ifndef MEMDESC_PARSER_WRAPPER_H_
#define MEMDESC_PARSER_WRAPPER_H_

#include <filesystem>
#include <optional>

#include "ast.h"
#include "parse_results.h"
#include "parser_helper_functions.h"

// This must be included after all other headers.
#include "parser.tab.h"

void yyerror(YYLTYPE* locp, ParseContext* parse_context, void* scanner,
             char const* msg);

ParseResultsOrError ParseFromBuffer(
    char* input, size_t size, const ParseResults* initial_context = nullptr,
    const std::optional<std::filesystem::path>& filename = std::nullopt);
ParseResultsOrError ParseFromFile(
    const std::filesystem::path& filename,
    const ParseResults* initial_context = nullptr);

#endif  // MEMDESC_PARSER_WRAPPER_H_
