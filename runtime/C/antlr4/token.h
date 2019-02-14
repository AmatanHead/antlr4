#pragma once

#include "common.h"


#define A4_TOKEN_INVALID 0
#define A4_MIN_USER_TOKEN_TYPE 1
#define A4_TOKEN_EOF 0xFFFF
#define A4_TOKEN_EPS 0xFFFE

#define A4_CHANNEL_DEFAULT 0
#define A4_CHANNEL_HIDDEN 1
#define A4_MIN_USER_CHANNEL_VALUE 2


/**
 * Determines how `line` and `column` are calculated.
 *
 * In normal mode, they are 1-based numbers that indicate number of line and number of codepoint in that line.
 * Normal mode is used for displaying error messages in console.
 *
 * In LSP mode, they are 0-based numbers that indicate index of line and index of the first byte of a codepoint
 * in a UTF-16 representation of the line. This mode is primarily used for LSP servers.
 *
 * For example, the letter `b` in the file `a𐐀𐐀b` has coordinates:
 *
 * - `1:4` in normal mode because it's located on the first line and it's the fourth codepoint in its line.
 * - `0:5` in LSP mode because it's line index is zero and in UTF-16 representation its codepoint start index is five.
 *   That is, `a` takes one byte and `𐐀`s takes four more bytes.
 */
a4_enum(A4_LCMode) {
    /// Normal line-column mode.
    A4_LC_NORMAL,

    /// LSP line-column mode.
    A4_LC_LSP,
};


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
