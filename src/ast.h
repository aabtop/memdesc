#ifndef MEMDESC_AST_H_
#define MEMDESC_AST_H_

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

struct Field {
  Field(const std::string& type, const std::string& name,
        std::optional<unsigned int> count)
      : type(type), name(name), count(count) {
        std::cout << "Creating " << ToString() << std::endl;
      }
  ~Field() {
    std::cout << "Destroying " << ToString() << std::endl;
  }

  std::string ToString() const {
    std::ostringstream oss;
    oss << "Field(" << type << ", " << name;
    if (count) {
      oss << ", " << *count;
    }
    oss << ")";
    return oss.str();
  }

  std::string type;
  std::string name;
  std::optional<unsigned int> count;
};

struct Struct {
  Struct(const std::string& name, const std::vector<Field>& fields)
      : name(name), fields(fields) {
    std::cout << "Creating " << ToString() << std::endl;
  }
  ~Struct() {
    std::cout << "Destroying " << ToString() << std::endl;
  }

  std::string ToString() const {
    std::ostringstream oss;
    oss << "Struct(" << name << ", {";
    for (const auto& field : fields) {
      oss << field.ToString() << ", ";
    }
    oss << "})";
    return oss.str();
  }

  std::string name;
  std::vector<Field> fields;  
};

#endif  // MEMDESC_AST_H_
