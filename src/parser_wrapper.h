#ifndef MEMDESC_PARSER_WRAPPER_H_
#define MEMDESC_PARSER_WRAPPER_H_

#include "ast.h"
#include "parse_results.h"
#include "parser.tab.h"

void yyerror(
		YYLTYPE *locp, void* my_parser, void* scanner, char const *msg);

std::optional<ParseResults> ParseFromBuffer(
    char* input, size_t size,
    std::unordered_map<std::string, std::unique_ptr<Primitive>>&&
        initial_primitives);

#endif  // MEMDESC_PARSER_WRAPPER_H_
