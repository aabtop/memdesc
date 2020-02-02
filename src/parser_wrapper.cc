#include "parser_wrapper.h"

#include "lexer_wrapper.h"

#include "parser.tab.cc"

#include <iostream>

void yyerror(
		YYLTYPE *locp, void* my_parser, void* scanner, char const *msg) {
  std::cerr << "Error: " << locp->first_line << ":" << locp->first_column
            << " -- " << msg << std::endl;
}

