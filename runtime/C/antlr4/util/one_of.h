#pragma once

#include <stdbool.h>
#include <stddef.h>

static inline bool A4_OneOf(const int elem, const int* arr, const size_t arr_size) {
    for (size_t i = 0; i < arr_size; ++i)
        if (arr[i] == elem)
            return true;
    return false;
}
