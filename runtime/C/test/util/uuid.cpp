#include <gtest/gtest.h>

extern "C" {
#include <antlr4/util/uuid.h>
}


const A4_UUID canonical = (A4_UUID) {
    .data = {0x12345678, 0x9abcdef0, 0x21436587, 0xa9cbed0f}
};


TEST(UUID, Read) {
    {
        A4_UUID uuid = A4_UUID_Read("12345678-9abc-def0-2143-6587a9cbed0f");
        ASSERT_EQ(strncmp((char*)&uuid, (char*)&canonical, 16), 0);
    }
    {
        A4_UUID uuid = A4_UUID_Read("12345678-9ABC-DEF0-2143-6587A9CBED0F");
        ASSERT_EQ(strncmp((char*)&uuid, (char*)&canonical, 16), 0);
    }
}

TEST(UUID, ReadBinary) {
    const uint16_t bin[] = {0xed0f, 0xa9cb, 0x6587, 0x2143, 0xdef0, 0x9abc, 0x5678, 0x1234};
    A4_UUID uuid = A4_UUID_ReadBinary(bin);
    ASSERT_EQ(strncmp((char*)&uuid, (char*)&canonical, 16), 0);
}

TEST(UUID, Real) {
    const uint16_t bin[] = {0x6089, 0xa728, 0x8131, 0xb9eb, 0x417a, 0x3be5, 0x7784, 0x5962};
    A4_UUID uuid = A4_UUID_ReadBinary(bin);
    A4_UUID uuid2 = A4_UUID_Read("59627784-3BE5-417A-B9EB-8131A7286089");
    ASSERT_EQ(strncmp((char*)&uuid, (char*)&uuid2, 16), 0);
}
