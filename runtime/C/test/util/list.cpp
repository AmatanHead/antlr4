#include <gtest/gtest.h>

#include "counted.h"

extern "C" {
#define A4_LIST_NAME IntList
#define A4_LIST_PAYLOAD uint32_t
#define A4_LIST_NO_DTOR
#include <antlr4/util/list.inl>

#define A4_LIST_NAME CountedList
#define A4_LIST_PAYLOAD struct Counted*
#define A4_LIST_DTOR Counted_Delete
#include <antlr4/util/list.inl>
}

static inline uint32_t IntListGetter(uint32_t x) { return x; }
static inline uint32_t CountedListGetter(struct Counted* x) { return x->x; }

#define CHECK_LIST_CONTENTS(name, list, ...)                                                \
    do {                                                                                    \
        uint32_t elems[] = {__VA_ARGS__};                                                   \
        size_t size = sizeof(elems) / sizeof(uint32_t);                                     \
                                                                                            \
        ASSERT_EQ(name##_Size(list), size);                                                 \
        ASSERT_EQ(name##Getter(name##_First(list)), elems[0]);                              \
        ASSERT_EQ(name##Getter(name##_Last(list)), elems[size - 1]);                        \
                                                                                            \
        {                                                                                   \
            name##_Elem* head = name##_Head(list);                                          \
            for (size_t i = 0; i < size; ++i) {                                             \
                ASSERT_NE(head, nullptr);                                                   \
                ASSERT_EQ(name##Getter(name##_Elem_Payload(head)), elems[i]);               \
                if (i == size - 1) {                                                        \
                    ASSERT_FALSE(name##_Elem_HasNext(head));                                \
                } else {                                                                    \
                    ASSERT_TRUE(name##_Elem_HasNext(head));                                 \
                }                                                                           \
                head = name##_Elem_Next(head);                                              \
            }                                                                               \
            ASSERT_EQ(head, nullptr);                                                       \
        }                                                                                   \
                                                                                            \
        {                                                                                   \
            name##_Elem* tail = name##_Tail(list);                                          \
            for (size_t i = 0; i < size; ++i) {                                             \
                ASSERT_NE(tail, nullptr);                                                   \
                ASSERT_EQ(name##Getter(name##_Elem_Payload(tail)), elems[size - i - 1]);    \
                if (i == size - 1) {                                                        \
                    ASSERT_FALSE(name##_Elem_HasPrevious(tail));                            \
                } else {                                                                    \
                    ASSERT_TRUE(name##_Elem_HasPrevious(tail));                             \
                }                                                                           \
                tail = name##_Elem_Previous(tail);                                          \
            }                                                                               \
            ASSERT_EQ(tail, nullptr);                                                       \
        }                                                                                   \
    } while (false)

TEST(List, SimpleList) {
    IntList* list = IntList_New();

    ASSERT_EQ(IntList_Size(list), 0);

    ASSERT_EQ(IntList_Append(list, 10), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 10);

    ASSERT_EQ(IntList_Append(list, 11), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 10, 11);

    ASSERT_EQ(IntList_Prepend(list, 9), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 9, 10, 11);

    IntList_RemoveFront(list);
    CHECK_LIST_CONTENTS(IntList, list, 10, 11);

    IntList_RemoveBack(list);
    CHECK_LIST_CONTENTS(IntList, list, 10);

    ASSERT_EQ(IntList_Append(list, 11), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 10, 11);

    ASSERT_EQ(IntList_Prepend(list, 9), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 9, 10, 11);

    IntList_Elem* elem = IntList_Elem_Next(IntList_Head(list));

    ASSERT_EQ(IntList_Elem_Payload(elem), 10);

    ASSERT_EQ(IntList_InsertBefore(list, elem, 1), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 9, 1, 10, 11);

    ASSERT_EQ(IntList_InsertAfter(list, elem, 2), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 9, 1, 10, 2, 11);

    IntList_Remove(list, elem);
    CHECK_LIST_CONTENTS(IntList, list, 9, 1, 2, 11);

    IntList_Clear(list);

    ASSERT_EQ(IntList_Size(list), 0);

    ASSERT_EQ(IntList_AppendMove(list, 10), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 10);

    ASSERT_EQ(IntList_PrependMove(list, 9), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 9, 10);

    elem = IntList_Elem_Next(IntList_Head(list));

    ASSERT_EQ(IntList_Elem_Payload(elem), 10);

    ASSERT_EQ(IntList_InsertBeforeMove(list, elem, 1), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 9, 1, 10);

    ASSERT_EQ(IntList_InsertAfterMove(list, elem, 2), A4_SUCCESS);
    CHECK_LIST_CONTENTS(IntList, list, 9, 1, 10, 2);

    IntList_Pop(list, elem);  // IntList_Pop return value can be ignored because of A4_LIST_NO_DTOR
    CHECK_LIST_CONTENTS(IntList, list, 9, 1, 2);

    IntList_PopFront(list);
    CHECK_LIST_CONTENTS(IntList, list, 1, 2);

    IntList_PopBack(list);
    CHECK_LIST_CONTENTS(IntList, list, 1);

    ASSERT_EQ(IntList_PopFront(list), 1);

    ASSERT_EQ(IntList_Size(list), 0);

    IntList_Delete(list);
}

TEST(List, CountedList) {
    int new_calls = 0;
    int delete_calls = 0;

    CountedList* list = CountedList_New();

    ASSERT_EQ(CountedList_Size(list), 0);

    ASSERT_EQ(new_calls, 0);
    ASSERT_EQ(delete_calls, 0);

    ASSERT_EQ(CountedList_Append(list, Counted_New({&new_calls, &delete_calls, 10})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10);
    ASSERT_EQ(new_calls, 1);
    ASSERT_EQ(delete_calls, 0);

    ASSERT_EQ(CountedList_Append(list, Counted_New({&new_calls, &delete_calls, 11})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10, 11);
    ASSERT_EQ(new_calls, 2);
    ASSERT_EQ(delete_calls, 0);

    ASSERT_EQ(CountedList_Prepend(list, Counted_New({&new_calls, &delete_calls, 9})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 10, 11);
    ASSERT_EQ(new_calls, 3);
    ASSERT_EQ(delete_calls, 0);

    CountedList_RemoveFront(list);
    CHECK_LIST_CONTENTS(CountedList, list, 10, 11);
    ASSERT_EQ(new_calls, 3);
    ASSERT_EQ(delete_calls, 1);

    CountedList_RemoveBack(list);
    CHECK_LIST_CONTENTS(CountedList, list, 10);
    ASSERT_EQ(new_calls, 3);
    ASSERT_EQ(delete_calls, 2);

    ASSERT_EQ(CountedList_Append(list, Counted_New({&new_calls, &delete_calls, 11})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10, 11);
    ASSERT_EQ(new_calls, 4);
    ASSERT_EQ(delete_calls, 2);

    ASSERT_EQ(CountedList_Prepend(list, Counted_New({&new_calls, &delete_calls, 9})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 10, 11);
    ASSERT_EQ(new_calls, 5);
    ASSERT_EQ(delete_calls, 2);

    CountedList_Elem* elem = CountedList_Elem_Next(CountedList_Head(list));

    ASSERT_EQ(CountedList_Elem_Payload(elem)->x, 10);

    ASSERT_EQ(CountedList_InsertBefore(list, elem, Counted_New({&new_calls, &delete_calls, 1})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10, 11);
    ASSERT_EQ(new_calls, 6);
    ASSERT_EQ(delete_calls, 2);

    ASSERT_EQ(CountedList_InsertAfter(list, elem, Counted_New({&new_calls, &delete_calls, 2})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10, 2, 11);
    ASSERT_EQ(new_calls, 7);
    ASSERT_EQ(delete_calls, 2);

    CountedList_Remove(list, elem);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 2, 11);
    ASSERT_EQ(new_calls, 7);
    ASSERT_EQ(delete_calls, 3);

    CountedList_Clear(list);

    ASSERT_EQ(CountedList_Size(list), 0);
    ASSERT_EQ(new_calls, 7);
    ASSERT_EQ(delete_calls, 7);

    ASSERT_EQ(CountedList_AppendMove(list, Counted_New({&new_calls, &delete_calls, 10})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10);
    ASSERT_EQ(new_calls, 8);
    ASSERT_EQ(delete_calls, 7);

    ASSERT_EQ(CountedList_PrependMove(list, Counted_New({&new_calls, &delete_calls, 9})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 10);
    ASSERT_EQ(new_calls, 9);
    ASSERT_EQ(delete_calls, 7);

    elem = CountedList_Elem_Next(CountedList_Head(list));

    ASSERT_EQ(CountedList_Elem_Payload(elem)->x, 10);

    ASSERT_EQ(CountedList_InsertBeforeMove(list, elem, Counted_New({&new_calls, &delete_calls, 1})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10);
    ASSERT_EQ(new_calls, 10);
    ASSERT_EQ(delete_calls, 7);

    ASSERT_EQ(CountedList_InsertAfterMove(list, elem, Counted_New({&new_calls, &delete_calls, 2})), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10, 2);
    ASSERT_EQ(new_calls, 11);
    ASSERT_EQ(delete_calls, 7);

    Counted_Delete(CountedList_Pop(list, elem));
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 2);
    ASSERT_EQ(new_calls, 11);
    ASSERT_EQ(delete_calls, 8);

    Counted_Delete(CountedList_PopFront(list));
    CHECK_LIST_CONTENTS(CountedList, list, 1, 2);
    ASSERT_EQ(new_calls, 11);
    ASSERT_EQ(delete_calls, 9);

    Counted_Delete(CountedList_PopBack(list));
    CHECK_LIST_CONTENTS(CountedList, list, 1);
    ASSERT_EQ(new_calls, 11);
    ASSERT_EQ(delete_calls, 10);

    Counted_Delete(CountedList_PopFront(list));

    ASSERT_EQ(CountedList_Size(list), 0);
    ASSERT_EQ(new_calls, 11);
    ASSERT_EQ(delete_calls, 11);

    CountedList_Delete(list);

    ASSERT_EQ(new_calls, 11);
    ASSERT_EQ(delete_calls, 11);
};

// TODO: copy to pool test
