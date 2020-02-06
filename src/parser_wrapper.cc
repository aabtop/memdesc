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

std::optional<StructDeclarationList> ParseFromBuffer(
    char* input, size_t size) {
  if (size < 2 || input[size-2] != '\0' || input[size-1] != '\0') {
    std::cerr << "Invalid input buffer, the last two characters must be '\\0'."
              << std::endl;
    return std::nullopt;
  }

  yyscan_t scanner;
  yylex_init(&scanner);
  YY_BUFFER_STATE scan_buffer = yy_scan_buffer(input, size, scanner);
  if (scan_buffer == NULL) {
    std::cerr << "Unexpected error when setting up scan buffer." << std::endl;
    return std::nullopt;
  }

  StructDeclarationList* final_output = nullptr;

  int yyparse_result = yyparse(reinterpret_cast<void**>(&final_output), scanner);
  yylex_destroy(scanner);

  if (yyparse_result != 0) {
    std::cerr << "yyparse() returned in failure." << std::endl;
    return std::nullopt;
  }

  if (final_output == nullptr) {
    std::cerr << "Unexpected null value as final output." << std::endl;
    return std::nullopt;
  }

  StructDeclarationList ret_val(std::move(*final_output));
  delete final_output;

  return ret_val;
}