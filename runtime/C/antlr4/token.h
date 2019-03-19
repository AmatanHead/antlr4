#ifndef _1B5A2432_DDC5_4503_8DC7_C8894DFF79AD
#define _1B5A2432_DDC5_4503_8DC7_C8894DFF79AD

#include "common.h"


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

    /// Line at which the first character of this token was encountered. See `A4_LCMode` for more info.
    uint32_t line;

    /// Column at which the first character of this token was encountered. See `A4_LCMode` for more info.
    uint32_t column;

    /// Line at which the last character of this token was encountered. See `A4_LCMode` for more info.
    uint32_t end_line;

    /// Column at which the last character of this token was encountered. See `A4_LCMode` for more info.
    uint32_t end_column;
};


// Constants
// --------------------------------------------------------------------------------------------------------------------

#define A4_TOKEN_INVALID 0
#define A4_MIN_USER_TOKEN_TYPE 1
#define A4_TOKEN_EOF 0xFFFF
#define A4_TOKEN_EPS 0xFFFE
#define A4_TOKEN_NO_EMIT 0xFFFD

#define A4_CHANNEL_DEFAULT 0
#define A4_CHANNEL_HIDDEN 1
#define A4_MIN_USER_CHANNEL_VALUE 2


// List of token pointers
// --------------------------------------------------------------------------------------------------------------------

#define A4_LIST_NAME A4_TokenList
#define A4_LIST_PAYLOAD A4_Token*
#define A4_LIST_NO_DTOR
#include "util/list.inl"

#endif /* UUID */
