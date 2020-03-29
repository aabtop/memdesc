#include "parse_results.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <sstream>
#include <variant>

#include "ast.h"
#include "ast_functions.h"
#include "parse_error.h"

namespace {
// When copying or merging ParseResults, the canonical definition for a given
// type may change, and this function is responsible for remapping type
// references from one pointer source to another.
void RemapStructTypePointers(
    const std::unordered_map<BaseType, BaseType>& remapping,
    std::vector<BaseType>::iterator begin,
    std::vector<BaseType>::iterator end) {
  for (auto iter = begin; iter != end; ++iter) {
    auto as_struct = AsStruct(*iter);
    if (as_struct == nullptr) {
      continue;
    }

    for (auto& f : as_struct->fields) {
      f.type.base_type = remapping.find(f.type.base_type)->second;
    }
  }
}
}  // namespace

ParseResults::ParseResults(const ParseResults& rhs) {
  // First copy the primitives and structs from |rhs| to the new ParseResults,
  // keeping track of the source and destination pointers as we go.
  std::unordered_map<BaseType, BaseType> remapping;

  for (const auto& item : rhs.declaration_order_) {
    if (const Primitive* as_primitive = AsPrimitive(item)) {
      auto copied_primitive = std::make_unique<Primitive>(*as_primitive);
      declaration_order_.push_back(copied_primitive.get());
      primitives_.insert({BaseTypeName(item), std::move(copied_primitive)});
    } else if (const Struct* as_struct = AsStruct(item)) {
      auto copied_struct = std::make_unique<Struct>(*as_struct);
      declaration_order_.push_back(copied_struct.get());
      structs_.insert({BaseTypeName(item), std::move(copied_struct)});
    }
    remapping[item] = declaration_order_.back();
  }

  // Now traverse through the copied AST and fix-up pointers to point to the
  // copied Structs.
  RemapStructTypePointers(remapping, declaration_order_.begin(),
                          declaration_order_.end());
}

std::optional<ParseErrorWithLocation> ParseResults::Merge(ParseResults&& src) {
  std::unordered_map<BaseType, BaseType> remappings;

  size_t original_num_declarations = declaration_order_.size();

  for (auto& merge_type : src.declaration_order_) {
    const std::string& merge_type_name = BaseTypeName(merge_type);

    if (auto existing_type = LookupBaseType(*this, merge_type_name)) {
      if (DefinedAt(*existing_type) == DefinedAt(merge_type)) {
        // If we found a pre-existing definition defined in the same place,
        // record the new location.
        remappings[merge_type] = *existing_type;
      } else {
        // We seem to have found two separate definitions across a merge,
        // return an error.
        return ParseErrorWithLocation{
            RedefinitionError{merge_type_name, DefinedAt(*existing_type)},
            DefinedAt(merge_type)};
      }
    } else {
      // No collisions, we're now safe to copy the definition over.
      remappings[merge_type] = merge_type;
      declaration_order_.push_back(merge_type);
      if (Primitive* as_primitive = AsPrimitive(merge_type)) {
        primitives_.insert(std::move(*src.primitives_.find(merge_type_name)));
      } else if (Struct* as_struct = AsStruct(merge_type)) {
        structs_.insert(std::move(*src.structs_.find(merge_type_name)));
      } else {
        assert(false);
      }
    }
  }

  RemapStructTypePointers(
      remappings, declaration_order_.begin() + original_num_declarations,
      declaration_order_.end());

  return std::nullopt;
}

void ParseResults::AddPrimitive(std::unique_ptr<Primitive> primitive) {
  auto found = primitives_.find(primitive->name);
  assert(found == primitives_.end());

  Primitive* as_ptr = primitive.get();
  declaration_order_.push_back(as_ptr);
  primitives_[as_ptr->name] = std::move(primitive);
}

void ParseResults::AddStruct(std::unique_ptr<Struct> s) {
  auto found = structs_.find(s->name);
  assert(found == structs_.end());

  Struct* as_ptr = s.get();
  declaration_order_.push_back(as_ptr);
  structs_[as_ptr->name] = std::move(s);
}

std::string ToString(const ParseResults& parse_results) {
  std::ostringstream oss;

  oss << "Primitives:" << std::endl;
  for (const auto& type : parse_results.primitives()) {
    oss << "  " << ToString(*type.second) << std::endl;
  }
  oss << std::endl;
  oss << "Structs:" << std::endl;
  for (const auto& type : parse_results.structs()) {
    oss << "  " << ToString(*type.second) << std::endl;
  }
  oss << std::endl;

  return oss.str();
}

std::optional<BaseType> LookupBaseType(const ParseResults& parse_results,
                                       const std::string& name) {
  auto found_primitive = parse_results.primitives().find(name);
  if (found_primitive != parse_results.primitives().end()) {
    return BaseType(found_primitive->second.get());
  }

  auto found_struct = parse_results.structs().find(name);
  if (found_struct != parse_results.structs().end()) {
    return BaseType(found_struct->second.get());
  }

  return std::nullopt;
}
