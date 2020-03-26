#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "command_line_arguments.h"
#include "parse_results.h"
#include "parser_wrapper.h"
#include "target_registry.h"

int main(int argc, const char** args) {
  std::optional<CommandLineArguments> arguments = ParseCommandLine(argc, args);
  if (!arguments) {
    std::cerr << std::endl;
    PrintUsage();
    return 1;
  }

  // Parse the preamble file to load some initial primitive type definitions
  // (e.g. 'float' and 'char') in before we parse user data.  This preamble
  // loading can be disabled via a command line switch, so we check that first.
  std::optional<ParseResults> preamble_results;
  if (arguments->preamble != PreambleType_None) {
    const std::filesystem::path executable_dir(
        std::filesystem::absolute(args[0]).parent_path());
    const std::filesystem::path preamble_file(
        executable_dir /
        ("preamble_" + ToString(arguments->preamble) + ".mdesc"));

    auto preamble_results_or_error = ParseFromFile(preamble_file);

    if (const auto error =
            std::get_if<ParseErrorWithLocation>(&preamble_results_or_error)) {
      std::cerr << "Error loading preamble file: " << preamble_file
                << std::endl;
      std::cerr << ToString(*error) << std::endl;
      return 1;
    }
    preamble_results =
        std::move(std::get<ParseResults>(preamble_results_or_error));
  }

  // Parse out of a file if a filename is passed.
  auto results_or_error =
      ParseFromFile(arguments->input_file, std::move(preamble_results));

  if (const auto error =
          std::get_if<ParseErrorWithLocation>(&results_or_error)) {
    std::cerr << ToString(*error) << std::endl;
    return 1;
  }

  const auto& results = std::get<ParseResults>(results_or_error);

  // Write the results into an in-memory buffer so that we can later decide
  // where those results end up going (and if there's an error in generating
  // the output we can avoid creating a file that ends up being corrupt).
  std::ostringstream oss;
  auto possible_error =
      (*GetGlobalTargetRegistry())[arguments->target](results, oss);

  if (possible_error) {
    std::cerr << "Error generating output for target '" << arguments->target
              << "': " << *possible_error << std::endl;
    return 1;
  }

  std::ostream* out = &std::cout;
  std::optional<std::ofstream> possible_output_file;
  if (arguments->output_file) {
    std::ofstream out_file(arguments->output_file->c_str());
    if (!out_file) {
      std::cerr << "Error opening output file, " << *arguments->output_file
                << " for writing." << std::endl;
      return 1;
    }
    out_file << oss.str();
  } else {
    std::cout << oss.str();
  }

  return 0;
}
