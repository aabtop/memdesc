#ifndef MEMDESC_DEPENDENCIES_FILE_H_
#define MEMDESC_DEPENDENCIES_FILE_H_

#include <filesystem>
#include <iostream>
#include <set>

void WriteDependenciesFile(const std::set<std::filesystem::path>& dependencies,
                           std::ostream& out);

#endif  // MEMDESC_DEPENDENCIES_FILE_H_
