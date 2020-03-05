#include <gtest/gtest.h>

#include "ast.h"
#include "parser_wrapper.h"

namespace {
auto ParseTestInput(const char* str) {
  std::string test_string(str);
  test_string.push_back('\0');
  test_string.push_back('\0');

  return ParseFromBuffer(test_string.data(), test_string.length());
}
}  // namespace

TEST(AstTest, SingleStructNoFields) {
  auto result = ParseTestInput(
      "struct Foo {};");

  ASSERT_TRUE(result);

  ASSERT_EQ(1, result->size());
  EXPECT_EQ("Foo", (*result)[0]->name);
  EXPECT_TRUE((*result)[0]->fields.empty());
}

TEST(AstTest, SingleStructSingleField) {
  auto result = ParseTestInput(
      "struct Foo {"
      "  float bar;"
      "};");

  ASSERT_TRUE(result);

  ASSERT_EQ(1, result->size());
  EXPECT_EQ("Foo", (*result)[0]->name);
  ASSERT_EQ(1, (*result)[0]->fields.size());
  EXPECT_EQ("float", (*result)[0]->fields[0].type);
  EXPECT_EQ("bar", (*result)[0]->fields[0].name);
  EXPECT_FALSE((*result)[0]->fields[0].count);
}

TEST(AstTest, SingleLineComments) {
  auto result = ParseTestInput(
      "// Comment in front!\n"
      "\n"
      "struct Foo {  // Starting a new struct.\n"
      "  // Just a comment on a line by itself.\n"
      "  float bar;  // This is a member.\n"
      "}; //And we're done.\n");

  ASSERT_TRUE(result);

  ASSERT_EQ(1, result->size());
  EXPECT_EQ("Foo", (*result)[0]->name);
  ASSERT_EQ(1, (*result)[0]->fields.size());
  EXPECT_EQ("float", (*result)[0]->fields[0].type);
  EXPECT_EQ("bar", (*result)[0]->fields[0].name);
  EXPECT_FALSE((*result)[0]->fields[0].count);
}

TEST(AstTest, MultiLineComments) {
  auto result = ParseTestInput(
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

  ASSERT_TRUE(result);

  ASSERT_EQ(1, result->size());
  EXPECT_EQ("Foo", (*result)[0]->name);
  ASSERT_EQ(1, (*result)[0]->fields.size());
  EXPECT_EQ("float", (*result)[0]->fields[0].type);
  EXPECT_EQ("bar", (*result)[0]->fields[0].name);
  EXPECT_FALSE((*result)[0]->fields[0].count);
}

TEST(AstTest, SingleStructMultiField) {
  auto result = ParseTestInput(
      "struct Foo {"
      "  float bar;"
      "  int32_t bar2;"
      "  double bar3;"
      "};");

  ASSERT_TRUE(result);

  ASSERT_EQ(1, result->size());
  EXPECT_EQ("Foo", (*result)[0]->name);
  ASSERT_EQ(3, (*result)[0]->fields.size());
  EXPECT_EQ("float", (*result)[0]->fields[0].type);
  EXPECT_EQ("bar", (*result)[0]->fields[0].name);
  EXPECT_FALSE((*result)[0]->fields[0].count);
  EXPECT_EQ("int32_t", (*result)[0]->fields[1].type);
  EXPECT_EQ("bar2", (*result)[0]->fields[1].name);
  EXPECT_FALSE((*result)[0]->fields[1].count);
  EXPECT_EQ("double", (*result)[0]->fields[2].type);
  EXPECT_EQ("bar3", (*result)[0]->fields[2].name);
  EXPECT_FALSE((*result)[0]->fields[2].count);
}

TEST(AstTest, SingleStructMultiFieldWithCount) {
  auto result = ParseTestInput(
      "struct Foo {"
      "  float bar;"
      "  int32_t bar2[4];"
      "  double bar3;"
      "};");

  ASSERT_TRUE(result);

  ASSERT_EQ(1, result->size());
  EXPECT_EQ("Foo", (*result)[0]->name);
  ASSERT_EQ(3, (*result)[0]->fields.size());
  EXPECT_EQ("float", (*result)[0]->fields[0].type);
  EXPECT_EQ("bar", (*result)[0]->fields[0].name);
  EXPECT_FALSE((*result)[0]->fields[0].count);
  EXPECT_EQ("int32_t", (*result)[0]->fields[1].type);
  EXPECT_EQ("bar2", (*result)[0]->fields[1].name);
  ASSERT_TRUE((*result)[0]->fields[1].count);
  EXPECT_EQ(4, *((*result)[0]->fields[1].count));
  EXPECT_EQ("double", (*result)[0]->fields[2].type);
  EXPECT_EQ("bar3", (*result)[0]->fields[2].name);
  EXPECT_FALSE((*result)[0]->fields[2].count);
}

TEST(AstTest, MultiStructs) {
  auto result = ParseTestInput(
      "struct Foo {"
      "  float bar;"
      "};"
      "struct Bar {"
      "  Foo bar;"
      "};");

  ASSERT_TRUE(result);

  ASSERT_EQ(2, result->size());
  EXPECT_EQ("Foo", (*result)[0]->name);
  ASSERT_EQ(1, (*result)[0]->fields.size());
  EXPECT_EQ("float", (*result)[0]->fields[0].type);
  EXPECT_EQ("bar", (*result)[0]->fields[0].name);
  EXPECT_FALSE((*result)[0]->fields[0].count);
  EXPECT_EQ("Bar", (*result)[1]->name);
  ASSERT_EQ(1, (*result)[1]->fields.size());
  EXPECT_EQ("Foo", (*result)[1]->fields[0].type);
  EXPECT_EQ("bar", (*result)[1]->fields[0].name);
  EXPECT_FALSE((*result)[1]->fields[0].count);
}
