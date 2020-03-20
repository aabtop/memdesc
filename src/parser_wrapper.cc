#include <cassert>

#include <iostream>

#include "parser_wrapper.h"
#include "lexer_wrapper.h"

#include "parse_results.h"
#include "parser.tab.cc"

void yyerror(
		YYLTYPE *locp, ParseResults* parse_results, void* scanner,
    char const *msg) {
  parse_results->error = Error({
      {
        locp->first_line,
        locp->first_column,
      },
      SyntaxError{msg},
  });
}

ParseResults ParseFromBuffer(char* input, size_t size) {
  ParseResults parse_results;

  if (size < 2 || input[size-2] != '\0' || input[size-1] != '\0') {
    std::cerr << "Invalid input buffer, the last two characters must be '\\0'."
              << std::endl;
    return std::move(parse_results);
  }

  yyscan_t scanner;
  yylex_init(&scanner);
  YY_BUFFER_STATE scan_buffer = yy_scan_buffer(input, size, scanner);
  if (scan_buffer == NULL) {
    std::cerr << "Unexpected error when setting up scan buffer." << std::endl;
    return std::move(parse_results);
  }

  int yyparse_result =
      yyparse(&parse_results, scanner);
  yylex_destroy(scanner);

  if (yyparse_result != 0) {
    assert(parse_results.error);
    return std::move(parse_results);
  }

  if (parse_results.error) {
    std::cerr << "Unexpected error without error code." << std::endl;
    return std::move(parse_results);
  }

  return std::move(parse_results);
}
