#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "command_line_arguments.h"
#include "dependencies_file.h"
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
  ParseResults* preamble_results_as_ptr =
      (preamble_results ? &(*preamble_results) : nullptr);

  // Parse out of a file if a filename is passed.
  auto results_or_error =
      ParseFromFile(arguments->input_file, preamble_results_as_ptr);

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

  // If provided, open the output files and make sure there are no errors.
  std::optional<std::ofstream> possible_output_file;
  if (arguments->output_file) {
    possible_output_file.emplace(arguments->output_file->c_str());
    if (!*possible_output_file) {
      std::cerr << "Error opening output file, " << *arguments->output_file
                << " for writing." << std::endl;
      return 1;
    }
  }
  std::optional<std::ofstream> possible_deps_file;
  if (arguments->dependency_file) {
    possible_deps_file.emplace(arguments->dependency_file->c_str());
    if (!*possible_deps_file) {
      std::cerr << "Error opening dependency file, "
                << *arguments->dependency_file << " for writing." << std::endl;
      return 1;
    }
  }

  // Finally, write output to the output files.
  if (possible_output_file) {
    *possible_output_file << oss.str();
  } else {
    std::cout << oss.str();
  }
  if (possible_deps_file) {
    WriteDependenciesFile(results.dependencies(), *possible_deps_file);
  }

  return 0;
}
