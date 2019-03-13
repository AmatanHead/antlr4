#include <gtest/gtest.h>

#include "counted.h"

#define DEFAULT_KEY_MOVE(where, what)   \
    if ((where) != (what)) {            \
        A4_HT_KEY_DTOR((where));        \
        (where) = (what);               \
    }

#define DEFAULT_VALUE_MOVE(where, what) \
    if ((where) != (what)) {            \
        A4_HT_VALUE_DTOR((where));      \
        (where) = (what);               \
    }


extern "C" {
#define A4_HT_NAME IntIntHt
#define A4_HT_KEY uint32_t
#define A4_HT_VALUE uint32_t
#define A4_HT_KEY_NO_DTOR
#define A4_HT_VALUE_NO_DTOR
#define A4_HT_KEY_HASH(a) (a)
#define A4_HT_KEY_EQ(a, b) ((a) == (b))
#include <antlr4/util/hashtable.inl>

#define A4_HT_NAME IntCoHt
#define A4_HT_KEY uint32_t
#define A4_HT_VALUE Counted*
#define A4_HT_KEY_NO_DTOR
#define A4_HT_VALUE_DTOR Counted_Delete
#define A4_HT_VALUE_MOVE DEFAULT_VALUE_MOVE
#define A4_HT_KEY_HASH(a) (a)
#define A4_HT_KEY_EQ(a, b) ((a) == (b))
#include <antlr4/util/hashtable.inl>

#define A4_HT_NAME IntSet
#define A4_HT_KEY uint32_t
#define A4_HT_KEY_NO_DTOR
#define A4_HT_NO_VALUE
#define A4_HT_KEY_HASH(a) (a)
#define A4_HT_KEY_EQ(a, b) ((a) == (b))
#include <antlr4/util/hashtable.inl>

#define A4_HT_NAME CoIntHt
#define A4_HT_KEY Counted*
#define A4_HT_VALUE uint32_t
#define A4_HT_KEY_DTOR Counted_Delete
#define A4_HT_KEY_MOVE DEFAULT_KEY_MOVE
#define A4_HT_VALUE_NO_DTOR
#define A4_HT_KEY_HASH(a) ((a)->x)
#define A4_HT_KEY_EQ(a, b) (((a)->x) == ((b)->x))
#include <antlr4/util/hashtable.inl>

#define A4_HT_NAME CoCoHt
#define A4_HT_KEY Counted*
#define A4_HT_VALUE Counted*
#define A4_HT_KEY_DTOR Counted_Delete
#define A4_HT_KEY_MOVE DEFAULT_KEY_MOVE
#define A4_HT_VALUE_DTOR Counted_Delete
#define A4_HT_VALUE_MOVE DEFAULT_VALUE_MOVE
#define A4_HT_KEY_HASH(a) ((a)->x)
#define A4_HT_KEY_EQ(a, b) (((a)->x) == ((b)->x))
#include <antlr4/util/hashtable.inl>

#define A4_HT_NAME CoSet
#define A4_HT_KEY Counted*
#define A4_HT_KEY_DTOR Counted_Delete
#define A4_HT_KEY_MOVE DEFAULT_KEY_MOVE
#define A4_HT_NO_VALUE
#define A4_HT_KEY_HASH(a) ((a)->x)
#define A4_HT_KEY_EQ(a, b) (((a)->x) == ((b)->x))
#include <antlr4/util/hashtable.inl>
}

#define HT_INSERT(name, k, v)                                                                                         \
    do {                                                                                                              \
        ASSERT_EQ(name##_Insert(ht, make_k(k), make_v(v), &idx), A4_SUCCESS);                                         \
        canon[k] = v;                                                                                                 \
    } while (false)
#define HT_TRIVIAL_REPLACE(name, k)                                                                                   \
    do {                                                                                                              \
        ASSERT_EQ(name##_Insert(ht, make_k(k), name##_Get(ht, make_k_placeholder(k)), &idx), A4_SUCCESS);             \
    } while (false)
#define HT_CHECK(name)                                                                                                \
    do {                                                                                                              \
        ASSERT_EQ(name##_Size(ht), canon.size());                                                                     \
        ASSERT_EQ(name##_Empty(ht), canon.empty());                                                                   \
        for (uint32_t k = 0; k < 100; ++k) {                                                                          \
            ASSERT_EQ(name##_Contains(ht, make_k_placeholder(k)), canon.count(k));                                    \
            if (canon.count(k)) {                                                                                     \
                ASSERT_EQ(name##_SafeGet(ht, make_k_placeholder(k), &res), A4_SUCCESS);                               \
                v_compare(res, canon[k]);                                                                             \
                v_compare(name##_Get(ht, make_k_placeholder(k)), canon[k]);                                           \
            } else {                                                                                                  \
                ASSERT_EQ(name##_SafeGet(ht, make_k_placeholder(k), &res), A4_E_INDEX_ERROR);                         \
                ASSERT_EQ(name##_Remove(ht, make_k_placeholder(k)), A4_E_INDEX_ERROR);                                \
            }                                                                                                         \
        }                                                                                                             \
    } while (false)
#define HT_REMOVE(name, k)                                                                                            \
    do {                                                                                                              \
        ASSERT_EQ(name##_Remove(ht, make_k_placeholder(k)), A4_SUCCESS);                                              \
        canon.erase(k);                                                                                               \
    } while (false)
#define HT_CLEAR(name)                                                                                                \
    do {                                                                                                              \
        name##_Clear(ht);                                                                                             \
        canon.clear();                                                                                                \
    } while (false)

TEST(Hashtable, IntIntHt) {
    auto make_k = [](uint32_t k) { return k; };
    auto make_k_placeholder = [](uint32_t k) { return k; };
    auto make_v = [](uint32_t v) { return v; };
    auto v_compare = [](uint32_t res, uint32_t expected) { ASSERT_EQ(res, expected); };

    IntIntHt* ht = IntIntHt_New();
    std::map<uint32_t, uint32_t> canon;

    size_t idx;
    uint32_t res;

    HT_CHECK(IntIntHt);

    HT_INSERT(IntIntHt, 1, 2);
    HT_CHECK(IntIntHt);

    HT_INSERT(IntIntHt, 5, 6);
    HT_CHECK(IntIntHt);

    HT_INSERT(IntIntHt, 5, 7);
    HT_CHECK(IntIntHt);

    HT_INSERT(IntIntHt, 6, 8);
    HT_CHECK(IntIntHt);

    HT_REMOVE(IntIntHt, 5);
    HT_CHECK(IntIntHt);

    HT_REMOVE(IntIntHt, 6);
    HT_CHECK(IntIntHt);

    HT_CLEAR(IntIntHt);
    HT_CHECK(IntIntHt);

    HT_INSERT(IntIntHt, 5, 6);
    HT_CHECK(IntIntHt);

    HT_INSERT(IntIntHt, 6, 7);
    HT_CHECK(IntIntHt);

    for (uint32_t key = 5; key < 90; ++key) {
        HT_INSERT(IntIntHt, key, key + 1);
        HT_CHECK(IntIntHt);
    }

    IntIntHt_Delete(ht);
}

TEST(Hashtable, IntCoHt) {
    int new_calls = 0;
    int delete_calls = 0;

    auto make_k = [](uint32_t k) { return k; };
    auto make_k_placeholder = [](uint32_t k) { return k; };
    auto make_v = [&](uint32_t v) { return Counted_New({&new_calls, &delete_calls, v}); };
    auto v_compare = [](Counted* res, uint32_t expected) { ASSERT_EQ(res->x, expected); };

    IntCoHt* ht = IntCoHt_New();
    std::map<uint32_t, uint32_t> canon;

    size_t idx;
    Counted* res;

    HT_CHECK(IntCoHt);

    HT_INSERT(IntCoHt, 1, 2);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 1);
    ASSERT_EQ(delete_calls, 0);

    HT_INSERT(IntCoHt, 5, 6);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 2);
    ASSERT_EQ(delete_calls, 0);

    HT_INSERT(IntCoHt, 5, 7);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 3);
    ASSERT_EQ(delete_calls, 1);

    HT_INSERT(IntCoHt, 6, 8);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 4);
    ASSERT_EQ(delete_calls, 1);

    HT_TRIVIAL_REPLACE(IntCoHt, 6);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 4);
    ASSERT_EQ(delete_calls, 1);

    HT_REMOVE(IntCoHt, 5);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 4);
    ASSERT_EQ(delete_calls, 2);

    HT_REMOVE(IntCoHt, 6);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 4);
    ASSERT_EQ(delete_calls, 3);

    HT_CLEAR(IntCoHt);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 4);
    ASSERT_EQ(delete_calls, 4);

    HT_INSERT(IntCoHt, 5, 6);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 5);
    ASSERT_EQ(delete_calls, 4);

    HT_INSERT(IntCoHt, 6, 7);
    HT_CHECK(IntCoHt);
    ASSERT_EQ(new_calls, 6);
    ASSERT_EQ(delete_calls, 4);

    for (uint32_t key = 5; key < 90; ++key) {
        HT_INSERT(IntCoHt, key, key + 1);
        HT_CHECK(IntCoHt);
    }

    ASSERT_EQ(new_calls, 91);
    ASSERT_EQ(delete_calls, 6);

    IntCoHt_Delete(ht);

    ASSERT_EQ(new_calls, 91);
    ASSERT_EQ(delete_calls, 91);
}
