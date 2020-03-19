#ifndef MEMDESC_AST_H_
#define MEMDESC_AST_H_

#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Primitive {
  std::string name;
  int size;
  int alignment;
};

struct Struct;

using BaseType =
    std::variant<Struct*, Primitive*>;

struct Type {
  BaseType base_type;
  int pointer_degree;
  std::optional<unsigned int> array_count;
};

struct Field {
  Type type;
  std::string name;
};

struct Struct {
  std::string name;
  std::vector<Field> fields;  
};

#endif  // MEMDESC_AST_H_
