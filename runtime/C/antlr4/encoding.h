#pragma once

#include "common.h"

/**
 * List of all input encodings supported by Antlr4 C runtime.
 */
a4_enum(A4_Encoding) {
    A4_E_UTF8,
    A4_E_UTF16,
    A4_E_UTF32,
};
