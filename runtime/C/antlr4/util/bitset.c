#include "bitset.h"

#include <assert.h>
#include <memory.h>
#include <errno.h>


// Set implementation
// --------------------------------------------------------------------------------------------------------------------

typedef unsigned A4_BitSet_ElemType;  // TODO: try 32 and 64 bit words on different arches

struct A4_BitSet {
    /// Where our bits stored.
    struct {
        A4_BitSet_ElemType* restrict data;
        size_t size;
    } bits;

    /// Number of elements in the set.
    size_t num_elements;

    /// Indicates that the set was allocated in a pool.
    bool is_immutable;
};

A4_BitSet* A4_BitSet_New() {
    A4_BitSet* set = malloc(sizeof(A4_BitSet));
    if (!set) return NULL;

    set->bits.data = NULL;
    set->bits.size = 0;
    set->num_elements = 0;
    set->is_immutable = false;

    return set;
}

void A4_BitSet_Delete(A4_BitSet* set) {
    if (!set) return;
    assert(!set->is_immutable);
    free(set->bits.data);
    free(set);
}

A4_BitSet* A4_BitSet_CopyToPool(const A4_BitSet* set, A4_MemoryPool* pool) {
    assert(set);

    A4_BitSet* new_set = A4_POOL_ALLOC(pool, A4_BitSet);
    if (!new_set) return NULL;

    new_set->bits.data = A4_POOL_CALLOC(pool, A4_BitSet_ElemType, set->bits.size);
    if (!new_set->bits.data) return NULL;

    memcpy(new_set->bits.data, set->bits.data, sizeof(A4_BitSet_ElemType) * set->bits.size);

    new_set->bits.size = set->bits.size;
    new_set->num_elements = set->num_elements;
    new_set->is_immutable = true;

    return new_set;
}

void A4_BitSet_Clear(A4_BitSet* set) {
    assert(set);
    assert(!set->is_immutable);
    free(set->bits.data);
    set->bits.data = NULL;
    set->bits.size = 0;
    set->num_elements = 0;
}

size_t A4_BitSet_Size(const A4_BitSet* set) {
    assert(set);
    return set->num_elements;
}

static inline A4_ERRNO A4_BitSet_Resize(A4_BitSet* set, const size_t size) {
    assert(set);
    assert(!set->is_immutable);

    if (set->bits.size >= size) return A4_SUCCESS;

    A4_BitSet_ElemType* new_data = realloc(set->bits.data, sizeof(A4_BitSet_ElemType) * size);
    if (!new_data) return ENOMEM;

    memset(new_data + set->bits.size, 0, sizeof(A4_BitSet_ElemType) * (size - set->bits.size));

    set->bits.data = new_data;
    set->bits.size = size;

    return A4_SUCCESS;
}

static inline A4_BitSet_ElemType A4_BitSet_GetMask(const uint32_t elem) {
    return ((A4_BitSet_ElemType)1) << (elem % (sizeof(A4_BitSet_ElemType) * 8));
}

A4_ERRNO A4_BitSet_Add(A4_BitSet* set, const uint32_t elem) {
    assert(set);
    assert(!set->is_immutable);

    const size_t index = elem / (sizeof(A4_BitSet_ElemType) * 8);

    if (index >= set->bits.size) {
        A4_Errno res = A4_BitSet_Resize(set, index + 1);
        if (res != A4_SUCCESS) return res;
    }

    assert(index < set->bits.size);

    A4_BitSet_ElemType mask = A4_BitSet_GetMask(elem);

    if (!(set->bits.data[index] & mask)) {
        set->num_elements += 1;
        set->bits.data[index] |= mask;
    }

    return A4_SUCCESS;
}

bool A4_BitSet_Contains(const A4_BitSet* set, const uint32_t elem) {
    assert(set);

    const size_t index = elem / (sizeof(A4_BitSet_ElemType) * 8);
    if (index >= set->bits.size) {
        return false;
    }

    return (set->bits.data[index] & A4_BitSet_GetMask(elem)) != 0;
}
