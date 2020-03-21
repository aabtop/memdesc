#include <gtest/gtest.h>

#include <memory>

#include "ast.h"
#include "ast_functions.h"
#include "ast_test_helpers.h"
#include "parse_error.h"

TEST(AstTest, SingleStructNoFields) {
  auto result_or_error = ParseTestInput("struct Foo {};");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(1, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  EXPECT_TRUE(result.structs["Foo"]->fields.empty());
}

TEST(AstTest, SinglePrimitiveDefTest) {
  auto result_or_error = ParseTestInput("primitive Foo(4, 8);");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(1, result.primitives.size());
  ASSERT_TRUE(result.primitives.find("Foo") != result.primitives.end());
  EXPECT_EQ("Foo", result.primitives["Foo"]->name);
  EXPECT_EQ(4, result.primitives["Foo"]->size);
  EXPECT_EQ(8, result.primitives["Foo"]->alignment);
}

TEST(AstTest, MultiplePrimitiveDefTest) {
  auto result_or_error = ParseTestInput(
      "primitive Foo(4, 8);\n"
      "primitive Bar(1, 1);\n");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(2, result.primitives.size());

  ASSERT_TRUE(result.primitives.find("Foo") != result.primitives.end());
  EXPECT_EQ("Foo", result.primitives["Foo"]->name);
  EXPECT_EQ(4, result.primitives["Foo"]->size);
  EXPECT_EQ(8, result.primitives["Foo"]->alignment);
  EXPECT_EQ(1, result.primitives["Foo"]->defined_at.line_number);
  EXPECT_EQ(11, result.primitives["Foo"]->defined_at.column_number);

  ASSERT_TRUE(result.primitives.find("Bar") != result.primitives.end());
  EXPECT_EQ("Bar", result.primitives["Bar"]->name);
  EXPECT_EQ(1, result.primitives["Bar"]->size);
  EXPECT_EQ(1, result.primitives["Bar"]->alignment);
  EXPECT_EQ(2, result.primitives["Bar"]->defined_at.line_number);
  EXPECT_EQ(11, result.primitives["Bar"]->defined_at.column_number);
}

TEST(AstTest, SingleStructSingleField) {
  auto result_or_error = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "};");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(1, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(1, result.structs["Foo"]->fields.size());
  Primitive* bar_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[0].type.base_type);
  EXPECT_EQ("float", bar_type->name);
  EXPECT_EQ("bar", result.structs["Foo"]->fields[0].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[0].type.array_count);
}

TEST(AstTest, SingleLineComments) {
  auto result_or_error = ParseTestInputWithDefaultPrimitives(
      "// Comment in front!\n"
      "\n"
      "struct Foo {  // Starting a new struct.\n"
      "  // Just a comment on a line by itself.\n"
      "  float bar;  // This is a member.\n"
      "}; //And we're done.\n");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(1, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(1, result.structs["Foo"]->fields.size());
  Primitive* bar_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[0].type.base_type);
  EXPECT_EQ("float", bar_type->name);
  EXPECT_EQ("bar", result.structs["Foo"]->fields[0].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[0].type.array_count);
}

TEST(AstTest, MultiLineComments) {
  auto result_or_error = ParseTestInputWithDefaultPrimitives(
      "/* Comment in front! */\n"
      "\n"
      "/* actual \n"
      "   multiline \n"
      "   comment.*/\n"
      "struct /* sneaky middle comment */ Foo {\n"
      "  /* Multiline comment in the struct.\n"
      "     Done now.\n"
      "  */\n"
      "  float bar;\n"
      "};\n");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(1, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(1, result.structs["Foo"]->fields.size());
  Primitive* bar_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[0].type.base_type);
  EXPECT_EQ("float", bar_type->name);
  EXPECT_EQ("bar", result.structs["Foo"]->fields[0].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[0].type.array_count);
}

TEST(AstTest, SingleStructMultiField) {
  auto result_or_error = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "  int32_t bar2;"
      "  double bar3;"
      "};");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(1, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(3, result.structs["Foo"]->fields.size());
  Primitive* bar_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[0].type.base_type);
  EXPECT_EQ("float", bar_type->name);
  EXPECT_EQ("bar", result.structs["Foo"]->fields[0].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[0].type.array_count);

  Primitive* bar2_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[1].type.base_type);
  EXPECT_EQ("int32_t", bar2_type->name);
  EXPECT_EQ("bar2", result.structs["Foo"]->fields[1].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[1].type.array_count);

  Primitive* bar3_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[2].type.base_type);
  EXPECT_EQ("double", bar3_type->name);
  EXPECT_EQ("bar3", result.structs["Foo"]->fields[2].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[2].type.array_count);
}

TEST(AstTest, SingleStructMultiFieldWithCount) {
  auto result_or_error = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "  int32_t bar2[4];"
      "  double bar3;"
      "};");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(1, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(3, result.structs["Foo"]->fields.size());
  Primitive* bar_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[0].type.base_type);
  EXPECT_EQ("float", bar_type->name);
  EXPECT_EQ("bar", result.structs["Foo"]->fields[0].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[0].type.array_count);

  Primitive* bar2_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[1].type.base_type);
  EXPECT_EQ("int32_t", bar2_type->name);
  EXPECT_EQ("bar2", result.structs["Foo"]->fields[1].name);
  ASSERT_TRUE(result.structs["Foo"]->fields[1].type.array_count);
  EXPECT_EQ(4, *(result.structs["Foo"]->fields[1].type.array_count));

  Primitive* bar3_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[2].type.base_type);
  EXPECT_EQ("double", bar3_type->name);
  EXPECT_EQ("bar3", result.structs["Foo"]->fields[2].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[2].type.array_count);
}

TEST(AstTest, MultiStructs) {
  auto result_or_error = ParseTestInputWithDefaultPrimitives(
      "struct Foo {\n"
      "  float bar;\n"
      "};\n"
      "struct Bar {\n"
      "  Foo foo;\n"
      "};\n");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(2, result.structs.size());

  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_EQ(1, result.structs["Foo"]->fields.size());
  Primitive* bar_type =
      std::get<Primitive*>(result.structs["Foo"]->fields[0].type.base_type);
  EXPECT_EQ("float", bar_type->name);
  EXPECT_EQ("bar", result.structs["Foo"]->fields[0].name);
  EXPECT_FALSE(result.structs["Foo"]->fields[0].type.array_count);

  ASSERT_TRUE(result.structs.find("Bar") != result.structs.end());
  ASSERT_EQ(1, result.structs["Bar"]->fields.size());
  Struct* foo_type =
      std::get<Struct*>(result.structs["Bar"]->fields[0].type.base_type);
  EXPECT_EQ("Foo", foo_type->name);
  EXPECT_EQ("foo", result.structs["Bar"]->fields[0].name);
  EXPECT_FALSE(result.structs["Bar"]->fields[0].type.array_count);
}

TEST(AstTest, StructDefinitionLocations) {
  auto result_or_error = ParseTestInput(
      "struct Foo {};\n"
      "struct Bar {\n"
      "  Foo foo;\n"
      "};\n");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(2, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  ASSERT_TRUE(result.structs.find("Bar") != result.structs.end());
  EXPECT_EQ(1, result.structs["Foo"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Foo"]->defined_at.column_number);
  EXPECT_EQ(2, result.structs["Bar"]->defined_at.line_number);
  EXPECT_EQ(8, result.structs["Bar"]->defined_at.column_number);
}

TEST(AstTest, ErrorLocationTest) {
  auto result_or_error = ParseTestInput(
      "struct Foo {\n"
      "  float;\n"
      "};\n");

  ASSERT_TRUE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& error = std::get<ParseErrorWithLocation>(result_or_error);

  EXPECT_EQ(2, error.location.line_number);
  EXPECT_EQ(8, error.location.column_number);
  EXPECT_TRUE(std::holds_alternative<SyntaxError>(error.error));
}

TEST(AstTest, ErrorLocationWithMultilineCommentsTest) {
  auto result_or_error = ParseTestInput(
      "struct Foo {\n"
      "/* just a multiline\n"
      "   comment\n"
      "   okay done. */\n"
      "  float;\n"
      "};\n");

  ASSERT_TRUE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& error = std::get<ParseErrorWithLocation>(result_or_error);

  EXPECT_EQ(5, error.location.line_number);
  EXPECT_EQ(8, error.location.column_number);
  EXPECT_TRUE(std::holds_alternative<SyntaxError>(error.error));
}

TEST(AstTest, UndeclaredTypeTest) {
  auto result_or_error = ParseTestInput(
      "struct Foo {\n"
      "  Bar bar;\n"
      "};\n");

  ASSERT_TRUE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& error = std::get<ParseErrorWithLocation>(result_or_error);

  EXPECT_EQ(2, error.location.line_number);
  EXPECT_EQ(3, error.location.column_number);
  UndeclaredTypeReference* undeclared_type_reference =
      std::get_if<UndeclaredTypeReference>(&(error.error));
  ASSERT_TRUE(undeclared_type_reference);
  EXPECT_EQ("Bar", undeclared_type_reference->type_name);
}

TEST(AstTest, StructTypeRedefinitionTest) {
  auto result_or_error = ParseTestInput(
      "\n"
      "struct Foo {};\n"
      "struct Bar {};\n"
      "struct Foo {};\n");

  ASSERT_TRUE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& error = std::get<ParseErrorWithLocation>(result_or_error);

  EXPECT_EQ(4, error.location.line_number);
  EXPECT_EQ(8, error.location.column_number);
  TypeRedefinition* type_redefinition =
      std::get_if<TypeRedefinition>(&(error.error));
  ASSERT_TRUE(type_redefinition);
  EXPECT_EQ(2, type_redefinition->original_definition_location.line_number);
  EXPECT_EQ(8, type_redefinition->original_definition_location.column_number);
  EXPECT_EQ("Foo", type_redefinition->type_name);
}

TEST(AstTest, PrimitiveTypeRedefinitionTest) {
  auto result_or_error = ParseTestInput(
      "\n"
      "primitive Foo(1, 1);\n"
      "primitive Bar(1, 1);\n"
      "primitive Foo(4, 4);\n");

  ASSERT_TRUE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& error = std::get<ParseErrorWithLocation>(result_or_error);

  EXPECT_EQ(4, error.location.line_number);
  EXPECT_EQ(11, error.location.column_number);
  TypeRedefinition* type_redefinition =
      std::get_if<TypeRedefinition>(&(error.error));
  ASSERT_TRUE(type_redefinition);
  EXPECT_EQ(2, type_redefinition->original_definition_location.line_number);
  EXPECT_EQ(11, type_redefinition->original_definition_location.column_number);
  EXPECT_EQ("Foo", type_redefinition->type_name);
}
