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
  parse_context->error = ParseErrorWithLocation({
      SyntaxError{msg},
      {
        locp->first_line,
        locp->first_column,
        parse_context->filename
      },
  });
}

ParseResultsOrError ParseFromBuffer(
    char* input, size_t size, const std::optional<std::string>& filename) {
  if (size < 2 || input[size-2] != '\0' || input[size-1] != '\0') {
    return ParseErrorWithLocation{
        GenericError{
            "Invalid input buffer, the last two characters must be '\\0'."},
        SourceLocation{1, 1, filename}};
  }

  yyscan_t scanner;
  yylex_init(&scanner);
  YY_BUFFER_STATE scan_buffer = yy_scan_buffer(input, size, scanner);
  if (scan_buffer == NULL) {
    return ParseErrorWithLocation{
        GenericError{
            "Unexpected error when setting up scan buffer."},
        SourceLocation{1, 1, filename}};
  }

  ParseContext parse_context{filename};

  int yyparse_result = yyparse(&parse_context, scanner);
  yylex_destroy(scanner);

  if (yyparse_result != 0 || parse_context.error) {
    assert(parse_context.error);
    return std::move(*parse_context.error);
  }

  return std::move(parse_context.results);
}

ParseResultsOrError ParseFromFile(const std::string& filename) {
  std::ifstream in_file(filename);
  if (!in_file) {
    return ParseErrorWithLocation{FileOpenError{filename}, {0, 0}};
  }
  std::ostringstream buffer;
  buffer << in_file.rdbuf();
  in_file.close();

  std::string file_contents = buffer.str();
  file_contents += '\0';
  file_contents += '\0';

  return ParseFromBuffer(file_contents.data(), file_contents.size(), filename);
}