#ifndef INC_52323810DD6943B5A32D3494EE2EB26B
#define INC_52323810DD6943B5A32D3494EE2EB26B

#include "common.h"
#include "token.h"
#include "lexing_error.h"


/**
 * Lexer invocation result.
 */
a4_struct(A4_LexingResult) {
    /// Array of tokens.
    struct {
        A4_Token const* const* data;
        size_t size;
    } tokens;

    /// Array of errors. If empty, lexing finished successfully.
    struct {
        A4_LexingError const* const* data;
        size_t size;
    } errors;
};

#endif /* UUID */
