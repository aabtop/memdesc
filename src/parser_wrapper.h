#ifndef MEMDESC_PARSER_WRAPPER_H_
#define MEMDESC_PARSER_WRAPPER_H_

#include "ast.h"
#include "parse_results.h"
#include "parser_helper_functions.h"
#include "parser.tab.h"

void yyerror(
		YYLTYPE *locp, ParseContext* parse_context, void* scanner, char const *msg);

ParseResults ParseFromBuffer(
		char* input, size_t size,
		const std::optional<std::string>& filename = std::nullopt);
ParseResults ParseFromFile(const std::string& filename);

#endif  // MEMDESC_PARSER_WRAPPER_H_
