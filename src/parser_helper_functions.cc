#include "parser_helper_functions.h"

#include <string>

#include "ast_functions.h"
#include "parser_wrapper.h"

#include <iostream>

std::optional<int64_t> ParseIntInRange(
    const TokenText& token, int64_t lowest, int64_t highest) {
  std::string number_as_text(token.text, token.length);
  int64_t as_long_int = strtol(number_as_text.c_str(), NULL, 10);

  return (as_long_int < lowest || as_long_int > highest) ?
             std::nullopt : std::optional<int64_t>(as_long_int);
}

namespace {

template <typename T>
bool MergeParseResultList(
    const SourceLocation& source_location, const std::string& import_filename,
    const std::unordered_map<std::string, std::unique_ptr<T>>& src_list,
    std::unordered_map<std::string, std::unique_ptr<T>>* dest_list,
    ParseContext* parse_context) {
  for (const auto& item : src_list) {
    auto found = dest_list->find(item.first);
    if (found != dest_list->end()) {
      if (!(found->second->defined_at == item.second->defined_at)) {
        // We seem to have found two separate definitions between
        // two different files, raise an error and return in error.
        parse_context->error = ParseErrorWithLocation{
          ImportError{
              import_filename,
              std::unique_ptr<ParseErrorWithLocation>(
                  new ParseErrorWithLocation{
                      TypeRedefinition{
                          item.first,
                          found->second->defined_at
                      },
                      item.second->defined_at
                  })
          },
          source_location
        };
        return false;
      }
    } else {
      // No collisions, we're now safe to copy the importee's definition into
      // our outer definition set.
      (*dest_list)[item.first] = std::make_unique<T>(*item.second);
    }
  }

  return true;
}

// Used by imort statements to combine the definitions from an imported file
// into those from the file doing the importing.  It's preferred to do the
// merge *after* importing the file, so that definitions that exist in the
// importer, prior to the import, cannot affect the import results.
bool MergeParseResults(
    const SourceLocation& source_location, const std::string& import_filename,
    const ParseResults& merge_source, ParseContext* parse_context) {
  return MergeParseResultList(
             source_location, import_filename,
             merge_source.primitives, &parse_context->results.primitives,
             parse_context) &&
         MergeParseResultList(
             source_location, import_filename,
             merge_source.structs, &parse_context->results.structs,
             parse_context);
}


}  // namespace

bool ProcessImportStatement(
    ParseContext* parse_context, const std::string& filename,
    const SourceLocation& source_location) {
  std::cerr << "ProcessImportStatement(" << filename << ")" << std::endl;

  auto result_or_error = ParseFromFile(filename);

  if (auto error = std::get_if<ParseErrorWithLocation>(&result_or_error)) {
    // Propagate the error to the parse_context and return false to notify
    // the parser grammar logic that an error has occured.
    parse_context->error = ParseErrorWithLocation{
        ImportError{
            filename,
            std::make_unique<ParseErrorWithLocation>(std::move(*error))
        },
        source_location
    };

    return false;
  }

  // Merge the importee parse results into the main parse results.
  return MergeParseResults(
             source_location, filename, std::get<ParseResults>(result_or_error),
             parse_context);
}