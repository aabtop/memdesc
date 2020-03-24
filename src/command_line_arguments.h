#ifndef MEMDESC_COMMAND_LINE_ARGUMENTS_H_
#define MEMDESC_COMMAND_LINE_ARGUMENTS_H_

#include <filesystem>
#include <optional>

enum PreambleType {
  PreambleType_Default,
  PreambleType_None,

  PreambleType_End,
  PreambleType_Begin = 0,
};

struct CommandLineArguments {
  std::filesystem::path input_file;

  PreambleType preamble = PreambleType_Default;
};

std::optional<CommandLineArguments> ParseCommandLine(int argc,
                                                     const char** args);
void PrintUsage();

#endif  // MEMDESC_COMMAND_LINE_ARGUMENTS_H_
