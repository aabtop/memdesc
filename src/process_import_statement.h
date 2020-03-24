#ifndef MEMDESC_PARSE_IMPORT_STATEMENT_H_
#define MEMDESC_PARSE_IMPORT_STATEMENT_H_

#include <filesystem>

#include "ast.h"
#include "parser_helper_functions.h"

bool ProcessImportStatement(ParseContext* parse_context,
                            const std::filesystem::path& filename,
                            const SourceLocation& source_location);

#endif  // MEMDESC_PARSE_IMPORT_STATEMENT_H_
