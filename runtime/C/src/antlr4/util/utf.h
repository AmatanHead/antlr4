#ifndef INC_DF058DE6D42A4F5392A0A1342F949CC6
#define INC_DF058DE6D42A4F5392A0A1342F949CC6

#include <antlr4/common.h>

#define A4_UTF8_BROKEN_CHAR 0xFFFD

#define A4_IS_C(x) ((x) & 0xC0u) == 0x80u
#define A4_L(x, l) do { *res = (x) & ("\0\0\037\017\007"[(l)]); } while (0)
#define A4_C(x) do { *res <<= 6u; *res |= (x) & 0x3Fu; } while (0)
#define A4_E() do { *res = A4_UTF8_BROKEN_CHAR; return 0; } while (0)

/// Get length of a UTF8 codepoint by its first byte.
static inline uint8_t A4_CodepointLen(const unsigned char* p, const unsigned char* e) {
    if (p >= e) return 0;
    if ((*p & 0x80u) == 0x00) return 1;
    if ((*p & 0xe0u) == 0xc0) return 2;
    if ((*p & 0xf0u) == 0xe0) return 3;
    if ((*p & 0xf8u) == 0xf0) return 4;
    return 0;
}

/// Get length of a UTF8 codepoint by its first byte. Also check that the entire codepoint is not broken.
static inline uint8_t A4_CheckedCodepointLen(const unsigned char* p, const unsigned char* e) {
    if (p >= e) return 0;
    if ((*p & 0x80u) == 0x00) return 1;
    if ((*p & 0xe0u) == 0xc0) return p + 1 < e && A4_IS_C(p[1]) ? 2 : 0;
    if ((*p & 0xf0u) == 0xe0) return p + 2 < e && A4_IS_C(p[1]) && A4_IS_C(p[2]) ? 3 : 0;
    if ((*p & 0xf8u) == 0xf0) return p + 3 < e && A4_IS_C(p[1]) && A4_IS_C(p[2]) && A4_IS_C(p[3]) ? 4 : 0;
    return 0;
}

/// Read one codepoint. Return length of the codepoint.
static inline uint8_t A4_ReadCodepoint(uint32_t* res, const unsigned char* p, const unsigned char* e) {
    switch (A4_CheckedCodepointLen(p, e)) {
        case 1:
            *res = *p;
            return 1;
        case 2:
            A4_L(p[0], 2);
            A4_C(p[1]);
            if (A4_UNLIKELY(*res < 0x80)) A4_E();
            return 2;
        case 3:
            A4_L(p[0], 3);
            A4_C(p[1]);
            A4_C(p[2]);
            if (A4_UNLIKELY(*res < 0x800)) A4_E();
            return 3;
        case 4:
            A4_L(p[0], 4);
            A4_C(p[1]);
            A4_C(p[2]);
            A4_C(p[3]);
            if (A4_UNLIKELY(*res < 0x10000 || *res > 0x10FFFF)) A4_E();
            return 4;
        default:
            *res = A4_UTF8_BROKEN_CHAR;
            return 0;
    }
}

#undef A4_IS_C
#undef A4_L
#undef A4_C
#undef A4_E

#endif /* UUID */
