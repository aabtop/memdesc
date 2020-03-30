#include <gtest/gtest.h>

#include <filesystem>

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

  EXPECT_TRUE(result.dependencies().empty());

  ASSERT_EQ(4, result.primitives().size());

  ASSERT_TRUE(result.primitives().find("float") != result.primitives().end());
  EXPECT_EQ(4, result.primitives().at("float")->size);
  EXPECT_EQ(4, result.primitives().at("float")->alignment);
  EXPECT_EQ(filename, result.primitives().at("float")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("int") != result.primitives().end());
  EXPECT_EQ(4, result.primitives().at("int")->size);
  EXPECT_EQ(4, result.primitives().at("int")->alignment);
  EXPECT_EQ(filename, result.primitives().at("int")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("double") != result.primitives().end());
  EXPECT_EQ(8, result.primitives().at("double")->size);
  EXPECT_EQ(8, result.primitives().at("double")->alignment);
  EXPECT_EQ(filename, result.primitives().at("double")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("char") != result.primitives().end());
  EXPECT_EQ(1, result.primitives().at("char")->size);
  EXPECT_EQ(1, result.primitives().at("char")->alignment);
  EXPECT_EQ(filename, result.primitives().at("char")->defined_at.filename);

  ASSERT_EQ(2, result.structs().size());

  ASSERT_TRUE(result.structs().find("Foo") != result.structs().end());
  ASSERT_EQ(3, result.structs().at("Foo")->fields.size());
  EXPECT_EQ(filename, result.structs().at("Foo")->defined_at.filename);
  EXPECT_EQ(6, result.structs().at("Foo")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("Foo")->defined_at.column_number);

  ASSERT_TRUE(result.structs().find("Bar") != result.structs().end());
  ASSERT_EQ(3, result.structs().at("Bar")->fields.size());
  EXPECT_EQ(filename, result.structs().at("Bar")->defined_at.filename);
  EXPECT_EQ(12, result.structs().at("Bar")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("Bar")->defined_at.column_number);
}

TEST(ParseFileTest, ParseFileWithImport) {
  const fs::path filename(AbsoluteCanonicalPath("examples/importer.mdesc"));
  const fs::path importee_filename(
      AbsoluteCanonicalPath("examples/importee.mdesc"));
  auto result_or_error = ParseFromFile(filename);

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  EXPECT_EQ(1, result.dependencies().size());
  EXPECT_EQ(1, result.dependencies().count(importee_filename));

  ASSERT_EQ(4, result.primitives().size());

  ASSERT_TRUE(result.primitives().find("float") != result.primitives().end());
  EXPECT_EQ(4, result.primitives().at("float")->size);
  EXPECT_EQ(4, result.primitives().at("float")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("float")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("int") != result.primitives().end());
  EXPECT_EQ(4, result.primitives().at("int")->size);
  EXPECT_EQ(4, result.primitives().at("int")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("int")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("double") != result.primitives().end());
  EXPECT_EQ(8, result.primitives().at("double")->size);
  EXPECT_EQ(8, result.primitives().at("double")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("double")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("char") != result.primitives().end());
  EXPECT_EQ(1, result.primitives().at("char")->size);
  EXPECT_EQ(1, result.primitives().at("char")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("char")->defined_at.filename);

  ASSERT_EQ(2, result.structs().size());

  ASSERT_TRUE(result.structs().find("Foo") != result.structs().end());
  ASSERT_EQ(3, result.structs().at("Foo")->fields.size());
  EXPECT_EQ(filename, result.structs().at("Foo")->defined_at.filename);
  EXPECT_EQ(3, result.structs().at("Foo")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("Foo")->defined_at.column_number);

  ASSERT_TRUE(result.structs().find("Bar") != result.structs().end());
  ASSERT_EQ(3, result.structs().at("Bar")->fields.size());
  EXPECT_EQ(filename, result.structs().at("Bar")->defined_at.filename);
  EXPECT_EQ(9, result.structs().at("Bar")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("Bar")->defined_at.column_number);
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

  EXPECT_EQ(3, result.dependencies().size());
  EXPECT_EQ(1, result.dependencies().count(importee_filename));
  EXPECT_EQ(1, result.dependencies().count(importer_filename));
  EXPECT_EQ(1, result.dependencies().count(other_importer_filename));

  ASSERT_EQ(4, result.primitives().size());

  ASSERT_TRUE(result.primitives().find("float") != result.primitives().end());
  EXPECT_EQ(4, result.primitives().at("float")->size);
  EXPECT_EQ(4, result.primitives().at("float")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("float")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("int") != result.primitives().end());
  EXPECT_EQ(4, result.primitives().at("int")->size);
  EXPECT_EQ(4, result.primitives().at("int")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("int")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("double") != result.primitives().end());
  EXPECT_EQ(8, result.primitives().at("double")->size);
  EXPECT_EQ(8, result.primitives().at("double")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("double")->defined_at.filename);

  ASSERT_TRUE(result.primitives().find("char") != result.primitives().end());
  EXPECT_EQ(1, result.primitives().at("char")->size);
  EXPECT_EQ(1, result.primitives().at("char")->alignment);
  EXPECT_EQ(importee_filename,
            result.primitives().at("char")->defined_at.filename);

  ASSERT_EQ(3, result.structs().size());

  ASSERT_TRUE(result.structs().find("Foo") != result.structs().end());
  ASSERT_EQ(3, result.structs().at("Foo")->fields.size());
  EXPECT_EQ(importer_filename, result.structs().at("Foo")->defined_at.filename);
  EXPECT_EQ(3, result.structs().at("Foo")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("Foo")->defined_at.column_number);

  ASSERT_TRUE(result.structs().find("Bar") != result.structs().end());
  ASSERT_EQ(3, result.structs().at("Bar")->fields.size());
  EXPECT_EQ(importer_filename, result.structs().at("Bar")->defined_at.filename);
  EXPECT_EQ(9, result.structs().at("Bar")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("Bar")->defined_at.column_number);

  ASSERT_TRUE(result.structs().find("Rey") != result.structs().end());
  ASSERT_EQ(4, result.structs().at("Rey")->fields.size());
  EXPECT_EQ(other_importer_filename,
            result.structs().at("Rey")->defined_at.filename);
  EXPECT_EQ(3, result.structs().at("Rey")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("Rey")->defined_at.column_number);
}

TEST(ParseFileTest, ImporteeHasAccessToPreambleTest) {
  // Make sure that an importee also has access to the preamble.
  const fs::path preamble_filename(
      AbsoluteCanonicalPath("examples/importee.mdesc"));
  const fs::path importer_filename(
      AbsoluteCanonicalPath("examples/importer_with_preamble.mdesc"));
  const fs::path importee_filename(
      AbsoluteCanonicalPath("examples/importee_with_preamble.mdesc"));
  auto preamble_result_or_error = ParseFromFile(preamble_filename);
  ASSERT_FALSE(
      std::holds_alternative<ParseErrorWithLocation>(preamble_result_or_error));
  auto& preamble_result = std::get<ParseResults>(preamble_result_or_error);

  auto result_or_error = ParseFromFile(importer_filename, &preamble_result);

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  EXPECT_EQ(2, result.dependencies().size());
  EXPECT_EQ(1, result.dependencies().count(importee_filename));
  EXPECT_EQ(1, result.dependencies().count(preamble_filename));

  // The preamble primitives().
  ASSERT_EQ(4, result.primitives().size());

  ASSERT_EQ(2, result.structs().size());

  ASSERT_TRUE(result.structs().find("ImporteeStruct") !=
              result.structs().end());
  ASSERT_EQ(2, result.structs().at("ImporteeStruct")->fields.size());
  EXPECT_EQ(importee_filename,
            result.structs().at("ImporteeStruct")->defined_at.filename);
  EXPECT_EQ(1, result.structs().at("ImporteeStruct")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("ImporteeStruct")->defined_at.column_number);

  const Primitive* importee_field_primitive_type = AsPrimitive(
      result.structs().at("ImporteeStruct")->fields[0].type.base_type);
  ASSERT_TRUE(importee_field_primitive_type);
  EXPECT_EQ("float", importee_field_primitive_type->name);

  ASSERT_TRUE(result.structs().find("ImporterStruct") !=
              result.structs().end());
  ASSERT_EQ(2, result.structs().at("ImporterStruct")->fields.size());
  EXPECT_EQ(importer_filename,
            result.structs().at("ImporterStruct")->defined_at.filename);
  EXPECT_EQ(8, result.structs().at("ImporterStruct")->defined_at.line_number);
  EXPECT_EQ(8, result.structs().at("ImporterStruct")->defined_at.column_number);

  const Primitive* importer_field_primitive_type = AsPrimitive(
      result.structs().at("ImporterStruct")->fields[0].type.base_type);
  ASSERT_TRUE(importer_field_primitive_type);
  EXPECT_EQ("float", importer_field_primitive_type->name);

  // The type pointers should all point to the same thing.
  EXPECT_EQ(importee_field_primitive_type, importer_field_primitive_type);
}

TEST(ParseFileTest, ImportErrorsRegisteredTest) {
  const fs::path filename(
      AbsoluteCanonicalPath("examples/importer_with_error.mdesc"));
  const fs::path importee_filename(
      AbsoluteCanonicalPath("examples/importee.mdesc"));
  auto result_or_error = ParseFromFile(filename);

  ASSERT_TRUE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& error = std::get<ParseErrorWithLocation>(result_or_error);

  EXPECT_EQ(filename, error.location.filename);
  EXPECT_EQ(4, error.location.line_number);
  EXPECT_EQ(1, error.location.column_number);

  ASSERT_TRUE(std::holds_alternative<ImportError>(error.error));
  auto& import_error = std::get<ImportError>(error.error);

  EXPECT_EQ(importee_filename, import_error.import_filename);

  EXPECT_EQ(importee_filename, import_error.sub_error->location.filename);
  EXPECT_EQ(2, import_error.sub_error->location.line_number);
  EXPECT_EQ(11, import_error.sub_error->location.column_number);

  ASSERT_TRUE(
      std::holds_alternative<RedefinitionError>(import_error.sub_error->error));
  const auto& redefinition_error =
      std::get<RedefinitionError>(import_error.sub_error->error);

  EXPECT_EQ("int", redefinition_error.name);
  EXPECT_EQ(filename, redefinition_error.original_definition_location.filename);
  EXPECT_EQ(2, redefinition_error.original_definition_location.line_number);
  EXPECT_EQ(11, redefinition_error.original_definition_location.column_number);
}
