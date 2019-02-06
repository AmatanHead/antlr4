#pragma once

#include "common.h"

#include <stdint.h>


/**
 * Lexing/parsing error.
 */
a4_struct(A4_Error) {
    /// Line at which the error was generated.
    uint32_t line;

    /// Column at which the error was generated.
    uint32_t column;

    /// If applicable, line at which the errored range ended.
    uint32_t end_line;

    /// If applicable, column at which the errored range ended.
    uint32_t end_column;

    /// Non-owning pointer to the null-terminated error message.
    const char* message;
};
