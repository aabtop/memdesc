#include "command_line_arguments.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string ToString(PreambleType preamble_type) {
  switch (preamble_type) {
    case PreambleType_Default: {
      return "default";
    } break;
    case PreambleType_None: {
      return "none";
    } break;
    case PreambleType_End:
      break;
  }

  assert(false);
  return "invalid";
}

std::optional<PreambleType> ParsePreamble(const std::string& preamble_str) {
  for (int i = PreambleType_Begin; i < PreambleType_End; ++i) {
    PreambleType as_preamble_type = static_cast<PreambleType>(i);
    if (ToString(as_preamble_type) == preamble_str) {
      return as_preamble_type;
    }
  }

  return std::nullopt;
}

std::string GetValidPreambleStrings() {
  std::ostringstream oss;
  oss << "(";
  for (int i = PreambleType_Begin; i < PreambleType_End; ++i) {
    PreambleType as_preamble_type = static_cast<PreambleType>(i);
    oss << ToString(as_preamble_type);
    if (i < PreambleType_End - 1) {
      oss << ", ";
    }
  }
  oss << ")";
  return oss.str();
}

std::optional<std::filesystem::path> CanonicalizeFile(
    const std::filesystem::path& file_path) {
  std::filesystem::path absolute_file_path =
      file_path.is_relative() ? std::filesystem::current_path() / file_path
                              : file_path;

  // Make sure that the file exists and we can open it.
  std::ifstream file_handle(absolute_file_path);
  if (!file_handle) {
    return std::nullopt;
  }

  return std::filesystem::canonical(absolute_file_path);
}

}  // namespace

std::optional<CommandLineArguments> ParseCommandLine(int argc,
                                                     const char** args) {
  CommandLineArguments result;

  bool parsed_input_file = false;
  for (int i = 1; i < argc; ++i) {
    std::string current_arg(args[i]);

    if (current_arg == "-p" || current_arg == "--preamble") {
      if (++i >= argc) {
        std::cerr << "Missing parameter for '--preamble'." << std::endl;
        return std::nullopt;
      }
      std::optional<PreambleType> preamble = ParsePreamble(args[i]);
      if (!preamble) {
        std::cerr << "Invalid '--preamble' parameter." << std::endl;
        std::cerr << "Valid values are: " << GetValidPreambleStrings()
                  << std::endl;
        return std::nullopt;
      }
      result.preamble = *preamble;
      continue;
    }

    // Handle the positional required arguments.
    if (parsed_input_file) {
      return std::nullopt;
    }
    parsed_input_file = true;
    auto canonicalized_input_file = CanonicalizeFile(current_arg);
    if (!canonicalized_input_file) {
      std::cerr << "Invalid input file: " << current_arg << std::endl;
      return std::nullopt;
    }

    result.input_file = *canonicalized_input_file;
  }

  if (!parsed_input_file) {
    std::cerr << "Input file not provided as command line parameter."
              << std::endl;
    return std::nullopt;
  }

  return result;
}

void PrintUsage() {
  std::cerr << "Usage: memdesc <INPUT FILE> [-p | --preamble PREAMBLE_VALUE]"
            << std::endl
            << std::endl;
  std::cerr << "  INPUT FILE: The file to be parsed." << std::endl;
  std::cerr << "  [-p | --preamble PREAMBLE_VALUE]: Indicates whether or not a "
               "'preamble' source file will be parsed to provide definitions "
               "for a basic set of types.  May be one of: "
            << GetValidPreambleStrings() << std::endl;
  std::cerr << std::endl;
}
