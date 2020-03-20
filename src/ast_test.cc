#include <gtest/gtest.h>

#include <memory>

#include "ast.h"
#include "ast_test_helpers.h"

TEST(AstTest, SingleStructNoFields) {
  auto result = ParseTestInput("struct Foo {};");

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

  ASSERT_EQ(1, result.structs.size());
  ASSERT_TRUE(result.structs.find("Foo") != result.structs.end());
  EXPECT_TRUE(result.structs["Foo"]->fields.empty());
}

TEST(AstTest, SinglePrimitiveDefTest) {
  auto result = ParseTestInput("primitive Foo(4, 8);");

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

  ASSERT_EQ(1, result.primitives.size());
  ASSERT_TRUE(result.primitives.find("Foo") != result.primitives.end());
  EXPECT_EQ("Foo", result.primitives["Foo"]->name);
  EXPECT_EQ(4, result.primitives["Foo"]->size);
  EXPECT_EQ(8, result.primitives["Foo"]->alignment);
}

TEST(AstTest, SingleStructSingleField) {
  auto result = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "};");

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

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
  auto result = ParseTestInputWithDefaultPrimitives(
      "// Comment in front!\n"
      "\n"
      "struct Foo {  // Starting a new struct.\n"
      "  // Just a comment on a line by itself.\n"
      "  float bar;  // This is a member.\n"
      "}; //And we're done.\n");

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

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
  auto result = ParseTestInputWithDefaultPrimitives(
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

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

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
  auto result = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "  int32_t bar2;"
      "  double bar3;"
      "};");

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

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
  auto result = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "  int32_t bar2[4];"
      "  double bar3;"
      "};");

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

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
  auto result = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "};"
      "struct Bar {"
      "  Foo foo;"
      "};");

  ASSERT_FALSE(result.error);
  ASSERT_TRUE(result.complete);

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

TEST(AstTest, ErrorLocationTest) {
  auto result = ParseTestInput(
      "struct Foo {\n"
      "  float;\n"
      "};\n");

  ASSERT_TRUE(result.error);

  EXPECT_EQ(2, result.error->location.line_number);
  EXPECT_EQ(8, result.error->location.column_number);
}

TEST(AstTest, ErrorLocationWithMultilineCommentsTest) {
  auto result = ParseTestInput(
      "struct Foo {\n"
      "/* just a multiline\n"
      "   comment\n"
      "   okay done. */\n"
      "  float;\n"
      "};\n");

  ASSERT_TRUE(result.error);

  EXPECT_EQ(5, result.error->location.line_number);
  EXPECT_EQ(8, result.error->location.column_number);
}
