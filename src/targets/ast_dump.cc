#include <iostream>
#include <optional>

#include "parse_results.h"
#include "target_registry.h"

std::optional<TargetError> MakeASTDump(const ParseResults& parse_results,
                                       std::ostream& out) {
  out << ToString(parse_results) << std::endl;
  return std::nullopt;
}
