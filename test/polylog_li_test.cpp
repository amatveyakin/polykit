#include "lib/polylog_li.h"

#include "gtest/gtest.h"

#include "test_util/matchers.h"


TEST(LiTest, LiShuffle_Arg2_Weight2) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + Li(1,1)({1},{2})
    + Li(1,1)({2},{1})
    + Li(2)  ({1,2})
  );
}
TEST(LiTest, LiShuffle_Arg2_Weight3) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + Li(1,2)({1},{2})
    + Li(2,1)({2},{1})
    + Li(3)  ({1,2})
  );
}
TEST(LiTest, LiShuffle_Arg2_Weight4) {
  EXPECT_EXPR_ZERO_AFTER_LYNDON(
    + Li(1,3)({1},{2})
    + Li(3,1)({2},{1})
    + Li(4)  ({1,2})
  );
}

TEST(LiTest, LiShuffle_NoLyndon_Arg3_Weight3) {
  EXPECT_EXPR_EQ(
    + Li(1,1,1)({1},{2},{3})
    + Li(1,1,1)({1},{3},{2})
    + Li(1,1,1)({3},{1},{2})
    + Li(1,2)  ({1},{2,3})
    + Li(2,1)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Li(1,1)({1},{2}),
      Li(1)  ({3})
    )
  );
}

TEST(LiTest, LiShuffle_NoLyndon_Arg3_Weight4) {
  EXPECT_EXPR_EQ(
    + Li(1,1,2)({1},{2},{3})
    + Li(1,2,1)({1},{3},{2})
    + Li(2,1,1)({3},{1},{2})
    + Li(1,3)  ({1},{2,3})
    + Li(3,1)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Li(1,1)({1},{2}),
      Li(2)  ({3})
    )
  );
}

#if RUN_LARGE_TESTS
TEST(LiTest, LiShuffle_NoLyndon_Arg3_Weight6) {
  EXPECT_EXPR_EQ(
    + Li(2,2,2)({1},{2},{3})
    + Li(2,2,2)({1},{3},{2})
    + Li(2,2,2)({3},{1},{2})
    + Li(2,4)  ({1},{2,3})
    + Li(4,2)  ({1,3},{2})
    ,
    shuffle_product_expr(
      Li(2,2)({1},{2}),
      Li(2)  ({3})
    )
  );
}

TEST(LiTest, LiShuffle_NoLyndon_Arg4_Weight8) {
  EXPECT_EXPR_EQ(
    + Li(2,2,2,2)({1},{2},{3},{4})
    + Li(2,2,2,2)({1},{2},{4},{3})
    + Li(2,2,2,2)({1},{4},{2},{3})
    + Li(2,2,2,2)({4},{1},{2},{3})
    + Li(2,2,4)  ({1},{2},{3,4})
    + Li(2,4,2)  ({1},{2,4},{3})
    + Li(4,2,2)  ({1,4},{2},{3})
    ,
    shuffle_product_expr(
      Li(2,2,2)({1},{2},{3}),
      Li(2)    ({4})
    )
  );
}
#endif

TEST(CoLiTest, CoLiShuffle_Arg2_Weight2) {
  const LiParam li_a(1, {1,1},{{1},{2}});
  const LiParam li_b(1, {1,1},{{2},{1}});
  const LiParam li_c(1, {2},{{1,2}});
  EXPECT_EXPR_EQ(
    + CoLiVec(li_a)
    + CoLiVec(li_b)
    + CoLiVec(li_c)
    ,
    + coproduct(EFormalSymbolPositive(LiParam(1, {1}, {{1}})),
                EFormalSymbolPositive(LiParam(1, {1}, {{2}})))
    + coproduct(EFormalSymbolPositive(LiParam(1, {1}, {{2}})),
                EFormalSymbolPositive(LiParam(1, {1}, {{1}})))
    + coproduct(EUnity(), EFormalSymbolPositive(li_a))
    + coproduct(EUnity(), EFormalSymbolPositive(li_b))
    + coproduct(EUnity(), EFormalSymbolPositive(li_c))
    + coproduct(EFormalSymbolPositive(li_a), EUnity())
    + coproduct(EFormalSymbolPositive(li_b), EUnity())
    + coproduct(EFormalSymbolPositive(li_c), EUnity())
  );
}