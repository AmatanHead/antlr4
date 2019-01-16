#pragma once


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
