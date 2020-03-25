#include "target_registry.h"

#include <cassert>

extern std::optional<TargetError> MakeASTDump(const ParseResults&,
                                              std::ostream&);

namespace {
TargetRegistry CreateTargetRegistry() {
  return TargetRegistry{
      {"ast_dump", MakeASTDump},
  };
}
}  // namespace

TargetRegistry* GetGlobalTargetRegistry() {
  static TargetRegistry s_registry = CreateTargetRegistry();
  return &s_registry;
}
