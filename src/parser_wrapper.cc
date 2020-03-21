#include <cassert>

#include <fstream>
#include <iostream>
#include <sstream>

#include "parser_wrapper.h"
#include "lexer_wrapper.h"

#include "parse_results.h"
#include "parser.tab.cc"

void yyerror(
		YYLTYPE *locp, ParseContext* parse_context, void* scanner,
    char const *msg) {
  parse_context->results.error = Error({
      {
        locp->first_line,
        locp->first_column,
        parse_context->filename
      },
      SyntaxError{msg},
  });
}

ParseResults ParseFromBuffer(
    char* input, size_t size, const std::optional<std::string>& filename) {
  ParseContext parse_context{filename};

  if (size < 2 || input[size-2] != '\0' || input[size-1] != '\0') {
    std::cerr << "Invalid input buffer, the last two characters must be '\\0'."
              << std::endl;
    return std::move(parse_context.results);
  }

  yyscan_t scanner;
  yylex_init(&scanner);
  YY_BUFFER_STATE scan_buffer = yy_scan_buffer(input, size, scanner);
  if (scan_buffer == NULL) {
    std::cerr << "Unexpected error when setting up scan buffer." << std::endl;
    return std::move(parse_context.results);
  }

  int yyparse_result = yyparse(&parse_context, scanner);
  yylex_destroy(scanner);

  if (yyparse_result != 0) {
    assert(parse_context.results.error);
    return std::move(parse_context.results);
  }

  if (parse_context.results.error) {
    std::cerr << "Unexpected error without error code." << std::endl;
    return std::move(parse_context.results);
  }

  return std::move(parse_context.results);
}

ParseResults ParseFromFile(const std::string& filename) {
  std::ifstream in_file(filename);
  std::ostringstream buffer;
  buffer << in_file.rdbuf() << "\0\0";
  in_file.close();

  std::string file_contents = buffer.str();

  return ParseFromBuffer(file_contents.data(), file_contents.size(), filename);
}