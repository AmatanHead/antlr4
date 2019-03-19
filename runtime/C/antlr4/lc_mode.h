#ifndef _1A02C8EC_98AC_47A9_9BC8_5DBEF9F77B67
#define _1A02C8EC_98AC_47A9_9BC8_5DBEF9F77B67

#include "common.h"


/**
 * Determines how lexer calculates `line` and `column` properties of a token.
 *
 * In normal mode, they are 1-based numbers that indicate number of line and number of codepoint in that line.
 * Normal mode is used for displaying error messages in console.
 *
 * In LSP mode, they are 0-based numbers that indicate index of line and index of the first byte of a codepoint
 * in a UTF-16 representation of the line. This mode is primarily used for LSP implementations.
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

#endif /* UUID */
