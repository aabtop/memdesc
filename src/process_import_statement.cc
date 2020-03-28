#include "process_import_statement.h"

#include <fstream>
#include <string>

#include "ast_functions.h"
#include "parser_wrapper.h"

namespace {

template <typename T>
bool MergeParseResultList(
    const SourceLocation& source_location,
    const std::filesystem::path& import_filename,
    std::unordered_map<std::string, std::unique_ptr<T>>&& src_list,
    std::unordered_map<std::string, std::unique_ptr<T>>* dest_list,
    ParseContext* parse_context) {
  for (auto&& item : src_list) {
    auto found = dest_list->find(item.first);
    if (found != dest_list->end()) {
      if (!(found->second->defined_at == item.second->defined_at)) {
        // We seem to have found two separate definitions between
        // two different files, raise an error and return in error.
        parse_context->error = ParseErrorWithLocation{
            ImportError{import_filename,
                        std::unique_ptr<ParseErrorWithLocation>(
                            new ParseErrorWithLocation{
                                RedefinitionError{item.first,
                                                  found->second->defined_at},
                                item.second->defined_at})},
            source_location};
        return false;
      }
    } else {
      // No collisions, we're now safe to copy the importee's definition into
      // our outer definition set.
      (*dest_list)[item.first] = std::move(item.second);
    }
  }

  return true;
}

// Used by imort statements to combine the definitions from an imported file
// into those from the file doing the importing.  It's preferred to do the
// merge *after* importing the file, so that definitions that exist in the
// importer, prior to the import, cannot affect the import results.
bool MergeParseResults(const SourceLocation& source_location,
                       const std::filesystem::path& import_filename,
                       ParseResults&& merge_source,
                       ParseContext* parse_context) {
  return MergeParseResultList(source_location, import_filename,
                              std::move(merge_source.primitives),
                              &parse_context->results.primitives,
                              parse_context) &&
         MergeParseResultList(source_location, import_filename,
                              std::move(merge_source.structs),
                              &parse_context->results.structs, parse_context);
}

}  // namespace

bool ProcessImportStatement(ParseContext* parse_context,
                            const std::filesystem::path& filename,
                            const SourceLocation& source_location) {
  std::filesystem::path absolute_import_path;
  if (filename.is_relative()) {
    if (!parse_context->filename) {
      parse_context->error = ParseErrorWithLocation{
          GenericError{
              "Import file path cannot be relative if the importer is not a "
              "file."},
          source_location};
      return false;
    }

    absolute_import_path = parse_context->filename->parent_path() / filename;
  } else {
    absolute_import_path = filename;
  }
  {
    // Make sure that the file exists and we can open it.
    std::ifstream input_file_handle(absolute_import_path);
    if (!input_file_handle) {
      parse_context->error = ParseErrorWithLocation{
          GenericError{"Import file path '" + absolute_import_path.string() +
                       "' cannot opened."},
          source_location};
      return false;
    }
  }
  auto canonical_import_path = std::filesystem::canonical(absolute_import_path);

  auto result_or_error = ParseFromFile(canonical_import_path);

  if (auto error = std::get_if<ParseErrorWithLocation>(&result_or_error)) {
    // Propagate the error to the parse_context and return false to notify
    // the parser grammar logic that an error has occured.
    parse_context->error = ParseErrorWithLocation{
        ImportError{
            absolute_import_path,
            std::make_unique<ParseErrorWithLocation>(std::move(*error))},
        source_location};

    return false;
  }

  // Merge the importee parse results into the main parse results.
  return MergeParseResults(source_location, filename,
                           std::move(std::get<ParseResults>(result_or_error)),
                           parse_context);
}