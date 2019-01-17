#pragma once

#include "../common.h"
#include "../memory/pool.h"

#include <stdbool.h>
#include <stdlib.h>


/**
 * A set of integers implemented as a collection of non-overlapping intervals.
 *
 * We use interval set to store data for lexer 'char set' rules. Since those are constructed only once, we trade
 * insertion speed for lookup speed. That is, insertion is `O(n)` where `n` is number of intervals, while
 * lookup is `O(log(n))`.
 */
typedef struct A4_IntSet A4_IntSet;


// Set API
// --------------------------------------------------------------------------------------------------------------------

/// Allocate and init new set.
A4_IntSet* A4_IntSet_New();

/// Delete set.
void A4_IntSet_Delete(A4_IntSet* set);

/// Copy contents of the set so that it's allocated in the given pool. Returned set is immutable.
A4_IntSet* A4_IntSet_CopyToPool(const A4_IntSet* set, A4_MemoryPool* pool);

/// Remove all elements from the set.
void A4_IntSet_Clear(A4_IntSet* set);

/// Add new element to the set. If reallocation was unsuccessful, set is guaranteed to stay valid and unchanged.
A4_Errno A4_IntSet_Add(A4_IntSet* set, uint32_t elem);

/// Add new interval to the set. If reallocation was unsuccessful, set is guaranteed to stay valid and unchanged.
A4_Errno A4_IntSet_AddInterval(A4_IntSet* set, uint32_t lower, uint32_t upper);

/// Allocate a new chunk of memory on the set with the given alignment.
bool A4_IntSet_Contains(const A4_IntSet* set, uint32_t elem);

/// Get number of stored intervals.
size_t A4_IntSet_NumIntervals(const A4_IntSet* set);
