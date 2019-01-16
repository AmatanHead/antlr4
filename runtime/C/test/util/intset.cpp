#include <gtest/gtest.h>

extern "C" {
#include <antlr4/util/intset.h>
}

#define CHECK_INTSET(set, a, b, c, d) \
    ASSERT_EQ(A4_IntSet_Contains(set, 0), a); \
    ASSERT_EQ(A4_IntSet_Contains(set, 1), b); \
    ASSERT_EQ(A4_IntSet_Contains(set, 10), c); \
    ASSERT_EQ(A4_IntSet_Contains(set, 100), d);

TEST(IntSet, Basics) {
    struct A4_IntSet* set = A4_IntSet_New();
    CHECK_INTSET(set, false, false, false, false);
    ASSERT_EQ(A4_IntSet_Size(set), 0);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, false, false);
    ASSERT_EQ(A4_IntSet_Size(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, false, false);
    ASSERT_EQ(A4_IntSet_Size(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 1), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 3);

    ASSERT_EQ(A4_IntSet_Add(set, 1), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 3);

    ASSERT_EQ(A4_IntSet_Add(set, 100), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, true);
    ASSERT_EQ(A4_IntSet_Size(set), 4);

    ASSERT_EQ(A4_IntSet_Add(set, 100), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, true);
    ASSERT_EQ(A4_IntSet_Size(set), 4);

    A4_IntSet_Clear(set);

    CHECK_INTSET(set, false, false, false, false);
    ASSERT_EQ(A4_IntSet_Size(set), 0);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, false, false, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, false, false, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_Size(set), 2);

    A4_IntSet_Delete(set);
}

TEST(IntSet, CopyToPool) {
    struct A4_IntSet* set = A4_IntSet_New();
    struct A4_MemoryPool* pool = A4_MemoryPool_NewDefault();

    struct A4_IntSet* set1 = A4_IntSet_CopyToPool(set, pool);
    ASSERT_NE(set1, (struct A4_IntSet*)NULL);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);

    struct A4_IntSet* set2 = A4_IntSet_CopyToPool(set, pool);
    ASSERT_NE(set1, (struct A4_IntSet*)NULL);

    ASSERT_EQ(A4_IntSet_Add(set, 100), A4_SUCCESS);

    struct A4_IntSet* set3 = A4_IntSet_CopyToPool(set, pool);
    ASSERT_NE(set1, (struct A4_IntSet*)NULL);

    A4_IntSet_Delete(set);

    CHECK_INTSET(set1, false, false, false, false);
    ASSERT_EQ(A4_IntSet_Size(set1), 0);
    CHECK_INTSET(set2, true, false, false, false);
    ASSERT_EQ(A4_IntSet_Size(set2), 1);
    CHECK_INTSET(set3, true, false, false, true);
    ASSERT_EQ(A4_IntSet_Size(set3), 2);

    A4_MemoryPool_Delete(pool);
}
