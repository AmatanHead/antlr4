#ifndef INC_2610B80E557044F5AE7E6D50EF5CC6AB
#define INC_2610B80E557044F5AE7E6D50EF5CC6AB

#include <assert.h>
#include <stddef.h>


static inline size_t A4_AlignUp(size_t size, size_t align) {
    return size + (align - 1) & ~(align - 1);
}

static inline char* A4_AlignUpMem(char* mem, size_t align) {
    return (char*)A4_AlignUp((size_t)mem, align);
}

#endif /* UUID */
