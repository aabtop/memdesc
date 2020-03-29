#include "ast_functions.h"

#include <numeric>
#include <sstream>

template <typename F>
std::string JoinWith(const std::vector<F>& v, const std::string& join_string) {
  return std::accumulate(std::next(v.begin()), v.end(), ToString(v[0]),
                         [&join_string](const std::string& a, F b) {
                           return a + join_string + ToString(b);
                         });
}

std::string ToString(const SourceLocation& location) {
  std::ostringstream oss;

  oss << "(";

  if (location.filename) {
    oss << *location.filename << " : ";
  }

  oss << "line " << location.line_number << " : column "
      << location.column_number << ")";

  return oss.str();
}

std::string ToString(const Primitive& primitive) {
  std::ostringstream oss;
  oss << "Primitive(" << primitive.name << ", " << primitive.size << ", "
      << primitive.alignment << ")";
  return oss.str();
}

std::string ToString(const BaseType& base_type) {
  if (auto primitive = std::get_if<Primitive*>(&base_type)) {
    return ToString(**primitive);
  }
  if (auto struct_type = std::get_if<Struct*>(&base_type)) {
    return ToString(**struct_type);
  }

  return std::string("Unknown Type");
}

std::string ToString(const Type& type) {
  std::ostringstream oss;
  oss << "Type('" << BaseTypeName(type.base_type) << "'";
  if (type.array_count) {
    oss << ", " << *type.array_count;
  }
  oss << ")";
  return oss.str();
}

std::string ToString(const Field& field) {
  std::ostringstream oss;
  oss << "Field(" << ToString(field.type) << ", " << field.name << ")";
  return oss.str();
}

std::string ToString(const Struct& s) {
  return "Struct(" + s.name + ", {" + JoinWith(s.fields, ", ") + "})";
}

const std::string& BaseTypeName(const BaseType& base_type) {
  return std::visit([](auto& x) -> const std::string& { return x->name; },
                    base_type);
}

const SourceLocation& DefinedAt(const BaseType& base_type) {
  return std::visit(
      [](auto& x) -> const SourceLocation& { return x->defined_at; },
      base_type);
}

Primitive* AsPrimitive(BaseType& base_type) {
  if (std::holds_alternative<Primitive*>(base_type)) {
    return std::get<Primitive*>(base_type);
  }
  return nullptr;
}

const Primitive* AsPrimitive(const BaseType& base_type) {
  if (std::holds_alternative<Primitive*>(base_type)) {
    return std::get<Primitive*>(base_type);
  }
  return nullptr;
}

Struct* AsStruct(BaseType& base_type) {
  if (std::holds_alternative<Struct*>(base_type)) {
    return std::get<Struct*>(base_type);
  }
  return nullptr;
}

const Struct* AsStruct(const BaseType& base_type) {
  if (std::holds_alternative<Struct*>(base_type)) {
    return std::get<Struct*>(base_type);
  }
  return nullptr;
}
