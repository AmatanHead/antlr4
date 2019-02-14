#pragma once

#include "common.h"

#include "util/utf.h"


/**
 * A UTF8 string reader with lookahead and seek operations.
 */
a4_struct(A4_Input) {
    const unsigned char* beg;
    const unsigned char* end;
    const unsigned char* cur;
    uint32_t cur_codepoint;
};


// Reader API
// --------------------------------------------------------------------------------------------------------------------

/// Init new reader
static inline A4_ERRNO A4_Input_Init(A4_Input* res, const char* begin, const char* end);

/// Get index of the current symbol.
static inline uint32_t A4_Input_Index(const A4_Input* input);

/// Jump to a specific index.
static inline A4_ERRNO A4_Input_Seek(A4_Input* input, uint32_t index);

/// Get current symbol. Returns zero if hit EOF.
static inline uint32_t A4_Input_Current(const A4_Input* input);

/// Lookahead one symbol after the current one. Returns zero if hit EOF.
static inline uint32_t A4_Input_Next(const A4_Input* input);

/// Advance one symbol forward.
static inline A4_ERRNO A4_Input_Advance(A4_Input* input);


// Reader implementation
// --------------------------------------------------------------------------------------------------------------------

static inline A4_ERRNO A4_Input_Init(A4_Input* res, const char* begin, const char* end) {
    assert(res);
    assert(begin);
    assert(end);

    *res = (A4_Input) {
        .beg = (const unsigned char*) begin,
        .end = (const unsigned char*) end,
        .cur = (const unsigned char*) end,
        .cur_codepoint = 0,
    };

    if (end - begin > 0XFFFFFFFEu) return A4_E_INPUT_TOO_LARGE;  // we use 32-bit integers as indices.

    return A4_Input_Seek(res, 0);
}

static inline uint32_t A4_Input_Index(const A4_Input* input) {
    assert(input);

    return (uint32_t)(input->cur - input->beg);
}

static inline A4_ERRNO A4_Input_Seek(A4_Input* input, uint32_t index) {
    assert(input);

    input->cur = input->beg + index;

    assert(input->cur <= input->end);
    assert(input->cur >= input->beg);

    if (input->cur == input->end) {
        input->cur_codepoint = 0;
    } else {
        uint8_t len;
        if (!(len = A4_ReadCodepoint(&input->cur_codepoint, input->cur, input->end))) {
            return A4_E_INVALID_ENCODING;
        }
        input->cur += len;
    }

    return A4_SUCCESS;
}

static inline uint32_t A4_Input_Current(const A4_Input* input) {
    assert(input);

    return input->cur_codepoint;
}

static inline uint32_t A4_Input_Next(const A4_Input* input) {
    assert(input);

    if (input->cur == input->end) return 0;

    uint32_t res;
    A4_ReadCodepoint(&res, input->cur, input->end);
    return res;
}

static inline A4_ERRNO A4_Input_Advance(A4_Input* input) {
    assert(input);

    uint8_t len;
    if (!(len = A4_ReadCodepoint(&input->cur_codepoint, input->cur, input->end))) {
        return A4_E_INVALID_ENCODING;
    }
    input->cur += len;

    return A4_SUCCESS;
}
