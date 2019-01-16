#pragma once

#include "../antlr4.h"

#include <stdbool.h>
#include <stdlib.h>


/**
 * Note: this pool is not thread safe.
 */
struct A4_MemoryPool;


// Memory pool options
// --------------------------------------------------------------------------------------------------------------------

/// Options for memory pool growth rate.
enum A4_MemoryPool_GrowthStrategy {
    /// Each new page is as big as the previous one.
    A4_GS_LINEAR,
    /// Each new page twice as big as the previous one.
    A4_GS_EXPONENT,
};

/// Options for memory pool init.
struct A4_MemoryPool_Options {
    /// Size of the first page.
    size_t page_size;
    /// How page size changes over time.
    enum A4_MemoryPool_GrowthStrategy growth_strategy;
};

/// Construct default options for memory pool init.
inline struct A4_MemoryPool_Options A4_MemoryPool_Options_Default() {
    return (struct A4_MemoryPool_Options) {
        .page_size = 2048,
        .growth_strategy = A4_GS_EXPONENT
    };
}


// Memory pool API
// --------------------------------------------------------------------------------------------------------------------

/// Allocate and init new memory pool.
struct A4_MemoryPool* A4_MemoryPool_New(struct A4_MemoryPool_Options options);
inline struct A4_MemoryPool* A4_MemoryPool_NewDefault() { return A4_MemoryPool_New(A4_MemoryPool_Options_Default()); }

/// Delete memory pool and free all its memory.
void A4_MemoryPool_Delete(struct A4_MemoryPool* pool);

/// Release all memory stored in the pool.
void A4_MemoryPool_Clear(struct A4_MemoryPool* pool);

/// Get total number of bytes that's currently allocated.
size_t A4_MemoryPool_Allocated(const struct A4_MemoryPool* pool);

/// Get number of bytes left on the current page.
size_t A4_MemoryPool_Left(const struct A4_MemoryPool* pool);

/// Get number of bytes that are allocated but unused.
size_t A4_MemoryPool_Waste(const struct A4_MemoryPool* pool);

/// Allocate a new chunk of unaligned memory on the pool.
void* A4_MemoryPool_Malloc(struct A4_MemoryPool* pool, size_t size);

/// Allocate a new chunk of memory on the pool with the given alignment.
void* A4_MemoryPool_MallocAligned(struct A4_MemoryPool* pool, size_t size, size_t align);

/// Allocate object of the given type.
#define A4_POOL_ALLOC(pool, T) (T*)A4_MemoryPool_MallocAligned((pool), sizeof(T), alignof(T));

