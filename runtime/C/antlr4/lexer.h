#pragma once

#include "common.h"
#include "input.h"
#include "token.h"
#include "lexing_error.h"
#include "lexing_result.h"

#include "memory/pool.h"


/**
 * Contains lexer state.
 */
a4_struct(A4_Lexer) {
    /// Memory pool in which all tokens and errors are allocated.
    A4_MemoryPool* pool;

    /// List of tokens emitted so far.
    A4_TokenList* tokens;

    /// List of errors encountered so far.
    A4_LexingErrorList* errors;

    /// Input from which this lexer draws symbols.
    A4_Input input;
};


// Lexer API
// --------------------------------------------------------------------------------------------------------------------

/// Init new lexer without a state.
A4_Lexer A4_Lexer_Init(void);

/// Reset a lexer.
void A4_Lexer_Reset(A4_Lexer* lexer, A4_Input input);
