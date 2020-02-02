#include <cassert>

#include <iostream>

#include "parser_wrapper.h"
#include "lexer_wrapper.h"

#include "parser.tab.cc"

void yyerror(
		YYLTYPE *locp, void* my_parser, void* scanner, char const *msg) {
  std::cerr << "Error: " << locp->first_line << ":" << locp->first_column
            << " -- " << msg << std::endl;
}

bool ParseFromBuffer(char* input, size_t size) {
  if (size < 2 || input[size-2] != '\0' || input[size-1] != '\0') {
    std::cerr << "Invalid input buffer, the last two characters must be '\\0'."
              << std::endl;
    return false;
  }

  yyscan_t scanner;
  yylex_init(&scanner);
  YY_BUFFER_STATE scan_buffer = yy_scan_buffer(input, size, scanner);
  if (scan_buffer == NULL) {
    std::cerr << "Unexpected error when setting up scan buffer." << std::endl;
    return false;
  }

  yyparse(0, scanner);
  yylex_destroy(scanner);
}