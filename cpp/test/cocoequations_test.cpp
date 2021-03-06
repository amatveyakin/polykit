// Tests for iterated comultiplication (with 3 or more components).

#include "gtest/gtest.h"

#include "lib/coalgebra.h"
#include "lib/iterated_integral.h"
#include "lib/polylog_qli.h"
#include "lib/polylog_liquad.h"
#include "lib/polylog_lira.h"
#include "lib/summation.h"
#include "test_util/matchers.h"


TEST(CocoequationsTest, LARGE_QLi6_Arg6_Comult_2_2_2) {
  const auto expr = QLi6(1,2,3,4,5,6);
  EXPECT_EXPR_ZERO(comultiply(expr, {2,2,2}));
}

TEST(CocoequationsTest, LARGE_QLi6_Arg8_Comult_2_2_2) {
  const auto expr = QLi6(1,2,3,4,5,6,7,8);
  EXPECT_FALSE(comultiply(expr, {2,2,2}).is_zero());
}

TEST(CocoequationsTest, HUGE_QLi6_Arg8_Sum_Comult_2_2_2) {
  const int num_points = 9;
  auto qli_expr = sum_looped_vec(
    [&](const XArgs& args) {
      return QLiVec(6, args);
    },
    num_points,
    seq_incl(1, num_points - 1)
  );
  auto qli_comult = comultiply(qli_expr, {2,2,2});
  EXPECT_EXPR_ZERO(qli_comult);
}

TEST(CocoequationsTest, LARGE_Lira4_Comult_2_2_2) {
  const auto expr = theta_expr_to_delta_expr(Lira3(2,1)(CR(1,2,3,4), CR(1,4,5,2)));
  EXPECT_EXPR_ZERO(comultiply(expr, {2,2,2}));
}

TEST(CocoequationsTest, LARGE_Corr_Comult_2_2_2) {
  const auto expr = Corr(1,2,3,4,5,6,7);
  EXPECT_FALSE(comultiply(expr, {2,2,2}).is_zero());
}
