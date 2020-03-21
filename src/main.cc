#include <iostream>
#include <string>

#include "parse_results.h"
#include "parser_wrapper.h"
#include "lexer_wrapper.h"

int main(int argc, const char** args) {
  if (argc >= 2) {
    // Parse out of a file if a filename is passed.
    auto result = ParseFromFile(args[1]);

    if (result.error) {
      std::cerr << ToString(*result.error) << std::endl;
      return 1;
    }  

    std::cout << "Successful parse.  Result: " << std::endl;
    std::cout << ToString(result) << std::endl << std::endl;
    return 0;
  } else {
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
      if (result.error) {
        std::cerr << ToString (*result.error) << std::endl;
        return 1;
      }

      std::cout << "Successful parse.  Result: " << std::endl;
      std::cout << ToString(result) << std::endl << std::endl;
    }

    return 0;
  }
}
