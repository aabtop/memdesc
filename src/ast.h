#ifndef MEMDESC_AST_H_
#define MEMDESC_AST_H_

#include <memory>
#include <optional>
#include <string>

#include <iostream>

struct Field {
  Field(const char* type, const char* name, std::optional<unsigned int> count)
      : type(type), name(name), count(count) {
        std::cout << "Field(" << type << ", " << name << ")" << std::endl;
      }
  ~Field() {
    std::cout << "Destroying Field()" << std::endl;
  }

  std::string type;
  std::string name;
  std::optional<unsigned int> count;
};

#endif  // MEMDESC_AST_H_
