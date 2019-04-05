#ifndef INC_76A38C430EEE47789CC0015F1B597974
#define INC_76A38C430EEE47789CC0015F1B597974

#include <stdbool.h>
#include <stddef.h>

static inline bool A4_OneOf(const int elem, const int* arr, const size_t arr_size) {
    for (size_t i = 0; i < arr_size; ++i)
        if (arr[i] == elem)
            return true;
    return false;
}

#endif /* UUID */
