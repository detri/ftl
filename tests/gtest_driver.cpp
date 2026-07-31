#include <gtest/gtest.h>

bool ftl_test();

TEST(FTL, works) {
    EXPECT_TRUE(ftl_test());
}
