#include <gtest/gtest.h>

extern "C" {
#include <antlr4/memory/align.h>
}

TEST(Align, Basics) {
    ASSERT_EQ(A4_AlignUp(1, 4), 4);
    ASSERT_EQ(A4_AlignUp(2, 4), 4);
    ASSERT_EQ(A4_AlignUp(3, 4), 4);
    ASSERT_EQ(A4_AlignUp(4, 4), 4);
    ASSERT_EQ(A4_AlignUp(5, 4), 8);
    ASSERT_EQ(A4_AlignUp(6, 4), 8);
    ASSERT_EQ(A4_AlignUp(7, 4), 8);
    ASSERT_EQ(A4_AlignUp(8, 4), 8);
}
