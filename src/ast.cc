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

std::string ToString(const Field& field) {
  std::ostringstream oss;
  oss << "Field(" << field.type << ", " << field.name;
  if (field.count) {
    oss << ", " << *field.count;
  }
  oss << ")";
  return oss.str();
}

std::string ToString(const Struct& s) {
  return "Struct(" + s.name + ", {" + JoinWith(s.fields, ", ") + "})";
}

std::string ToString(const StructDeclarationList& list) {
  return "StructDeclarationList({" + JoinWith(list, ", ") + "})";
}
