#include <gtest/gtest.h>

extern "C" {

struct Counted { int* ac; int x; };
struct Counted* Counted_New(int* ac, int x) {
    (*ac) += 1;
    return new Counted{ac, x};
}
void Counted_Delete(struct Counted* x) {
    *(x->ac) -= 1;
    free(x);
}


#define A4_LIST_NAME IntList
#define A4_LIST_PAYLOAD int
#define A4_LIST_NO_DTOR
#include <antlr4/util/list.inl>

#define A4_LIST_NAME CountedList
#define A4_LIST_PAYLOAD struct Counted*
#define A4_LIST_DTOR Counted_Delete
#include <antlr4/util/list.inl>

}

static inline int IntListGetter(int x) { return x; }
static inline int CountedListGetter(struct Counted* x) { return x->x; }

#define CHECK_LIST_CONTENTS(name, list, ...)                                                \
    {                                                                                       \
        int elems[] = {__VA_ARGS__};                                                        \
        size_t size = sizeof(elems) / sizeof(int);                                          \
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
    }

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
    int ac = 0;

    CountedList* list = CountedList_New();

    ASSERT_EQ(CountedList_Size(list), 0);

    ASSERT_EQ(ac, 0);

    ASSERT_EQ(CountedList_Append(list, Counted_New(&ac, 10)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10);
    ASSERT_EQ(ac, 1);

    ASSERT_EQ(CountedList_Append(list, Counted_New(&ac, 11)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10, 11);
    ASSERT_EQ(ac, 2);

    ASSERT_EQ(CountedList_Prepend(list, Counted_New(&ac, 9)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 10, 11);
    ASSERT_EQ(ac, 3);

    CountedList_RemoveFront(list);
    CHECK_LIST_CONTENTS(CountedList, list, 10, 11);
    ASSERT_EQ(ac, 2);

    CountedList_RemoveBack(list);
    CHECK_LIST_CONTENTS(CountedList, list, 10);
    ASSERT_EQ(ac, 1);

    ASSERT_EQ(CountedList_Append(list, Counted_New(&ac, 11)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10, 11);
    ASSERT_EQ(ac, 2);

    ASSERT_EQ(CountedList_Prepend(list, Counted_New(&ac, 9)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 10, 11);
    ASSERT_EQ(ac, 3);

    CountedList_Elem* elem = CountedList_Elem_Next(CountedList_Head(list));

    ASSERT_EQ(CountedList_Elem_Payload(elem)->x, 10);

    ASSERT_EQ(CountedList_InsertBefore(list, elem, Counted_New(&ac, 1)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10, 11);
    ASSERT_EQ(ac, 4);

    ASSERT_EQ(CountedList_InsertAfter(list, elem, Counted_New(&ac, 2)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10, 2, 11);
    ASSERT_EQ(ac, 5);

    CountedList_Remove(list, elem);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 2, 11);
    ASSERT_EQ(ac, 4);

    CountedList_Clear(list);

    ASSERT_EQ(CountedList_Size(list), 0);
    ASSERT_EQ(ac, 0);

    ASSERT_EQ(CountedList_AppendMove(list, Counted_New(&ac, 10)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 10);
    ASSERT_EQ(ac, 1);

    ASSERT_EQ(CountedList_PrependMove(list, Counted_New(&ac, 9)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 10);
    ASSERT_EQ(ac, 2);

    elem = CountedList_Elem_Next(CountedList_Head(list));

    ASSERT_EQ(CountedList_Elem_Payload(elem)->x, 10);

    ASSERT_EQ(CountedList_InsertBeforeMove(list, elem, Counted_New(&ac, 1)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10);
    ASSERT_EQ(ac, 3);

    ASSERT_EQ(CountedList_InsertAfterMove(list, elem, Counted_New(&ac, 2)), A4_SUCCESS);
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 10, 2);
    ASSERT_EQ(ac, 4);

    Counted_Delete(CountedList_Pop(list, elem));
    CHECK_LIST_CONTENTS(CountedList, list, 9, 1, 2);
    ASSERT_EQ(ac, 3);

    Counted_Delete(CountedList_PopFront(list));
    CHECK_LIST_CONTENTS(CountedList, list, 1, 2);
    ASSERT_EQ(ac, 2);

    Counted_Delete(CountedList_PopBack(list));
    CHECK_LIST_CONTENTS(CountedList, list, 1);
    ASSERT_EQ(ac, 1);

    Counted_Delete(CountedList_PopFront(list));

    ASSERT_EQ(CountedList_Size(list), 0);
    ASSERT_EQ(ac, 0);

    CountedList_Delete(list);
};

// TODO: copy to pool test
