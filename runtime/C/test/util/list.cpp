#include <gtest/gtest.h>

extern "C" {

static int alloc_counter = 0;
struct Counted { int x; };
struct Counted* Counted_New(void) {
    alloc_counter += 1;
    return new Counted{alloc_counter};
}

#define A4_LIST_NAME IntList
#define A4_LIST_PAYLOAD int
#define A4_LIST_NO_DTOR
#include <antlr4/util/list.inl>

#define A4_LIST_NAME CountedList
#define A4_LIST_PAYLOAD struct Counted*
#define A4_LIST_DTOR(x)     \
    {                       \
        alloc_counter -= 1; \
        free(x);            \
    };
#include <antlr4/util/list.inl>

}
