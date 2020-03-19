#include <gtest/gtest.h>

#include "ast.h"
#include "ast_functions.h"
#include "ast_test_helpers.h"

TEST(AstFunctionsTest, TypeName) {
  auto result = ParseTestInputWithDefaultPrimitives(
      "struct Foo {"
      "  float bar;"
      "};"
      "struct Bar {"
      "  Foo foo;"
      "};");

  ASSERT_TRUE(result);

  ASSERT_EQ(2, result->structs.size());

  ASSERT_TRUE(result->structs.find("Foo") != result->structs.end());
  ASSERT_EQ(1, result->structs["Foo"]->fields.size());
  EXPECT_EQ("float", TypeName(result->structs["Foo"]->fields[0].type));

  ASSERT_TRUE(result->structs.find("Bar") != result->structs.end());
  ASSERT_EQ(1, result->structs["Bar"]->fields.size());
  EXPECT_EQ("Foo", TypeName(result->structs["Bar"]->fields[0].type));
}
