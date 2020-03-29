#include "ast_functions.h"

#include <gtest/gtest.h>

#include "ast.h"
#include "ast_test_helpers.h"

TEST(AstFunctionsTest, TypeName) {
  auto result_or_error = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "};"
      "struct Bar {"
      "  Foo foo;"
      "};");

  ASSERT_FALSE(std::holds_alternative<ParseErrorWithLocation>(result_or_error));
  auto& result = std::get<ParseResults>(result_or_error);

  ASSERT_EQ(2, result.structs().size());

  ASSERT_TRUE(result.structs().find("Foo") != result.structs().end());
  ASSERT_EQ(1, result.structs().at("Foo")->fields.size());
  EXPECT_EQ("float", TypeName(result.structs().at("Foo")->fields[0].type));

  ASSERT_TRUE(result.structs().find("Bar") != result.structs().end());
  ASSERT_EQ(1, result.structs().at("Bar")->fields.size());
  EXPECT_EQ("Foo", TypeName(result.structs().at("Bar")->fields[0].type));
}
