#include <cassert>
#include <iostream>
#include <string>

// Must be defined before including lexer.yy.h.
#define YY_NO_UNISTD_H
#include "parser.tab.h"
//#define YYSTYPE MEMDESC_YYSTYPE
#include "lexer.yy.h"

/*
void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}
*/

void yyerror(YYLTYPE *locp, void* my_parser, void* scanner, char const *msg) {
  std::cerr << "Error: " << locp->first_line << ":" << locp->first_column
            << " -- " << msg << std::endl;
}

int main() {
  std::string line;
  while(true) {
    std::getline(std::cin, line);
    if (line.empty()) {
      break;
    }

    line += '\n';
    line += '\0';
    line += '\0';

    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE scan_buffer = yy_scan_buffer(
        const_cast<char*>(line.c_str()), line.size(), scanner);
    
    // Possibly forgot the last two null bytes expected by flex.
    assert(scan_buffer != NULL);

    yyparse(0, scanner);
    yylex_destroy(scanner);
	}

	return 0;
}
