#include "target_registry.h"

std::optional<TargetError> OutputASTDump(const ParseResults& parse_results,
                                         std::ostream& out) {
  out << ToString(parse_results) << std::endl;
  return std::nullopt;
}
