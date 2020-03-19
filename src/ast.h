#ifndef MEMDESC_AST_H_
#define MEMDESC_AST_H_

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Primitive {
  std::string name;
  int size;
  int alignment;
};

std::string ToString(const Primitive& primitive);

struct Struct;

using BaseType =
    std::variant<Struct*, Primitive*>;

std::string ToString(const BaseType& base_type);

struct Type {
  BaseType base_type;
  int pointer_degree;
  std::optional<unsigned int> array_count;
};

std::string ToString(const Type& type);

struct Field {
  Type type;
  std::string name;
};

std::string ToString(const Field& field);

struct Struct {
  std::string name;
  std::vector<Field> fields;  
};

std::string ToString(const Struct& s);

#endif  // MEMDESC_AST_H_
