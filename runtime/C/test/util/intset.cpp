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
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 0);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, false, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, false, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 1), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 1), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 100), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, true);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 3);

    ASSERT_EQ(A4_IntSet_Add(set, 100), A4_SUCCESS);
    CHECK_INTSET(set, true, true, true, true);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 3);

    A4_IntSet_Clear(set);

    CHECK_INTSET(set, false, false, false, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 0);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, false, false, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 10), A4_SUCCESS);
    CHECK_INTSET(set, false, false, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 1);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 2);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);
    CHECK_INTSET(set, true, false, true, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set), 2);

    A4_IntSet_Delete(set);
}

TEST(IntSet, CopyToPool) {
    struct A4_IntSet* set = A4_IntSet_New();
    struct A4_MemoryPool* pool = A4_MemoryPool_NewDefault();

    struct A4_IntSet* set1 = A4_IntSet_CopyToPool(set, pool);
    ASSERT_NE(set1, nullptr);

    ASSERT_EQ(A4_IntSet_Add(set, 0), A4_SUCCESS);

    struct A4_IntSet* set2 = A4_IntSet_CopyToPool(set, pool);
    ASSERT_NE(set1, nullptr);

    ASSERT_EQ(A4_IntSet_Add(set, 100), A4_SUCCESS);

    struct A4_IntSet* set3 = A4_IntSet_CopyToPool(set, pool);
    ASSERT_NE(set1, nullptr);

    A4_IntSet_Delete(set);

    CHECK_INTSET(set1, false, false, false, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set1), 0);
    CHECK_INTSET(set2, true, false, false, false);
    ASSERT_EQ(A4_IntSet_NumIntervals(set2), 1);
    CHECK_INTSET(set3, true, false, false, true);
    ASSERT_EQ(A4_IntSet_NumIntervals(set3), 2);

    A4_MemoryPool_Delete(pool);
}


#define ADD_INTSET_DYNAMIC(lo, hi, in)                      \
    for (size_t i = lo; i <= hi; ++i) results[i] = true;    \
    A4_IntSet_AddInterval(set, lo, hi);                     \
    ASSERT_EQ(A4_IntSet_NumIntervals(set), in);             \
    CHECK_INTSET_DYNAMIC();

#define CHECK_INTSET_DYNAMIC() \
    for (size_t i = 0; i < 100; ++i) ASSERT_EQ(A4_IntSet_Contains(set, i), results[i]);


TEST(IntSet, Intervals) {
    bool results[100];
    for (bool& result : results) result = false;

    struct A4_IntSet* set = A4_IntSet_New();

    ASSERT_EQ(A4_IntSet_NumIntervals(set), 0);

    ADD_INTSET_DYNAMIC(10, 15, 1);
    ADD_INTSET_DYNAMIC(20, 25, 2);
    ADD_INTSET_DYNAMIC(17, 18, 3);
    ADD_INTSET_DYNAMIC(15, 17, 2);
    ADD_INTSET_DYNAMIC(15, 25, 1);

    A4_IntSet_Clear(set);
    for (bool& result : results) result = false;

    ASSERT_EQ(A4_IntSet_NumIntervals(set), 0);

    ADD_INTSET_DYNAMIC(10, 10, 1);
    ADD_INTSET_DYNAMIC(11, 15, 1);
    ADD_INTSET_DYNAMIC(9, 9, 1);

    A4_IntSet_Clear(set);
    for (bool& result : results) result = false;

    ASSERT_EQ(A4_IntSet_NumIntervals(set), 0);

    for (uint32_t x = 0; x <= 40; ++x) {
        ADD_INTSET_DYNAMIC(x * 2, x * 2, x + 1);
    }

    for (uint32_t x = 0; x < 40; ++x) {
        ADD_INTSET_DYNAMIC(x * 2 + 1, x * 2 + 1, 40 - x);
    }

    A4_IntSet_Delete(set);
}
