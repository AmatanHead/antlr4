#include "uuid.h"

#include <printf.h>


#define READ_DIGIT                                                                                                    \
    if ('0' <= data[i] && data[i] <= '9') {                                                                           \
        tmp = (uint8_t) (data[i] - '0');                                                                              \
    } else if ('a' <= data[i] && data[i] <= 'f') {                                                                    \
        tmp = (uint8_t) (data[i] - 'a' + 10);                                                                         \
    } else if ('A' <= data[i] && data[i] <= 'F') {                                                                    \
        tmp = (uint8_t) (data[i] - 'A' + 10);                                                                         \
    } else {                                                                                                          \
        return (A4_UUID) { .data = {0, 0, 0, 0} };                                                                    \
    }                                                                                                                 \
                                                                                                                      \
    uuid_value = uuid_value * 16 + tmp;                                                                               \
                                                                                                                      \
    i += 1;                                                                                                           \
    j += 1;                                                                                                           \
                                                                                                                      \
    if (j == 8) {                                                                                                     \
        res.data[uuid_index++] = uuid_value;                                                                          \
        uuid_value = 0;                                                                                               \
        j = 0;                                                                                                        \
    }                                                                                                                 \

#define READ_DASH                                                                                                     \
    if (data[i++] != '-') return (A4_UUID) { .data = {0, 0, 0, 0} };


A4_UUID A4_UUID_Read(const char data[36]) {
    A4_UUID res;

    size_t uuid_index = 0;
    uint32_t uuid_value = 0;
    uint8_t tmp = 0;
    size_t i = 0;
    size_t j = 0;

    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT

    READ_DASH

    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT

    READ_DASH

    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT

    READ_DASH

    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT

    READ_DASH

    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT
    READ_DIGIT

    return res;
}

A4_UUID A4_UUID_ReadBinary(const uint16_t* bytes) {
    A4_UUID res;

    size_t uuid_index = 0;
    uint32_t uuid_value = 0;

    for (size_t i = 0; i < 8; ++i) {
        uuid_value = (uuid_value << 16) + bytes[7 - i];
        if (i & 1) {
            res.data[uuid_index++] = uuid_value;
            uuid_value = 0;
        }
    }

    return res;
}
