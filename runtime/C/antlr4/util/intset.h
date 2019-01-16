#pragma once

#include "../common.h"
#include "../memory/pool.h"

#include <stdbool.h>
#include <stdlib.h>


/**
 * A set of integers.
 *
 * Antlr uses integer sets to store state ids. Since number of states is usually low (below 10000),
 * we use bitset here.
 *
 * TODO: try different implementations (trees, hashes, intervals).
 */
struct A4_IntSet;


// Set API
// --------------------------------------------------------------------------------------------------------------------

/// Allocate and init new set.
struct A4_IntSet* A4_IntSet_New();

/// Delete set.
void A4_IntSet_Delete(struct A4_IntSet* set);

/// Copy contents of the set so that it's allocated in the given pool. Returned set is immutable.
struct A4_IntSet* A4_IntSet_CopyToPool(const struct A4_IntSet* set, struct A4_MemoryPool* pool);

/// Remove all elements from the set.
void A4_IntSet_Clear(struct A4_IntSet* set);

/// Get number of elements in the set.
size_t A4_IntSet_Size(const struct A4_IntSet* set);

/// Add new element to the set. If reallocation was unsuccessful, set is guaranteed to stay valid and unchanged.
A4_Errno A4_IntSet_Add(struct A4_IntSet* set, size_t elem);

/// Allocate a new chunk of memory on the set with the given alignment.
bool A4_IntSet_Contains(const struct A4_IntSet* set, size_t elem);
