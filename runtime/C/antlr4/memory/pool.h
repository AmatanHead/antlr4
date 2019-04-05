#ifndef INC_83C7D418796D4E0F96097C06BCF78347
#define INC_83C7D418796D4E0F96097C06BCF78347

#include "../common.h"

#include <stdalign.h>
#include <stdbool.h>
#include <stdlib.h>


/**
 * Memory pool is used to optimize memory allocation and garbage collection.
 *
 * Note: this pool is not thread safe.
 */
typedef struct A4_MemoryPool A4_MemoryPool;


// Memory pool options
// --------------------------------------------------------------------------------------------------------------------

/// Options for memory pool growth rate.
a4_enum(A4_MemoryPool_GrowthStrategy) {
    /// Each new page is as big as the previous one.
    A4_GS_LINEAR,
    /// Each new page twice as big as the previous one.
    A4_GS_EXPONENT,
};

/// Options for memory pool init.
a4_struct(A4_MemoryPool_Options) {
    /// Size of the first page.
    size_t page_size;
    /// How page size changes over time.
    A4_MemoryPool_GrowthStrategy growth_strategy;
};

/// Construct default options for memory pool init.
inline A4_MemoryPool_Options A4_MemoryPool_Options_Default(void) {
    return (A4_MemoryPool_Options) {
        .page_size = 2048,
        .growth_strategy = A4_GS_EXPONENT
    };
}


// Memory pool API
// --------------------------------------------------------------------------------------------------------------------

/// Allocate and init new memory pool.
A4_MemoryPool* A4_NODISCARD A4_MemoryPool_New(A4_MemoryPool_Options options);
inline A4_MemoryPool* A4_NODISCARD A4_MemoryPool_NewDefault(void) {
    return A4_MemoryPool_New(A4_MemoryPool_Options_Default());
}

/// Delete memory pool and free all its memory.
void A4_MemoryPool_Delete(A4_MemoryPool* pool);

/// Release all memory stored in the pool.
void A4_MemoryPool_Clear(A4_MemoryPool* pool);

/// Get total number of bytes that's currently allocated.
size_t A4_MemoryPool_Allocated(const A4_MemoryPool* pool);

/// Get number of bytes left on the current page.
size_t A4_MemoryPool_Left(const A4_MemoryPool* pool);

/// Get number of bytes that are allocated but unused.
size_t A4_MemoryPool_Waste(const A4_MemoryPool* pool);

/// Allocate a new chunk of unaligned memory on the pool.
void* A4_MemoryPool_Malloc(A4_MemoryPool* pool, size_t size);

/// Allocate a new chunk of memory on the pool with the given alignment.
void* A4_MemoryPool_MallocAligned(A4_MemoryPool* pool, size_t size, size_t align);

/// Allocate object of the given type.
#define A4_POOL_ALLOC(pool, T) (T*)A4_MemoryPool_MallocAligned((pool), sizeof(T), alignof(T))
#define A4_POOL_CALLOC(pool, T, size) (T*)A4_MemoryPool_MallocAligned((pool), sizeof(T) * size, alignof(T))

#endif /* UUID */
