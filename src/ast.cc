#include "ast.h"

#include <numeric>
#include <sstream>

template <typename F>
std::string ToString(const std::shared_ptr<F>& x) {
  return ToString(*x);
}

template <typename F>
std::string JoinWith(const std::vector<F>& v, const std::string& join_string) {
  return std::accumulate(std::next(v.begin()), v.end(), ToString(v[0]),
                         [&join_string](std::string a, F b) {
                           return a + join_string + ToString(b);
                         });
}

std::string ToString(const Primitive& primitive) {
  std::ostringstream oss;
  oss << "Primitive(" << primitive.name << ", "
      << primitive.size << ", " << primitive.alignment << ")";
  return oss.str();
}

std::string ToString(const BaseType& base_type) {
  if (auto primitive = std::get_if<Primitive*>(&base_type)) {
    return ToString(**primitive);
  } else if (auto struct_type = std::get_if<Struct*>(&base_type)) {
    return ToString(**struct_type);
  } else {
    return std::string("Unknown Type");
  }
}

std::string ToString(const Type& type) {
  std::ostringstream oss;
  oss << "Type(" << ToString(type.base_type) << ", " << "PointerDegree(" << type.pointer_degree << ")";
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
