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

    auto result = ParseFromBuffer(const_cast<char*>(line.c_str()), line.size());
    if (!result) {
      std::cerr << "Error parsing buffer." << std::endl;
      return 1;
    }

    std::cout << "Successful parse.  Result: " << std::endl;
    std::cout << ToString(*result) << std::endl << std::endl;
	}

	return 0;
}
