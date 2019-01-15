#include <gtest/gtest.h>

#include <antlr4/memory/pool.h>

#define CHECK_POOL(allocated, left, waste)               \
    ASSERT_EQ(allocated, A4_MemoryPool_Allocated(pool)); \
    ASSERT_EQ(left, A4_MemoryPool_Left(pool));           \
    ASSERT_EQ(waste, A4_MemoryPool_Waste(pool));

TEST(MemoryPoolTest, TestBasics) {
    struct A4_MemoryPool* pool = A4_MemoryPool_NewDefault();
    CHECK_POOL(0, 0, 0)
    char* mem1 = (char*) A4_MemoryPool_Malloc(pool, 5);
    CHECK_POOL(2048 + 24, 2048 - 5, 2048 - 5)
    char* mem2 = (char*) A4_MemoryPool_Malloc(pool, 5);
    CHECK_POOL(2048 + 24, 2048 - 10, 2048 - 10)
    char* mem3 = (char*) A4_MemoryPool_Malloc(pool, 10000);
    CHECK_POOL(2048 + 24 + 10000 + 24, 0, 2048 - 10)

    ASSERT_NE(mem1, nullptr);
    ASSERT_NE(mem2, nullptr);
    ASSERT_NE(mem3, nullptr);

    ASSERT_NE(mem1, mem2);
    ASSERT_NE(mem2, mem3);
    ASSERT_NE(mem3, mem1);

    ASSERT_EQ(mem2 - mem1, 5);

    for (char i = 0; i < 5; ++i) mem1[i] = i;
    for (char i = 0; i < 5; ++i) mem2[i] = (char)5 - i;
    for (char i = 0; i < 5; ++i) ASSERT_NE(mem1[i], mem2[i]);

    A4_MemoryPool_Clear(pool);

    CHECK_POOL(0, 0, 0)

    char* mem4 = (char*) A4_MemoryPool_MallocAligned(pool, 5, 16);
    CHECK_POOL(2048 + 24, 2048 - 8 - 5, 2048 - 8 - 5)
    ASSERT_EQ((intptr_t)mem4 % 16, 0);

    A4_MemoryPool_Malloc(pool, 1);
    CHECK_POOL(2048 + 24, 2048 - 8 - 5 - 1, 2048 - 8 - 5 - 1)

    char* mem5 = (char*) A4_MemoryPool_MallocAligned(pool, 5, 8);
    CHECK_POOL(2048 + 24, 2048 - 8 - 5 - 1 - 2 - 5, 2048 - 8 - 5 - 1 - 2 - 5)
    ASSERT_EQ((intptr_t)mem5 % 8, 0);
    ASSERT_EQ((intptr_t)mem5 % 16, 8);

    auto mem6 = A4_POOL_ALLOC(pool, max_align_t);
    ASSERT_EQ((intptr_t)mem6 % 16, 0);

    A4_MemoryPool_Delete(pool);
}
