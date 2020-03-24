#include <filesystem>
#include <iostream>
#include <string>

#include "parse_results.h"
#include "parser_wrapper.h"

int main(int argc, const char** args) {
  if (argc != 2) {
    std::cerr << "Usage: memdesc <INPUT FILE>" << std::endl;
    return 1;
  }

  std::filesystem::path input_filename(args[1]);
  if (input_filename.is_relative()) {
    input_filename = std::filesystem::current_path() / input_filename;
  }
  input_filename = std::filesystem::canonical(input_filename);

  // Parse out of a file if a filename is passed.
  auto results_or_error = ParseFromFile(input_filename);

  if (const auto error =
          std::get_if<ParseErrorWithLocation>(&results_or_error)) {
    std::cerr << ToString(*error) << std::endl;
    return 1;
  }

  const auto& results = std::get<ParseResults>(results_or_error);

  std::cout << "Successful parse.  Result: " << std::endl;
  std::cout << ToString(results) << std::endl << std::endl;
  return 0;
}
