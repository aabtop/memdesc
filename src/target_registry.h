#ifndef MEMDESC_TARGET_REGISTRY_H_
#define MEMDESC_TARGET_REGISTRY_H_

#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

#include "parse_results.h"

using TargetError = std::string;
using TargetFunction = std::function<std::optional<TargetError>(
    const ParseResults&, std::ostream&)>;
using TargetRegistry = std::unordered_map<std::string, TargetFunction>;

TargetRegistry* GetGlobalTargetRegistry();

#endif  // MEMDESC_TARGET_REGISTRY_H_
