#include "process_import_statement.h"

#include <fstream>
#include <string>

#include "ast_functions.h"
#include "parser_wrapper.h"

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

  auto result_or_error =
      ParseFromFile(canonical_import_path, parse_context->initial_context);

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
  // source_location, filename, parse_context
  auto error = parse_context->results.Merge(
      std::move(std::get<ParseResults>(result_or_error)));
  if (error) {
    parse_context->error = ParseErrorWithLocation{
        ImportError{
            canonical_import_path,
            std::make_unique<ParseErrorWithLocation>(std::move(*error))},
        source_location};
    return false;
  }

  return true;
}
