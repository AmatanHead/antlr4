#include "pool.h"
#include "align.h"

#include <assert.h>
#include <memory.h>
#include <stdalign.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>


// Pool page implementation
// --------------------------------------------------------------------------------------------------------------------

struct A4_MemoryPoolPage {
    /// Non-owning pointer to the first unused byte in the memory chunk.
    char* current;
    /// How many bytes left in the chunk.
    size_t left;
    /// Owning pointer to the next page (used up previously).
    struct A4_MemoryPoolPage* next;
};

/// Take ownership over an old page and allocate a new one. If this function
/// returns NULL, new page is not created and ownership over an old page is not taken.
static inline struct A4_MemoryPoolPage* A4_MemoryPoolPage_New(struct A4_MemoryPoolPage* old, size_t size) {
    struct A4_MemoryPoolPage* page = malloc(sizeof(struct A4_MemoryPoolPage) + size);
    if (!page) return NULL;

    page->current = (char*)(page + 1);
    page->left = size;
    page->next = old;

    return page;
}

/// Free chain of pages.
static inline void A4_MemoryPoolPage_Delete(struct A4_MemoryPoolPage* page) {
    while (page) {
        struct A4_MemoryPoolPage* next = page->next;
        free(page);
        page = next;
    }
}

/// Get total size of the page.
static inline size_t A4_MemoryPoolPage_Size(const struct A4_MemoryPoolPage* page) {
    if (!page)
        return 0;
    else
        return page->current + page->left - (const char*)(page);
}

/// Get number of bytes left on this page.
static inline size_t A4_MemoryPoolPage_Left(const struct A4_MemoryPoolPage* page) {
    if (!page)
        return 0;
    else
        return page->left;
}

/// Try allocate new memory on the page.
static inline void* A4_MemoryPoolPage_Alloc(struct A4_MemoryPoolPage* page, size_t size) {
    if (!page) return NULL;
    if (page->left < size) return NULL;

    char* ret = page->current;

    page->current += size;
    page->left -= size;

    return ret;
}

/// Try allocate new memory on the page.
static inline void* A4_MemoryPoolPage_AllocAligned(struct A4_MemoryPoolPage* page, size_t size, size_t align) {
    if (!page) return NULL;

    size_t pad = A4_AlignUpMem(page->current, align) - page->current;

    void* ret = A4_MemoryPoolPage_Alloc(page, size + pad);

    if (ret) {
        return (char*)(ret) + pad;
    } else {
        return NULL;
    }
}


// Pool implementation
// --------------------------------------------------------------------------------------------------------------------

struct A4_MemoryPool {
    /// Current options.
    struct A4_MemoryPool_Options options;
    /// Current page.
    struct A4_MemoryPoolPage* page;
    /// Size of all pages except for the current one.
    size_t allocated;
    /// Allocated but unused memory on all pages except for the current one.
    size_t waste;
    /// Size of the next block.
    size_t next_page_size;
};

struct A4_MemoryPool* A4_MemoryPool_New(struct A4_MemoryPool_Options options) {
    struct A4_MemoryPool* pool = malloc(sizeof(struct A4_MemoryPool));
    if (!pool) return NULL;

    pool->page = NULL;
    pool->options = options;
    pool->allocated = 0;
    pool->waste = 0;
    pool->next_page_size = options.page_size;

    return pool;
}

void A4_MemoryPool_Delete(struct A4_MemoryPool* pool) {
    if (pool) A4_MemoryPoolPage_Delete(pool->page);
    free(pool);
}

void A4_MemoryPool_Clear(struct A4_MemoryPool* pool) {
    assert(pool);
    A4_MemoryPoolPage_Delete(pool->page);
    pool->page = NULL;
    pool->allocated = 0;
    pool->waste = 0;
    pool->next_page_size = pool->options.page_size;
}

static A4_ERRNO A4_MemoryPool_AddPageFor(struct A4_MemoryPool* pool, size_t size) {
    assert(pool);

    size_t page_size = pool->next_page_size;
    if (page_size < size)
        page_size = size;

    struct A4_MemoryPoolPage* page = A4_MemoryPoolPage_New(pool->page, page_size);
    if (!page) return ENOMEM;

    pool->allocated += A4_MemoryPoolPage_Size(pool->page);
    pool->waste += A4_MemoryPoolPage_Left(pool->page);
    pool->page = page;

    switch (pool->options.growth_strategy) {
        case A4_GS_LINEAR:
            pool->next_page_size = page_size;
            break;
        case A4_GS_EXPONENT:
            pool->next_page_size = page_size * 2;
            break;
    }

    return A4_SUCCESS;
}

size_t A4_MemoryPool_Allocated(const struct A4_MemoryPool* pool) {
    assert(pool);
    return pool->allocated + A4_MemoryPoolPage_Size(pool->page);
}

size_t A4_MemoryPool_Left(const struct A4_MemoryPool* pool) {
    assert(pool);
    return A4_MemoryPoolPage_Left(pool->page);
}

size_t A4_MemoryPool_Waste(const struct A4_MemoryPool* pool) {
    assert(pool);
    return pool->waste + A4_MemoryPoolPage_Left(pool->page);
}

void* A4_MemoryPool_Malloc(struct A4_MemoryPool* pool, size_t size) {
    assert(pool);
    // size = A4_AlignUp(size, alignof(max_align_t));
    void* ret = A4_MemoryPoolPage_Alloc(pool->page, size);
    if (ret) return ret;
    A4_MemoryPool_AddPageFor(pool, size);
    return A4_MemoryPoolPage_Alloc(pool->page, size);
}

void* A4_MemoryPool_MallocAligned(struct A4_MemoryPool* pool, size_t size, size_t align) {
    assert(pool);
    // size = A4_AlignUp(size, alignof(max_align_t));
    void* ret = A4_MemoryPoolPage_AllocAligned(pool->page, size, align);
    if (ret) return ret;
    A4_MemoryPool_AddPageFor(pool, size + align - 1);
    return A4_MemoryPoolPage_AllocAligned(pool->page, size, align);
}
