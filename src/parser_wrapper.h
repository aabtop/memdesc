#ifndef MEMDESC_PARSER_WRAPPER_H_
#define MEMDESC_PARSER_WRAPPER_H_

#include "ast.h"

#include "parser.tab.h"

void yyerror(
		YYLTYPE *locp, void* my_parser, void* scanner, char const *msg);

bool ParseFromBuffer(char* input, size_t size);

#endif  // MEMDESC_PARSER_WRAPPER_H_
