#include <gtest/gtest.h>

extern "C" {
#include <antlr4/util/bitset.h>
}

#define CHECK_INTSET(set, a, b, c, d) \
    ASSERT_EQ(A4_BitSet_Contains(set, 0), a); \
    ASSERT_EQ(A4_BitSet_Contains(set, 1), b); \
    ASSERT_EQ(A4_BitSet_Contains(set, 10), c); \
    ASSERT_EQ(A4_BitSet_Contains(set, 100), d);

TEST(BitSet, Basics) {
    struct A4_BitSet* set = A4_BitSet_New();
    CHECK_INTSET(set, false, false, false, false);
    ASSERT_EQ(A4_BitSet_Size(set), 0);

    ASSERT_EQ(A4_BitSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, false, false);
    ASSERT_EQ(A4_BitSet_Size(set), 1);

    ASSERT_EQ(A4_BitSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, false, false);
    ASSERT_EQ(A4_BitSet_Size(set), 1);

    ASSERT_EQ(A4_BitSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 2);

    ASSERT_EQ(A4_BitSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 2);

    ASSERT_EQ(A4_BitSet_Add(set, 1), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 3);

    ASSERT_EQ(A4_BitSet_Add(set, 1), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 3);

    ASSERT_EQ(A4_BitSet_Add(set, 100), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, true);
    ASSERT_EQ(A4_BitSet_Size(set), 4);

    ASSERT_EQ(A4_BitSet_Add(set, 100), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, true);
    ASSERT_EQ(A4_BitSet_Size(set), 4);

    A4_BitSet_Clear(set);

    CHECK_INTSET(set, false, false, false, false);
    ASSERT_EQ(A4_BitSet_Size(set), 0);

    ASSERT_EQ(A4_BitSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, false, false, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 1);

    ASSERT_EQ(A4_BitSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, false, false, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 1);

    ASSERT_EQ(A4_BitSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 2);

    ASSERT_EQ(A4_BitSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_BitSet_Size(set), 2);

    A4_BitSet_Delete(set);
}

TEST(BitSet, CopyToPool) {
    struct A4_BitSet* set = A4_BitSet_New();
    struct A4_MemoryPool* pool = A4_MemoryPool_NewDefault();

    struct A4_BitSet* set1 = A4_BitSet_CopyToPool(set, pool);
    ASSERT_NE(set1, (struct A4_BitSet*)NULL);

    ASSERT_EQ(A4_BitSet_Add(set, 0), A4_SUCCESS);

    struct A4_BitSet* set2 = A4_BitSet_CopyToPool(set, pool);
    ASSERT_NE(set1, (struct A4_BitSet*)NULL);

    ASSERT_EQ(A4_BitSet_Add(set, 100), A4_SUCCESS);

    struct A4_BitSet* set3 = A4_BitSet_CopyToPool(set, pool);
    ASSERT_NE(set1, (struct A4_BitSet*)NULL);

    A4_BitSet_Delete(set);

    CHECK_INTSET(set1, false, false, false, false);
    ASSERT_EQ(A4_BitSet_Size(set1), 0);
    CHECK_INTSET(set2, true, false, false, false);
    ASSERT_EQ(A4_BitSet_Size(set2), 1);
    CHECK_INTSET(set3, true, false, false, true);
    ASSERT_EQ(A4_BitSet_Size(set3), 2);

    A4_MemoryPool_Delete(pool);
}
