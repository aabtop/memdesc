#include <filesystem>
#include <iostream>
#include <string>

#include "command_line_arguments.h"
#include "parse_results.h"
#include "parser_wrapper.h"

int main(int argc, const char** args) {
  std::optional<CommandLineArguments> arguments = ParseCommandLine(argc, args);
  if (!arguments) {
    std::cerr << std::endl;
    PrintUsage();
    return 1;
  }

  // Parse out of a file if a filename is passed.
  auto results_or_error = ParseFromFile(arguments->input_file);

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
