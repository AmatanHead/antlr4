#include "intset.h"

#include <assert.h>
#include <memory.h>
#include <errno.h>
#include <printf.h>


// Set implementation
// --------------------------------------------------------------------------------------------------------------------

struct A4_IntSet_Interval {
    uint32_t lower;
    uint32_t upper;
};

struct A4_IntSet {
    /// Where our intervals stored.
    struct {
        struct A4_IntSet_Interval* restrict data;
        size_t size;
        size_t capacity;
    } intervals;

    /// Indicates that the set was allocated in a pool.
    bool is_immutable;
};

struct A4_IntSet* A4_IntSet_New() {
    struct A4_IntSet* set = malloc(sizeof(struct A4_IntSet));
    if (!set) return NULL;

    set->intervals.data = NULL;
    set->intervals.size = 0;
    set->intervals.capacity = 0;
    set->is_immutable = false;

    return set;
}

void A4_IntSet_Delete(struct A4_IntSet* set) {
    if (!set) return;
    assert(!set->is_immutable);
    free(set->intervals.data);
    free(set);
}

struct A4_IntSet* A4_IntSet_CopyToPool(const struct A4_IntSet* set, struct A4_MemoryPool* pool) {
    assert(set);

    struct A4_IntSet* new_set = A4_POOL_ALLOC(pool, struct A4_IntSet);
    if (!new_set) return NULL;

    new_set->intervals.data = A4_POOL_CALLOC(pool, struct A4_IntSet_Interval, set->intervals.size);
    if (!new_set->intervals.data) return NULL;

    memcpy(new_set->intervals.data, set->intervals.data, sizeof(struct A4_IntSet_Interval) * set->intervals.size);

    new_set->intervals.size = set->intervals.size;
    new_set->intervals.capacity = new_set->intervals.size;
    new_set->is_immutable = true;

    return new_set;
}

void A4_IntSet_Clear(struct A4_IntSet* set) {
    assert(set);
    assert(!set->is_immutable);
    free(set->intervals.data);
    set->intervals.data = NULL;
    set->intervals.size = 0;
    set->intervals.capacity = 0;
}

/// Finds the leftmost interval that has its upper bound greater than or equal to the given element.
/// Returns a past-the-end pointer if such interval cannot be found.
static inline struct A4_IntSet_Interval* A4_IntSet_FindInterval(const struct A4_IntSet* set, const uint32_t elem) {
    assert(set);

    if (set->intervals.size < 10) {  // TODO: check how this constant affect parsing speed
        for (size_t i = 0; i < set->intervals.size; ++i)
            if (set->intervals.data[i].upper >= elem)
                return set->intervals.data + i;
        return set->intervals.data + set->intervals.size;
    } else {
        size_t l = 0, r = set->intervals.size;
        while (l < r) {
            size_t i = (l + r) / 2;
            if (set->intervals.data[i].upper < elem)
                l = i + 1;
            else
                r = i;
        }
        return set->intervals.data + l;
    }
}

/// Check if point lies within an interval.
static inline bool A4_IntSet_Within(const uint32_t point, const struct A4_IntSet_Interval a) {
    return a.lower <= point && point <= a.upper;
}

/// Check if two intervals are disjoint (i.e. they do not overlap).
static inline bool A4_IntSet_Disjoint(const struct A4_IntSet_Interval a, const struct A4_IntSet_Interval b) {
    return !(
        A4_IntSet_Within(b.lower, a) ||
        A4_IntSet_Within(b.upper, a) ||
        A4_IntSet_Within(a.lower, b) ||
        A4_IntSet_Within(a.upper, b)
    );
}

/// Check if two intervals are adjacent (i.e. they do not overlap but lay next to each other).
static inline bool A4_IntSet_Adjacent(const struct A4_IntSet_Interval a, const struct A4_IntSet_Interval b) {
    return a.lower == b.upper + 1 || a.upper == b.lower - 1;
}

A4_Errno A4_IntSet_Add(struct A4_IntSet* set, const uint32_t elem) {
    return A4_IntSet_AddInterval(set, elem, elem);
}

A4_Errno A4_IntSet_InsertInterval(struct A4_IntSet* set, size_t where, struct A4_IntSet_Interval interval) {
    assert(set);
    assert(set->intervals.capacity >= set->intervals.size);

    if (set->intervals.capacity == set->intervals.size) {
        // no capacity -- reallocate
        // note that set with data==NULL falls into this case (capacity == size == 0)

        size_t new_capacity = set->intervals.size + 5;
        struct A4_IntSet_Interval* new_data = realloc(set->intervals.data, sizeof(struct A4_IntSet_Interval) * new_capacity);
        if (!new_data) return ENOMEM;

        set->intervals.data = new_data;
        set->intervals.capacity = new_capacity;
    }

    size_t tail_size = set->intervals.size - where;
    memmove(set->intervals.data + where + 1, set->intervals.data + where, sizeof(struct A4_IntSet_Interval) * tail_size);
    set->intervals.data[where] = interval;
    set->intervals.size += 1;

    return A4_SUCCESS;
}

A4_Errno A4_IntSet_AddInterval(struct A4_IntSet* set, const uint32_t lower, const uint32_t upper) {
    assert(set);

    struct A4_IntSet_Interval interval = {lower, upper};

    for (size_t i = 0; i < set->intervals.size; ++i) {
        if (A4_IntSet_Adjacent(set->intervals.data[i], interval) || !A4_IntSet_Disjoint(set->intervals.data[i], interval)) {
            // new interval overlaps with some pre-existing one
            size_t j = i;
            do {
                if (set->intervals.data[i].lower < interval.lower)
                    interval.lower = set->intervals.data[i].lower;
                if (set->intervals.data[i].upper > interval.upper)
                    interval.upper = set->intervals.data[i].upper;
                i++;
            } while (i < set->intervals.size && (
                A4_IntSet_Adjacent(set->intervals.data[i], interval) ||
                !A4_IntSet_Disjoint(set->intervals.data[i], interval)));

            set->intervals.data[j++] = interval;

            if (j != i) {
                while (i < set->intervals.size) {
                    set->intervals.data[j++] = set->intervals.data[i++];
                }
                set->intervals.size = j;
            }

            return A4_SUCCESS;
        } else if (interval.upper < set->intervals.data[i].lower) {
            // new interval has to be inserted at this position
            return A4_IntSet_InsertInterval(set, i, interval);
        }
    }

    // we have not found a proper position -- insert at to the end
    return A4_IntSet_InsertInterval(set, set->intervals.size, interval);
}

bool A4_IntSet_Contains(const struct A4_IntSet* set, const uint32_t elem) {
    assert(set);

    struct A4_IntSet_Interval* interval = A4_IntSet_FindInterval(set, elem);
    return interval < (set->intervals.data + set->intervals.size) && interval->lower <= elem;
}

size_t A4_IntSet_NumIntervals(const struct A4_IntSet* set) {
    assert(set);

    return set->intervals.size;
}
