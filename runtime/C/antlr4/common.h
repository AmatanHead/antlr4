#pragma once

#include "util/one_of.h"

#include <assert.h>


// Error handling
// --------------------------------------------------------------------------------------------------------------------

/// Indicates success (zero code) or failure (non-zero code) of some procedure.
typedef int A4_Errno;

/// Return code that indicates success.
#define A4_SUCCESS 0


// Compiler-specific macros
// --------------------------------------------------------------------------------------------------------------------

#if defined(__GNUC__)
#   define A4_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#   define A4_UNREACHABLE() __assume(0)
#else
#   define A4_UNREACHABLE() abort()
#endif


// Polymorphism support
// --------------------------------------------------------------------------------------------------------------------

// Here's how we do structural polymorphism in C. For each class hierarchy there's a base structure which contains
// an integer (or an enum element) called `type`. It indicates what kind of instance is actually referenced by
// a pointer. Each derived class must have its base struct stored in the first element. For example:
//
// @code
// struct Base {
//     int type;
//     ... base fields...
// }
//
// struct Derived {
//     struct Base base;
//     ... derived fields...
// }
// @endcode
//
// In this setup, we can cast a pointer to `Base` into a pointer to `Derived` and vice versa by simply
// reinterpret_cast'ing them (because C guarantees that there is no offset between first byte of a struct and
// first byte of the first element of a struct).
//
// To add some safety to this process we assume that each non-abstract derived struct gets its own `type` id. Than,
// before performing a downcast, we check that `type` id of the struct we're about to downcast matches that id
// of the type that we're casting to.
//
// See ATN structs for an example.
//
// Note that we don't perform virtual methods dispatch. Since there is no need for users to subclass any of our
// 'classes', a switch-case based dispatch is fine.

/// Macro for defining safe (well, as safe as you can get in C) function to perform downcasts.
///
/// Note: in C++, downcasts are usually a sign of bad system design. In C, downcasts are normal because there
/// are no virtual methods.
///
/// @param N name of conversion function.
/// @param B base struct type.
/// @param T derived struct type.
/// @param ... list of type ids that can be stored in the derived struct type.
#define A4_DOWNCAST_FUNC(N, B, T, ...)                                                                              \
static inline struct T* N(struct B* ptr) {                                                                          \
    if (ptr) assert(A4_OneOf(ptr->type, (int[]){ __VA_ARGS__ }, sizeof((int[]){ __VA_ARGS__ }) / sizeof(int)));     \
    return (struct T*)(ptr);                                                                                        \
}
