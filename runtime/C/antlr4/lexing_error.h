#ifndef _2DD0212A_0C79_4174_BE1F_D3EFFF674772
#define _2DD0212A_0C79_4174_BE1F_D3EFFF674772

#include "common.h"

#include <stdint.h>


/**
 * Lexing error.
 */
a4_struct(A4_LexingError) {
    /// Line at which the error occurred.
    uint32_t line;

    /// Column at which the error occurred.
    uint32_t column;

    /// If applicable, line at which the errored range ended.
    uint32_t end_line;

    /// If applicable, column at which the errored range ended.
    uint32_t end_column;

    /// Non-owning pointer to the null-terminated error message.
    const char* message;
};


// List of error pointers
// --------------------------------------------------------------------------------------------------------------------

#define A4_LIST_NAME A4_LexingErrorList
#define A4_LIST_PAYLOAD A4_LexingError*
#define A4_LIST_NO_DTOR
#include "util/list.inl"

#endif /* UUID */
