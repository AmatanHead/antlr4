#ifndef INC_9888A0F95EF34AA891A58D31A75A2F51
#define INC_9888A0F95EF34AA891A58D31A75A2F51

#include <antlr4/util/pool.h>

#include <antlr4/common.h>

#include <stdbool.h>
#include <stdlib.h>


/**
 * A set of integers implemented as a bitset.
 *
 * Antlr uses integer sets to store state ids. Since number of states is usually low (below 10000),
 * we use bitset here.
 *
 * TODO: try using interval sets instead.
 */
typedef struct A4_BitSet A4_BitSet;


// Set API
// --------------------------------------------------------------------------------------------------------------------

/// Allocate and init new set.
A4_BitSet* A4_NODISCARD A4_BitSet_New(void);

/// Delete set.
void A4_BitSet_Delete(A4_BitSet* set);

/// Copy contents of the set so that it's allocated in the given pool. Returned set is immutable.
A4_BitSet* A4_BitSet_CopyToPool(const A4_BitSet* set, struct A4_MemoryPool* pool);

/// Remove all elements from the set.
void A4_BitSet_Clear(A4_BitSet* set);

/// Get number of elements in the set.
size_t A4_BitSet_Size(const A4_BitSet* set);

/// Add new element to the set. If reallocation was unsuccessful, set is guaranteed to stay valid and unchanged.
A4_ERRNO A4_BitSet_Add(A4_BitSet* set, uint32_t elem);

/// Allocate a new chunk of memory on the set with the given alignment.
bool A4_BitSet_Contains(const A4_BitSet* set, uint32_t elem);

#endif /* UUID */
