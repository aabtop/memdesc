#ifndef MEMDESC_AST_H_
#define MEMDESC_AST_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct Field {
  std::string type;
  std::string name;
  std::optional<unsigned int> count;
};

std::string ToString(const Field& field);

struct Struct {
  std::string name;
  std::vector<Field> fields;  
};

std::string ToString(const Struct& s);

using StructDeclarationList = std::vector<std::shared_ptr<Struct>>;

std::string ToString(const StructDeclarationList& list);

#endif  // MEMDESC_AST_H_
