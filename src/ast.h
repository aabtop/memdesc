#ifndef MEMDESC_AST_H_
#define MEMDESC_AST_H_

#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct SourceLocation {
  int line_number;
  int column_number;
  std::optional<std::filesystem::path> filename;
};

struct Primitive {
  std::string name;
  int size;
  int alignment;

  SourceLocation defined_at;
};

struct Struct;

using BaseType = std::variant<Struct*, Primitive*>;

struct Type {
  BaseType base_type;
  std::optional<int> array_count;
};

struct Field {
  Type type;
  std::string name;

  SourceLocation defined_at;
};

struct Struct {
  std::string name;
  std::vector<Field> fields;

  SourceLocation defined_at;
};

#endif  // MEMDESC_AST_H_
