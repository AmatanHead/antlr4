#pragma once

#include "common.h"

#include <stdint.h>


#define A4_TOKEN_INVALID 0
#define A4_MIN_USER_TOKEN_TYPE 1
#define A4_TOKEN_EOF 0xFFFF
#define A4_TOKEN_EPS 0xFFFE

#define A4_CHANNEL_DEFAULT 0
#define A4_CHANNEL_HIDDEN 1
#define A4_MIN_USER_CHANNEL_VALUE 2


/**
 * Token is a single entity which can be emitted by tokenizer and processed by parser.
 */
a4_struct(A4_Token) {
    /// Index of this token in the token stream.
    uint32_t index;

    /// Type id.
    uint16_t type;

    /// Channel id.
    uint16_t channel;

    /// Non-owning pointer to the first character of token's content.
    const char* begin;  // TODO: maybe use indexes instead?

    /// Non-owning pointer to the past-the-end character of token's content.
    const char* end;  // TODO: maybe use indexes instead?

    /// Line at which the first character of this token was encountered.
    /// Note that line at which this token ends is equal to line at which the next token starts.
    uint32_t line;

    /// Column at which the first character of this token was encountered.
    /// Note that column at which this token ends is equal to column at which the next token starts minus one column.
    uint32_t column;
};
