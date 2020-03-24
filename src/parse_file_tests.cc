#include <filesystem>

#include <gtest/gtest.h>

#include "ast.h"
#include "ast_functions.h"
#include "parser_wrapper.h"

namespace fs = std::filesystem;

namespace {

fs::path AbsoluteCanonicalPath(const fs::path& path) {
  return fs::canonical(fs::current_path() / path);
}

}  // namespace

// These tests are essentially end-to-end tests, where we ensure that the flow
// of parsing from a file will work correctly.
TEST(ParseFileTest, ParseSimpleFile) {
  const fs::path filename(AbsoluteCanonicalPath("examples/simple.mdesc"));
  auto result_or_error = ParseFromFile(filename);

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(4, result.primitives.size());

  ASSERT_TRUE(result.primitives.find("float") != result.primitives.end());
  EXPECT_EQ(4, result.primitives["float"]->size);
  EXPECT_EQ(4, result.primitives["float"]->alignment);
  EXPECT_EQ(filename, result.primitives["float"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("int") != result.primitives.end());
  EXPECT_EQ(4, result.primitives["int"]->size);
  EXPECT_EQ(4, result.primitives["int"]->alignment);
  EXPECT_EQ(filename, result.primitives["int"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("double") != result.primitives.end());
  EXPECT_EQ(8, result.primitives["double"]->size);
  EXPECT_EQ(8, result.primitives["double"]->alignment);
  EXPECT_EQ(filename, result.primitives["double"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("char") != result.primitives.end());
  EXPECT_EQ(1, result.primitives["char"]->size);
  EXPECT_EQ(1, result.primitives["char"]->alignment);
  EXPECT_EQ(filename, result.primitives["char"]->defined_at.filename);

  ASSERT_EQ(2, result.structs.size());

  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(3, result.structs["Foo"]->fields.size());
  EXPECT_EQ(filename, result.structs["Foo"]->defined_at.filename);
  EXPECT_EQ(6, result.structs["Foo"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Foo"]->defined_at.column_number);

  ASSERT_TRUE(result.structs.find("Bar") != result.structs.end());
  ASSERT_EQ(3, result.structs["Bar"]->fields.size());
  EXPECT_EQ(filename, result.structs["Bar"]->defined_at.filename);
  EXPECT_EQ(12, result.structs["Bar"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Bar"]->defined_at.column_number);
}

TEST(ParseFileTest, ParseFileWithImport) {
  const fs::path filename(AbsoluteCanonicalPath("examples/importer.mdesc"));
  const fs::path importee_filename(
      AbsoluteCanonicalPath("examples/importee.mdesc"));
  auto result_or_error = ParseFromFile(filename);

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(4, result.primitives.size());

  ASSERT_TRUE(result.primitives.find("float") != result.primitives.end());
  EXPECT_EQ(4, result.primitives["float"]->size);
  EXPECT_EQ(4, result.primitives["float"]->alignment);
  EXPECT_EQ(importee_filename, result.primitives["float"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("int") != result.primitives.end());
  EXPECT_EQ(4, result.primitives["int"]->size);
  EXPECT_EQ(4, result.primitives["int"]->alignment);
  EXPECT_EQ(importee_filename, result.primitives["int"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("double") != result.primitives.end());
  EXPECT_EQ(8, result.primitives["double"]->size);
  EXPECT_EQ(8, result.primitives["double"]->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives["double"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("char") != result.primitives.end());
  EXPECT_EQ(1, result.primitives["char"]->size);
  EXPECT_EQ(1, result.primitives["char"]->alignment);
  EXPECT_EQ(importee_filename, result.primitives["char"]->defined_at.filename);

  ASSERT_EQ(2, result.structs.size());

  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(3, result.structs["Foo"]->fields.size());
  EXPECT_EQ(filename, result.structs["Foo"]->defined_at.filename);
  EXPECT_EQ(3, result.structs["Foo"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Foo"]->defined_at.column_number);

  ASSERT_TRUE(result.structs.find("Bar") != result.structs.end());
  ASSERT_EQ(3, result.structs["Bar"]->fields.size());
  EXPECT_EQ(filename, result.structs["Bar"]->defined_at.filename);
  EXPECT_EQ(9, result.structs["Bar"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Bar"]->defined_at.column_number);
}

TEST(ParseFileTest, ParseFileWithDiamondImportStructure) {
  // Parses a file that imports two other files which both import the same
  // fourth file.
  const fs::path filename(AbsoluteCanonicalPath("examples/diamond.mdesc"));
  const fs::path importee_filename(
      AbsoluteCanonicalPath("examples/importee.mdesc"));
  const fs::path importer_filename(
      AbsoluteCanonicalPath("examples/importer.mdesc"));
  const fs::path other_importer_filename(
      AbsoluteCanonicalPath("examples/other_importer.mdesc"));
  auto result_or_error = ParseFromFile(filename);

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(4, result.primitives.size());

  ASSERT_TRUE(result.primitives.find("float") != result.primitives.end());
  EXPECT_EQ(4, result.primitives["float"]->size);
  EXPECT_EQ(4, result.primitives["float"]->alignment);
  EXPECT_EQ(importee_filename, result.primitives["float"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("int") != result.primitives.end());
  EXPECT_EQ(4, result.primitives["int"]->size);
  EXPECT_EQ(4, result.primitives["int"]->alignment);
  EXPECT_EQ(importee_filename, result.primitives["int"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("double") != result.primitives.end());
  EXPECT_EQ(8, result.primitives["double"]->size);
  EXPECT_EQ(8, result.primitives["double"]->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives["double"]->defined_at.filename);

  ASSERT_TRUE(result.primitives.find("char") != result.primitives.end());
  EXPECT_EQ(1, result.primitives["char"]->size);
  EXPECT_EQ(1, result.primitives["char"]->alignment);
  EXPECT_EQ(importee_filename, result.primitives["char"]->defined_at.filename);

  ASSERT_EQ(3, result.structs.size());

  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(3, result.structs["Foo"]->fields.size());
  EXPECT_EQ(importer_filename, result.structs["Foo"]->defined_at.filename);
  EXPECT_EQ(3, result.structs["Foo"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Foo"]->defined_at.column_number);

  ASSERT_TRUE(result.structs.find("Bar") != result.structs.end());
  ASSERT_EQ(3, result.structs["Bar"]->fields.size());
  EXPECT_EQ(importer_filename, result.structs["Bar"]->defined_at.filename);
  EXPECT_EQ(9, result.structs["Bar"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Bar"]->defined_at.column_number);

  ASSERT_TRUE(result.structs.find("Rey") != result.structs.end());
  ASSERT_EQ(4, result.structs["Rey"]->fields.size());
  EXPECT_EQ(other_importer_filename,
            result.structs["Rey"]->defined_at.filename);
  EXPECT_EQ(3, result.structs["Rey"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Rey"]->defined_at.column_number);
}
