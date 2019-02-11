#include <gtest/gtest.h>

extern "C" {
#include <antlr4/util/utf.h>
}


static const unsigned char broken_lead[] = "\xa2\xa2";
static const unsigned char broken_cont[] = "\xc2\x24";
static const unsigned char s_1[] = "\x24";
static const unsigned char s_2[] = "\xc2\xa2";
static const unsigned char s_3[] = "\xe2\x82\xac";
static const unsigned char s_4[] = "\xf0\x90\x8d\x88";


TEST(UTF, CodepointLen) {
    ASSERT_EQ(A4_CodepointLen(broken_lead, broken_lead + 2), 0);
    ASSERT_EQ(A4_CodepointLen(broken_cont, broken_cont + 2), 2);
    ASSERT_EQ(A4_CodepointLen(s_1, s_1 + 1), 1);
    ASSERT_EQ(A4_CodepointLen(s_2, s_2 + 1), 2);
    ASSERT_EQ(A4_CodepointLen(s_3, s_3 + 1), 3);
    ASSERT_EQ(A4_CodepointLen(s_4, s_4 + 1), 4);
}

TEST(UTF, CheckedCodepointLen) {
    ASSERT_EQ(A4_CheckedCodepointLen(broken_lead, broken_lead + 2), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(broken_cont, broken_cont + 2), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_1, s_1 + 1), 1);
    ASSERT_EQ(A4_CheckedCodepointLen(s_2, s_2 + 2), 2);
    ASSERT_EQ(A4_CheckedCodepointLen(s_3, s_3 + 3), 3);
    ASSERT_EQ(A4_CheckedCodepointLen(s_4, s_4 + 4), 4);
    ASSERT_EQ(A4_CheckedCodepointLen(s_1, s_1 + 0), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_2, s_2 + 1), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_3, s_3 + 2), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_4, s_4 + 3), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_2, s_2 + 0), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_3, s_3 + 1), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_4, s_4 + 2), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_3, s_3 + 0), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_4, s_4 + 1), 0);
    ASSERT_EQ(A4_CheckedCodepointLen(s_4, s_4 + 0), 0);
}

TEST(UTF, ReadCodepoint) {
    uint32_t rune = 0;
    ASSERT_EQ(A4_ReadCodepoint(&rune, broken_lead, broken_lead + 2), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, broken_cont, broken_cont + 2), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_1, s_1 + 1), 1);
    ASSERT_EQ(rune, 0x24);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_2, s_2 + 2), 2);
    ASSERT_EQ(rune, 0xA2);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_3, s_3 + 3), 3);
    ASSERT_EQ(rune, 0x20AC);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_4, s_4 + 4), 4);
    ASSERT_EQ(rune, 0x10348);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_1, s_1 + 0), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_2, s_2 + 1), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_3, s_3 + 2), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_4, s_4 + 3), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_2, s_2 + 0), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_3, s_3 + 1), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_4, s_4 + 2), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_3, s_3 + 0), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_4, s_4 + 1), 0);
    ASSERT_EQ(rune, 0xFFFD);
    ASSERT_EQ(A4_ReadCodepoint(&rune, s_4, s_4 + 0), 0);
    ASSERT_EQ(rune, 0xFFFD);
}
