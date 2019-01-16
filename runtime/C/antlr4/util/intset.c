#include "intset.h"

#include <assert.h>
#include <memory.h>
#include <errno.h>


// Set implementation
// --------------------------------------------------------------------------------------------------------------------

typedef unsigned A4_IntSet_ElemType;  // TODO: try 32 and 64 bit words on different arches

struct A4_IntSet {
    /// Where out bits stored.
    struct {
        A4_IntSet_ElemType* restrict data;
        size_t size;
    } bits;

    /// Number of elements in the set.
    size_t num_elements;

    /// Indicates that the set was allocated in a pool.
    bool is_immutable;
};

struct A4_IntSet* A4_IntSet_New() {
    struct A4_IntSet* set = malloc(sizeof(struct A4_IntSet));
    if (!set) return NULL;

    set->bits.data = NULL;
    set->bits.size = 0;
    set->num_elements = 0;
    set->is_immutable = false;

    return set;
}

void A4_IntSet_Delete(struct A4_IntSet* set) {
    if (!set) return;
    assert(!set->is_immutable);
    free(set->bits.data);
    free(set);
}

struct A4_IntSet* A4_IntSet_CopyToPool(const struct A4_IntSet* set, struct A4_MemoryPool* pool) {
    assert(set);

    struct A4_IntSet* new_set = A4_POOL_ALLOC(pool, struct A4_IntSet);
    if (!new_set) return NULL;

    new_set->bits.data = A4_POOL_CALLOC(pool, A4_IntSet_ElemType, set->bits.size);
    if (!new_set->bits.data) return NULL;

    memcpy(new_set->bits.data, set->bits.data, sizeof(A4_IntSet_ElemType) * set->bits.size);

    new_set->bits.size = set->bits.size;
    new_set->num_elements = set->num_elements;
    new_set->is_immutable = true;

    return new_set;
}

void A4_IntSet_Clear(struct A4_IntSet* set) {
    assert(set);
    assert(!set->is_immutable);
    free(set->bits.data);
    set->bits.data = NULL;
    set->bits.size = 0;
    set->num_elements = 0;
}

size_t A4_IntSet_Size(const struct A4_IntSet* set) {
    assert(set);
    return set->num_elements;
}

static inline A4_Errno A4_IntSet_Resize(struct A4_IntSet* set, const size_t size) {
    assert(set);
    assert(!set->is_immutable);

    if (set->bits.size >= size) return A4_SUCCESS;

    A4_IntSet_ElemType* new_data = realloc(set->bits.data, sizeof(A4_IntSet_ElemType) * size);
    if (!new_data) return ENOMEM;

    memset(new_data + set->bits.size, 0, sizeof(A4_IntSet_ElemType) * (size - set->bits.size));

    set->bits.data = new_data;
    set->bits.size = size;

    return A4_SUCCESS;
}

static inline A4_IntSet_ElemType A4_IntSet_GetMask(const size_t elem) {
    return ((A4_IntSet_ElemType)1) << (elem % (sizeof(A4_IntSet_ElemType) * 8));
}

A4_Errno A4_IntSet_Add(struct A4_IntSet* set, const size_t elem) {
    assert(set);
    assert(!set->is_immutable);

    const size_t index = elem / (sizeof(A4_IntSet_ElemType) * 8);

    if (index >= set->bits.size) {
        A4_Errno res = A4_IntSet_Resize(set, index + 1);
        if (res != A4_SUCCESS) return res;
    }

    assert(index < set->bits.size);

    A4_IntSet_ElemType mask = A4_IntSet_GetMask(elem);

    if (!(set->bits.data[index] & mask)) {
        set->num_elements += 1;
        set->bits.data[index] |= mask;
    }

    return A4_SUCCESS;
}

bool A4_IntSet_Contains(const struct A4_IntSet* set, const size_t elem) {
    assert(set);

    const size_t index = elem / (sizeof(A4_IntSet_ElemType) * 8);
    if (index >= set->bits.size) {
        return false;
    }

    A4_IntSet_ElemType mask = A4_IntSet_GetMask(elem);

    return (set->bits.data[index] & mask) != 0;
}
