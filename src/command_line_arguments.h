#ifndef MEMDESC_COMMAND_LINE_ARGUMENTS_H_
#define MEMDESC_COMMAND_LINE_ARGUMENTS_H_

#include <filesystem>
#include <optional>
#include <string>

enum PreambleType {
  PreambleType_Default,
  PreambleType_None,

  PreambleType_End,
  PreambleType_Begin = 0,
};
std::string ToString(PreambleType preamble_type);

struct CommandLineArguments {
  std::filesystem::path input_file;

  PreambleType preamble = PreambleType_Default;
  std::string target;
  std::optional<std::filesystem::path> output_file;
  std::optional<std::filesystem::path> dependency_file;
};

std::optional<CommandLineArguments> ParseCommandLine(int argc,
                                                     const char** args);
void PrintUsage();

#endif  // MEMDESC_COMMAND_LINE_ARGUMENTS_H_
