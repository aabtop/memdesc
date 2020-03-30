#include "dependencies_file.h"

void WriteDependenciesFile(const std::set<std::filesystem::path>& dependencies,
                           std::ostream& out) {
  for (const auto& path : dependencies) {
    out << path.string() << std::endl;
  }
}
