#pragma once

#include <stdint.h>
#include "../common.h"


/**
 * Basic support for UUIDs. For this library it's enough to parse them and to compare them.
 */
a4_struct(A4_UUID) {
    union {
        uint32_t data[4];
        struct {
            uint32_t time_low;
            uint16_t time_mid;
            uint8_t  time_hi_and_version;
            uint8_t  clock_seq_hi_and_reserved;
            uint8_t  clock_seq_low;
            uint8_t  node[6];
        };
    };
};


// UUID API
// --------------------------------------------------------------------------------------------------------------------

/// Make zero UUID.
static inline A4_UUID A4_UUID_Zero(void) {
    return (A4_UUID) { .data = {0, 0, 0, 0} };
}

/// Read UUID from string. If the string is malformed, returns zero UUID.
A4_UUID A4_UUID_Read(const char data[32]);

/// Read UUID from bytecode.
A4_UUID A4_UUID_ReadBinary(const uint16_t bytes[8]);

/// Compares two UUIDs.
static inline bool A4_UUID_Eq(const A4_UUID a, const A4_UUID b) {
    return a.data[0] == b.data[0] && a.data[1] == b.data[1] && a.data[2] == b.data[2] && a.data[3] == b.data[3];
}

/// Checks if UUID is zero.
static inline bool A4_UUID_IsZero(const A4_UUID uuid) {
    return (uuid.data[0] | uuid.data[1] | uuid.data[2] | uuid.data[3]) == 0;
}
