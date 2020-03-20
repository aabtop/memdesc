#ifndef MEMDESC_PARSER_WRAPPER_H_
#define MEMDESC_PARSER_WRAPPER_H_

#include "ast.h"
#include "parse_results.h"
#include "parser_helper_functions.h"
#include "parser.tab.h"

void yyerror(
		YYLTYPE *locp, ParseResults* parse_results, void* scanner, char const *msg);

ParseResults ParseFromBuffer(char* input, size_t size);

#endif  // MEMDESC_PARSER_WRAPPER_H_
