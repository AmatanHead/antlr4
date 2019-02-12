#pragma once

#include "util/one_of.h"

#include <assert.h>
#include <stdint.h>


// Error handling
// --------------------------------------------------------------------------------------------------------------------

/// Indicates success (zero code) or failure (non-zero code) of some procedure.
typedef int A4_Errno;

/// Should be used as function result type to enforce error checking.
#define A4_ERRNO A4_Errno A4_NODISCARD

#define A4_SUCCESS              0               ///< Return code that indicates success.

#define A4_E_SYSTEM             1000            ///< System errors.
#define A4_E_MEMORY             1001            ///< Memory allocation failure.
#define A4_E_FORMAT             1002            ///< Printf formatting error.

#define A4_E_INPUT              2000            ///< Input read and encoding/decoding errors.
#define A4_E_INVALID_ENCODING   2001            ///< UTF decoding error. Input string is invalid.
#define A4_E_INPUT_TOO_LARGE    2002            ///< Input string is too large.

#define A4_E_ATN                3000            ///< ATN deserialization errors.
#define A4_E_BC_INVALID         3001            ///< Invalid bytecode.
#define A4_E_BC_NOT_SUPPORTED   3002            ///< Unsupported bytecode version.
#define A4_E_ATN_INVALID        3003            ///< ATN correctness check failed.


// Compiler-specific macros
// --------------------------------------------------------------------------------------------------------------------

#if defined(__GNUC__)
#   define A4_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#   define A4_UNREACHABLE() __assume(0)
#else
#   define A4_UNREACHABLE() abort()
#endif

#if defined(__GNUC__)
#    define A4_NODISCARD __attribute__((warn_unused_result))
#elif  defined(_MSC_VER)
#    define A4_NODISCARD _Check_return_
#else
#    define A4_NODISCARD
#endif

#if defined(__GNUC__)
#   define A4_LIKELY(c) __builtin_expect(!!(c), 1)
#   define A4_UNLIKELY(c) __builtin_expect(!!(c), 0)
#else
#   define A4_LIKELY(c)
#   define A4_UNLIKELY(c)
#endif


// Typing support
// --------------------------------------------------------------------------------------------------------------------

// Normally I try to avoid typedefs as mush as possible, however, this project is a little bit different.
// I want to keep headers of this library C++-compatible so that it can eventually replace (or complement)
// the C++ runtime. To do so I need to ensure that there are no enum predeclarations and all enums have proper type
// specified (i.e. `enum : int`). So I'm forcing myself to include all necessary headers by using typedef'd enums
// and structs. As a bonus, typedefs add protection against misspells.

/// Helpers to avoid hell when defining structs, enums and unions. Add proper typedefs, keep enums C++-compatible.
/// @{
#define a4_struct(name) typedef struct name name; struct name
#define a4_union(name) typedef union name name; union name
#ifdef __cplusplus
#define a4_enum(name) enum name : int
#else
#define a4_enum(name) typedef enum name name; enum name
#endif
/// @}


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
    static inline bool A4_Is##N(struct B* ptr) {                                                                    \
        return A4_OneOf((int)ptr->type, (int[]){ __VA_ARGS__ }, sizeof((int[]){ __VA_ARGS__ }) / sizeof(int));      \
    }                                                                                                               \
    static inline struct T* A4_To##N(struct B* ptr) {                                                               \
        if (ptr) assert(A4_Is##N(ptr));                                                                             \
        return (struct T*)(ptr);                                                                                    \
    }


// Preprocessor stuff
// --------------------------------------------------------------------------------------------------------------------

#define A4_CAT(X, Y) __A4_CAT_1(X, Y)
#define __A4_CAT_1(X, Y) __A4_CAT_2(X, Y)
#define __A4_CAT_2(X, Y) X##Y
