#pragma once

#ifdef __cplusplus
#   define HEADER_BEGIN extern "C" {
#   define HEADER_END }
#else
#   define HEADER_BEGIN
#   define HEADER_END
#endif

HEADER_BEGIN

// Error handling
// --------------------------------------------------------------------------------------------------------------------

/// Indicates success (zero code) or failure (non-zero code) of some procedure.
typedef int A4_Errno;

/// Return code that indicates success.
#define A4_SUCCESS 0

HEADER_END
