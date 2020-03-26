#include "target_registry.h"

#include <cassert>

extern std::optional<TargetError> OutputASTDump(const ParseResults&,
                                                std::ostream&);
extern std::optional<TargetError> OutputC(const ParseResults&, std::ostream&);

namespace {
TargetRegistry CreateTargetRegistry() {
  return TargetRegistry{{"ast_dump", OutputASTDump}, {"c", OutputC}};
}
}  // namespace

TargetRegistry* GetGlobalTargetRegistry() {
  static TargetRegistry s_registry = CreateTargetRegistry();
  return &s_registry;
}
