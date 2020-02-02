#include <cassert>
#include <iostream>
#include <string>

#include "parser_wrapper.h"
#include "lexer_wrapper.h"

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
