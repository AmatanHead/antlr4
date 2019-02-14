// Generated from <file> by ANTLR 4.7
// DO NOT EDIT -- your changes will be overwritten!

#pragma once

#include <antlr4/antlr4.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct A4_My A4_My;


/// Token IDs.
a4_enum(My_Tok) {
    My_Tok_BlockComment = 1,
    My_Tok_DocComment = 2,
    My_Tok_Import = 3,
    My_Tok_From = 4,
    My_Tok_As = 5,
    My_Tok_Export = 6,
    My_Tok_Def = 7,
    My_Tok_Let = 8,
    My_Tok_Struct = 9,
    My_Tok_Enum = 10,
    My_Tok_Variant = 11,
    My_Tok_Type = 12,
    My_Tok_Newtype = 13,
    My_Tok_Narrowtype = 14,
    My_Tok_Transform = 15,
    My_Tok_Extends = 16,
    My_Tok_Based = 17,
    My_Tok_On = 18,
    My_Tok_Private = 19,
    My_Tok_Public = 20,
    My_Tok_Proteted = 21,
    My_Tok_If = 22,
    My_Tok_Else = 23,
    My_Tok_And = 24,
    My_Tok_Or = 25,
    My_Tok_Not = 26,
    My_Tok_Match = 27,
    My_Tok_Case = 28,
    My_Tok_LParen = 29,
    My_Tok_LBrack = 30,
    My_Tok_LBrace = 31,
    My_Tok_RParen = 32,
    My_Tok_RBrack = 33,
    My_Tok_RBrace = 34,
    My_Tok_Equal = 35,
    My_Tok_Comma = 36,
    My_Tok_Dot = 37,
    My_Tok_DoubleDot = 38,
    My_Tok_TripleDot = 39,
    My_Tok_Colon = 40,
    My_Tok_DoubleColon = 41,
    My_Tok_Semicolon = 42,
    My_Tok_Hash = 43,
    My_Tok_Question = 44,
    My_Tok_Exclamation = 45,
    My_Tok_Tilde = 46,
    My_Tok_At = 47,
    My_Tok_Plus = 48,
    My_Tok_Minus = 49,
    My_Tok_Star = 50,
    My_Tok_Slash = 51,
    My_Tok_Percent = 52,
    My_Tok_Ampersand = 53,
    My_Tok_Pipe = 54,
    My_Tok_Caret = 55,
    My_Tok_DoubleAmpersand = 56,
    My_Tok_DoublePipe = 57,
    My_Tok_DoubleEqual = 58,
    My_Tok_ExclamationEqual = 59,
    My_Tok_Greater = 60,
    My_Tok_Lesser = 61,
    My_Tok_GreaterEqual = 62,
    My_Tok_LesserEqual = 63,
    My_Tok_GreaterGreater = 64,
    My_Tok_LesserLesser = 65,
    My_Tok_LArrow = 66,
    My_Tok_RArrow = 67,
    My_Tok_RFatArrow = 68,
    My_Tok_Bind = 69,
    My_Tok_Backslash = 70,
    My_Tok_AnntationStart = 71,
    My_Tok_BoolLiteral = 72,
    My_Tok_NullLiteral = 73,
    My_Tok_IntLiteral = 74,
    My_Tok_FloatLiteral = 75,
    My_Tok_StringLiteral = 76,
    My_Tok_Id = 77,
    My_Tok_LineComment = 78,
    My_Tok_Whitespace = 79,
    My_Tok_Newline = 80,
    My_Tok_LexerError = 81,
    My_Tok_NestedBlockCommentEnd = 82,
    My_Tok_NestedBlockCommentUnterminated = 83,
    My_Tok_BlockCommentStart = 84
};

/// Channel IDs.
a4_enum(My_Ch) {
    My_Ch_COMMENT = 2,
    My_Ch_WHITESPACE = 3
};

/// Mode IDs.
a4_enum(My_M) {
    My_M_BlockCommentMode = 1
};


/// Convert token ID to token name.
const char* My_TokToName(My_Tok tok);

/// Convert channel ID to channel name.
const char* My_ChToName(My_Ch tok);

/// Convert mode ID to mode name.
const char* My_MToName(My_M tok);


/// Create a new instance of lexer.
A4_ERRNO My_New(A4_My** result);

/// Destroy an instance of lexer.
void My_Delete(A4_My* lexer);

/// Perform lexical analysis.
///
/// @param pool memory pool in which all tokens and errors will be allocated.
/// @param lexer lexer instance which will be used to parse the input.
/// @param begin pointer to the first character of the input string.
/// @param end pointer to the past-the-last character of the input string.
/// @param result
/// @return error code if something went wrong, `A4_SUCCESS` if all correct. Note that `A4_SUCCESS` means that
/// lexer finished its logic successfully, but it does not mean that the input was valid.
/// Check `A4_LexingResult`'s `errors` array to check input validity.
A4_ERRNO My_Run(A4_MemoryPool* pool, A4_My* lexer, const char* begin, const char* end, A4_LexingResult* result);


#ifdef __cplusplus
}
#endif
