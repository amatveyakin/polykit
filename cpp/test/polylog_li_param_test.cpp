#include "lib/polylog_li_param.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(LiParamTest, Serialization) {
  const LiParam orig{2, {1,3,5}, {{1},{3,4,5},{2}}};
  auto encoded = li_param_to_key(orig);
  const LiParam decoded = key_to_li_param(encoded);
  EXPECT_TRUE(orig == decoded);
  EXPECT_EQ(to_string(orig), "2_Li_1_3_5(x1,x3x4x5,x2)");
}
