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

    ParseFromBuffer(const_cast<char*>(line.c_str()), line.size());
	}

	return 0;
}
