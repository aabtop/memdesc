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
    const std::unordered_map<Primitive*, Primitive*>& primitive_remapping,
    const std::unordered_map<Struct*, Struct*>& struct_remapping,
    const std::vector<Struct*>& structs_to_remap) {
  for (auto& to_remap : structs_to_remap) {
    for (auto& f : to_remap->fields) {
      if (std::holds_alternative<Primitive*>(f.type.base_type)) {
        f.type.base_type =
            primitive_remapping.find(std::get<Primitive*>(f.type.base_type))
                ->second;
      } else if (std::holds_alternative<Struct*>(f.type.base_type)) {
        f.type.base_type =
            struct_remapping.find(std::get<Struct*>(f.type.base_type))->second;
      } else {
        assert(false);
      }
    }
  }
}
}  // namespace

ParseResults::ParseResults(const ParseResults& rhs) {
  // First copy the primitives and structs from |rhs| to the new ParseResults,
  // keeping track of the source and destination pointers as we go.
  std::unordered_map<Primitive*, Primitive*> primitive_remapping;
  std::unordered_map<Struct*, Struct*> struct_remapping;

  for (const auto& item : rhs.primitives) {
    auto copied_item = std::make_unique<Primitive>(*item.second);
    primitive_remapping[item.second.get()] = copied_item.get();
    primitives.insert({item.first, std::move(copied_item)});
  }
  for (const auto& item : rhs.structs) {
    auto copied_item = std::make_unique<Struct>(*item.second);
    struct_remapping[item.second.get()] = copied_item.get();
    structs.insert({item.first, std::move(copied_item)});
  }

  // Now traverse through the copied AST and fix-up pointers to point to the
  // copied Structs.
  std::vector<Struct*> all_structs;
  std::transform(structs.begin(), structs.end(),
                 std::back_inserter(all_structs),
                 [](const auto& item) { return item.second.get(); });
  RemapStructTypePointers(primitive_remapping, struct_remapping, all_structs);
}

namespace {
template <typename T>
struct MergeListResults {
  std::unordered_map<T*, T*> remappings;

  // The list of items that are newly merged over, as opposed to items that
  // did not get merged because a definition of the item already existed in
  // the merge destination.
  std::vector<T*> merged;
};

template <typename T>
std::variant<MergeListResults<T>, ParseErrorWithLocation> MergeParseResultList(
    std::unordered_map<std::string, std::unique_ptr<T>>&& src_list,
    std::unordered_map<std::string, std::unique_ptr<T>>* dest_list) {
  size_t src_list_size = src_list.size();

  MergeListResults<T> results;
  for (auto&& item : src_list) {
    auto found = dest_list->find(item.first);
    if (found != dest_list->end()) {
      if (found->second->defined_at == item.second->defined_at) {
        // If we found a pre-existing definition defined in the same place,
        // record the new location.
        results.remappings[item.second.get()] = found->second.get();
      } else {
        // We seem to have found two separate definitions across a merge,
        // return an error.
        return ParseErrorWithLocation{
            RedefinitionError{item.first, found->second->defined_at},
            item.second->defined_at};
      }
    } else {
      // No collisions, we're now safe to copy the definition over.
      results.remappings[item.second.get()] = item.second.get();
      results.merged.push_back(item.second.get());
      (*dest_list)[item.first] = std::move(item.second);
    }
  }

  assert(results.remappings.size() == src_list_size);
  return results;
}
}  // namespace

std::optional<ParseErrorWithLocation> ParseResults::Merge(ParseResults&& src) {
  auto primitive_merge_results =
      MergeParseResultList(std::move(src.primitives), &primitives);
  if (std::holds_alternative<ParseErrorWithLocation>(primitive_merge_results)) {
    return std::move(std::get<ParseErrorWithLocation>(primitive_merge_results));
  }

  auto struct_merge_results =
      MergeParseResultList(std::move(src.structs), &structs);
  if (std::holds_alternative<ParseErrorWithLocation>(struct_merge_results)) {
    return std::move(std::get<ParseErrorWithLocation>(struct_merge_results));
  }

  RemapStructTypePointers(
      std::get<MergeListResults<Primitive>>(primitive_merge_results).remappings,
      std::get<MergeListResults<Struct>>(struct_merge_results).remappings,
      std::get<MergeListResults<Struct>>(struct_merge_results).merged);
  return std::nullopt;
}

std::string ToString(const ParseResults& parse_results) {
  std::ostringstream oss;

  oss << "Primitives:" << std::endl;
  for (const auto& type : parse_results.primitives) {
    oss << "  " << ToString(*type.second) << std::endl;
  }
  oss << std::endl;
  oss << "Structs:" << std::endl;
  for (const auto& type : parse_results.structs) {
    oss << "  " << ToString(*type.second) << std::endl;
  }
  oss << std::endl;

  return oss.str();
}

std::optional<BaseType> LookupBaseType(const ParseResults& parse_results,
                                       const std::string& name) {
  auto found_primitive = parse_results.primitives.find(name);
  if (found_primitive != parse_results.primitives.end()) {
    return BaseType(found_primitive->second.get());
  }

  auto found_struct = parse_results.structs.find(name);
  if (found_struct != parse_results.structs.end()) {
    return BaseType(found_struct->second.get());
  }

  return std::nullopt;
}
