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

    if (!ParseFromBuffer(const_cast<char*>(line.c_str()), line.size())) {
      std::cerr << "Error parsing buffer." << std::endl;
      return 1;
    }
	}

	return 0;
}
