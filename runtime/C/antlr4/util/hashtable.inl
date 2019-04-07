/*
 * This code is based on khtable by Attractive Chaos <attractor@live.co.uk>.
 * This code is is distributed under the MIT license. See LICENSE.txt.
 */
/**
 * Generic hash map/set. Including this file generates a struct and bunch of API methods based on special macro values.
 *
 * Usage:
 *
 * 1. define some macros:
 *   - A4_HT_NAME -- name of the hashtable class;
 *   - A4_HT_KEY -- type of the key;
 *   - A4_HT_VALUE -- type of the value. Alternatively, define A4_HT_NO_VALUE;
 *   - A4_HT_KEY_DTOR and A4_HT_KEY_MOVE -- function to be called whenever a key gets destroyed and a function
 *     called whenever a key needs to be replaced with a new key.
 *     Alternatively, define A4_HT_KEY_NO_DTOR if payload does not require destruction.
 *   - A4_HT_VALUE_DTOR and A4_HT_VALUE_MOVE -- function to be called whenever a value gets destroyed and a function
 *     called whenever a value needs to be replaced with a value key.
 *     Alternatively, define A4_HT_VALUE_NO_DTOR if payload does not require destruction.
 *     This is not required if A4_HT_NO_VALUE is defined.
 *   - A4_HT_KEY_HASH -- function `uint64_t(A4_HT_KEY)` to hash a key.
 *   - A4_HT_KEY_EQ -- function `bool(A4_HT_KEY, A4_HT_KEY)` to compare keys.
 * 2. include `hashtable.inl`;
 * 3. done.
 *
 * For example:
 *
 * @code
 * #define A4_HT_NAME IntToStrMap
 * #define A4_HT_KEY int
 * #define A4_HT_VALUE char*
 * #define A4_HT_KEY_NO_DTOR
 * #define A4_HT_VALUE_DTOR free
 * #define A4_HT_VALUE_MOVE(where, what) if ((where) != what) { free((where)); (where) = what; }
 * #define A4_HT_KEY_HASH(x) (x)
 * #define A4_HT_KEY_EQ(x, y) ((x) == (y))
 * #include <antlr4/util/hashtable.inl>
 * @endcode
 */

#ifndef A4_HT_NAME
#    error "A4_HT_NAME must be defined prior to hashtable.inl inclusion"
#    define A4_HT_NAME UnnamedHt
#endif

#ifndef A4_HT_KEY
#    error "A4_HT_KEY must be defined prior to hashtable.inl inclusion"
#    define A4_HT_KEY void*
#endif

#if defined(A4_HT_VALUE) && defined(A4_HT_NO_VALUE)
#    error "only one of A4_HT_VALUE and A4_HT_NO_VALUE may be defined at the same time"
#elif !defined(A4_HT_VALUE) && !defined(A4_HT_NO_VALUE)
#    error "one one of A4_HT_VALUE or A4_HT_NO_VALUE must be defined prior to hashtable.inl inclusion"
#    define A4_HT_VALUE void*
#endif

#if defined(A4_HT_KEY_DTOR) && defined(A4_HT_KEY_NO_DTOR)
#    error "only one of A4_HT_KEY_DTOR and A4_HT_KEY_NO_DTOR may be defined at the same time"
#endif
#ifndef A4_HT_KEY_DTOR
#    if !defined(A4_HT_KEY_NO_DTOR)
#        error "A4_HT_KEY_DTOR or A4_HT_KEY_NO_DTOR must be defined prior to hashtable.inl inclusion"
#    endif
#    define A4_HT_KEY_DTOR(x) (void)(x)
#endif

#if defined(A4_HT_KEY_MOVE) && defined(A4_HT_KEY_NO_DTOR)
#    error "only one of A4_HT_KEY_MOVE and A4_HT_KEY_NO_DTOR may be defined at the same time"
#endif
#ifndef A4_HT_KEY_MOVE
#    if !defined(A4_HT_KEY_NO_DTOR)
#        error "A4_HT_KEY_MOVE or A4_HT_KEY_NO_DTOR must be defined prior to hashtable.inl inclusion"
#    endif
#    define A4_HT_KEY_MOVE(where, what) ((where) = (what))
#endif

#if defined(A4_HT_VALUE_DTOR) && defined(A4_HT_VALUE_NO_DTOR)
#    error "only one of A4_HT_VALUE_DTOR and A4_HT_VALUE_NO_DTOR may be defined at the same time"
#endif
#if defined(A4_HT_VALUE_DTOR) && defined(A4_HT_NO_VALUE)
#    error "only one of A4_HT_VALUE_DTOR and A4_HT_NO_VALUE may be defined at the same time"
#endif
#ifndef A4_HT_VALUE_DTOR
#    if !defined(A4_HT_VALUE_NO_DTOR) && !defined(A4_HT_NO_VALUE)
#        error "A4_HT_VALUE_DTOR or A4_HT_VALUE_NO_DTOR must be defined prior to hashtable.inl inclusion"
#    endif
#    define A4_HT_VALUE_DTOR(x) (void)(x)
#endif

#if defined(A4_HT_VALUE_MOVE) && defined(A4_HT_VALUE_NO_DTOR)
#    error "only one of A4_HT_VALUE_MOVE and A4_HT_VALUE_NO_DTOR may be defined at the same time"
#endif
#if defined(A4_HT_VALUE_MOVE) && defined(A4_HT_NO_VALUE)
#    error "only one of A4_HT_VALUE_MOVE and A4_HT_NO_VALUE may be defined at the same time"
#endif
#ifndef A4_HT_VALUE_MOVE
#    if !defined(A4_HT_VALUE_NO_DTOR) && !defined(A4_HT_NO_VALUE)
#        error "A4_HT_VALUE_MOVE or A4_HT_VALUE_NO_DTOR must be defined prior to hashtable.inl inclusion"
#    endif
#    define A4_HT_VALUE_MOVE(where, what) ((where) = (what))
#endif

#ifndef A4_HT_KEY_HASH
#    error "A4_HT_KEY_HASH must be defined prior to hashtable.inl inclusion"
#    define A4_HT_KEY_HASH(x) (uint64_t)0
#endif

#ifndef A4_HT_KEY_EQ
#    error "A4_HT_KEY_EQ must be defined prior to hashtable.inl inclusion"
#    define A4_HT_KEY_EQ(x, y) ((x) == (y))
#endif

#include "../common.h"
#include "../memory/pool.h"

#include <stddef.h>
#include <memory.h>


#define A4_HT_POP_RESULT A4_CAT(A4_HT_NAME, _PopResult)
#define A4_HT_NEW A4_CAT(A4_HT_NAME, _New)
#define A4_HT_DELETE A4_CAT(A4_HT_NAME, _Delete)
#define A4_HT_SIZE A4_CAT(A4_HT_NAME, _Size)
#define A4_HT_EMPTY A4_CAT(A4_HT_NAME, _Empty)
#define A4_HT_CALCULATE_IDX A4_CAT(A4_HT_NAME, _CalculateIdx)
#define A4_HT_CALCULATE_IDX_HINT_HASH A4_CAT(A4_HT_NAME, _CalculateIdxHintHash)
#define A4_HT_INSERT A4_CAT(A4_HT_NAME, _Insert)
#define A4_HT_INSERT_HINT_HASH A4_CAT(A4_HT_NAME, _InsertHintHash)
#define A4_HT_RESIZE A4_CAT(A4_HT_NAME, _Resize)
#define A4_HT_CONTAINS A4_CAT(A4_HT_NAME, _Contains)
#define A4_HT_CONTAINS_HINT_HASH A4_CAT(A4_HT_NAME, _ContainsHintHash)
#define A4_HT_CONTAINS_HINT_IDX A4_CAT(A4_HT_NAME, _ContainsHintIdx)
#define A4_HT_GET A4_CAT(A4_HT_NAME, _Get)
#define A4_HT_GET_HINT_HASH A4_CAT(A4_HT_NAME, _GetHintHash)
#define A4_HT_GET_HINT_IDX A4_CAT(A4_HT_NAME, _GetHintIdx)
#define A4_HT_SAFE_GET A4_CAT(A4_HT_NAME, _SafeGet)
#define A4_HT_SAFE_GET_HINT_HASH A4_CAT(A4_HT_NAME, _SafeGetHintHash)
#define A4_HT_SAFE_GET_HINT_IDX A4_CAT(A4_HT_NAME, _SafeGetIdxHash)
#define A4_HT_POP A4_CAT(A4_HT_NAME, _Pop)
#define A4_HT_POP_HINT_HASH A4_CAT(A4_HT_NAME, _PopHintHash)
#define A4_HT_POP_HINT_IDX A4_CAT(A4_HT_NAME, _PopHintIdx)
#define A4_HT_REMOVE A4_CAT(A4_HT_NAME, _Remove)
#define A4_HT_REMOVE_HINT_HASH A4_CAT(A4_HT_NAME, _RemoveHintHash)
#define A4_HT_REMOVE_HINT_IDX A4_CAT(A4_HT_NAME, _RemoveHintIdx)
#define A4_HT_CLEAR A4_CAT(A4_HT_NAME, _Clear)


// Hashtable API
// --------------------------------------------------------------------------------------------------------------------

/// Hashtable type.
/// Contents of this struct shall only be changed via API methods below.
a4_struct(A4_HT_NAME) {
    size_t size;

    size_t n_buckets;
    size_t n_occupied;
    size_t upper_bound;

    uint32_t* flags;
    A4_HT_KEY* keys;
#ifndef A4_HT_NO_VALUE
    A4_HT_VALUE* values;
#endif
};

/// Result of the `pop` operation.
#ifndef A4_HT_NO_VALUE
a4_struct(A4_HT_POP_RESULT) {
    A4_HT_KEY key;
    A4_HT_VALUE value;
};
#else
typedef A4_HT_KEY A4_HT_POP_RESULT;
#endif

/// Create new empty hashtable.
static inline A4_HT_NAME* A4_NODISCARD A4_HT_NEW(void);

/// Delete hashtable and all its contents.
static inline void A4_HT_DELETE(A4_HT_NAME* ht);

/// Get number of elements in the hashtable.
static inline size_t A4_HT_SIZE(const A4_HT_NAME* ht);

/// Check if the given hashtable is empty.
static inline bool A4_HT_EMPTY(const A4_HT_NAME* ht);

/// Calculate bucket index for the given key. The bucket index can be used to hint functions like `insert`,
/// `contains` etc. Note that bucket index becomes invalid after any modificative operation. Using an invalid
/// bucket index is undefined behaviour.
/// @{
static inline size_t A4_HT_CALCULATE_IDX(const A4_HT_NAME* ht, A4_HT_KEY key);
static inline size_t A4_HT_CALCULATE_IDX_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash);
/// @}

/// Take ownership over some key and some value and insert it to the hashtable.
/// If insertion does not succeed, ownership is not taken.
/// If there is an entry with an equal key
/// already stored in the table, this entry is destroyed and new entry is stored instead.
/// @{
#ifdef A4_HT_NO_VALUE
static inline A4_ERRNO A4_HT_INSERT(A4_HT_NAME* ht, A4_HT_KEY key, size_t* idx);
static A4_ERRNO A4_HT_INSERT_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, size_t* idx);
#else
static inline A4_ERRNO A4_HT_INSERT(A4_HT_NAME* ht, A4_HT_KEY key, A4_HT_VALUE value, size_t* idx);
static A4_ERRNO A4_HT_INSERT_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, A4_HT_VALUE value, size_t* idx);
#endif
/// @}

/// Resize hashtable.
static A4_ERRNO A4_HT_RESIZE(A4_HT_NAME* ht, size_t new_n_buckets);

/// Check if there is an entry with the given key.
/// @{
static inline bool A4_HT_CONTAINS(const A4_HT_NAME* ht, A4_HT_KEY key);
static inline bool A4_HT_CONTAINS_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash);
static inline bool A4_HT_CONTAINS_HINT_IDX(const A4_HT_NAME* ht, A4_HT_KEY key, size_t idx);
/// @}

/// Get an entry with the given key. The behaviour is undefined if there is no entry that corresponds to the given key.
/// @{
#ifndef A4_HT_NO_VALUE
static inline A4_HT_VALUE A4_HT_GET(const A4_HT_NAME* ht, A4_HT_KEY key);
static inline A4_HT_VALUE A4_HT_GET_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash);
static inline A4_HT_VALUE A4_HT_GET_HINT_IDX(const A4_HT_NAME* ht, A4_HT_KEY key, size_t idx);
#endif
/// @}

/// Get an entry with the given key. Returns A4_E_INDEX_ERROR if entry not found.
/// @{
#ifndef A4_HT_NO_VALUE
static inline A4_ERRNO A4_HT_SAFE_GET(const A4_HT_NAME* ht, A4_HT_KEY key, A4_HT_VALUE* result);
static inline A4_ERRNO A4_HT_SAFE_GET_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, A4_HT_VALUE* result);
static inline A4_ERRNO A4_HT_SAFE_GET_HINT_IDX(const A4_HT_NAME* ht, A4_HT_KEY key, size_t idx, A4_HT_VALUE* result);
#endif
/// @}

/// Remove an entry with the given key and return ownership over the entry components to the caller.
/// Returns A4_E_INDEX_ERROR if entry not found.
/// @{
static inline A4_ERRNO A4_HT_POP(A4_HT_NAME* ht, A4_HT_KEY key, A4_HT_POP_RESULT* result);
static inline A4_ERRNO A4_HT_POP_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, A4_HT_POP_RESULT* result);
static inline A4_ERRNO A4_HT_POP_HINT_IDX(A4_HT_NAME* ht, A4_HT_KEY key, size_t i, A4_HT_POP_RESULT* result);
/// @}

/// Remove and destroy an entry with the given key.
/// @{
static inline A4_ERRNO A4_HT_REMOVE(A4_HT_NAME* ht, A4_HT_KEY key);
static inline A4_ERRNO A4_HT_REMOVE_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash);
static inline A4_ERRNO A4_HT_REMOVE_HINT_IDX(A4_HT_NAME* ht, A4_HT_KEY key, size_t idx);
/// @}

/// Remove and destroy all elements from the hashtable.
static inline void A4_HT_CLEAR(A4_HT_NAME* ht);


// Implementation details
// --------------------------------------------------------------------------------------------------------------------

#define __A4_HT_ISEMPTY(flag, i) ((flag[i >> 4u] >> ((i & 0xFU) << 1u)) & 2u)
#define __A4_HT_ISDEL(flag, i) ((flag[i >> 4u] >> ((i & 0xFU) << 1u)) & 1u)
#define __A4_HT_ISEITHER(flag, i) ((flag[i >> 4u] >> ((i & 0xFU) << 1u)) & 3u)
#define __A4_HT_SET_ISDEL_FALSE(flag, i) (flag[i >> 4u] &= ~(1ul << ((i & 0xFU) << 1u)))
#define __A4_HT_SET_ISEMPTY_FALSE(flag, i) (flag[i >> 4u] &= ~(2ul << ((i & 0xFU) << 1u)))
#define __A4_HT_SET_ISBOTH_FALSE(flag, i) (flag[i >> 4u] &= ~(3ul << ((i & 0xFU) << 1u)))
#define __A4_HT_SET_ISDEL_TRUE(flag, i) (flag[i >> 4u] |= 1ul << ((i & 0xFU) << 1u))
#define __A4_HT_FSIZE(m) ((m) < 16u ? 1 : (m) >> 4u)
#define __A4_HT_ROUNDUP64(x) (--(x), (x) |= (x) >> 1u, (x) |= (x) >> 2u, (x) |= (x) >> 4u, (x) |= (x) >> 8u, (x) |= (x) >> 16u, (x) |= (x) >> 32u, ++(x))

static inline A4_HT_NAME* A4_NODISCARD A4_HT_NEW(void) {
    return (A4_HT_NAME*) calloc(1, sizeof(A4_HT_NAME));
}

static inline void A4_HT_DELETE(A4_HT_NAME* ht) {
    if (!ht) return;

    A4_HT_CLEAR(ht);

    free(ht->keys);
#ifndef A4_HT_NO_VALUE
    free(ht->values);
#endif
    free(ht->flags);
    free(ht);
}

static inline size_t A4_HT_SIZE(const A4_HT_NAME* ht) {
    assert(ht);

    return ht->size;
}

static inline bool A4_HT_EMPTY(const A4_HT_NAME* ht) {
    return !A4_HT_SIZE(ht);
}

static inline size_t A4_HT_CALCULATE_IDX(const A4_HT_NAME* ht, A4_HT_KEY key) {
    return A4_HT_CALCULATE_IDX_HINT_HASH(ht, key, A4_HT_KEY_HASH(key));
}
static inline size_t A4_HT_CALCULATE_IDX_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash) {
    assert(ht);

    if (ht->n_buckets) {
        uint64_t step = 0;
        uint64_t mask = ht->n_buckets - 1;
        uint64_t i = hash & mask;
        uint64_t last = i;
        while (!__A4_HT_ISEMPTY(ht->flags, i) && (__A4_HT_ISDEL(ht->flags, i) || !A4_HT_KEY_EQ(ht->keys[i], key))) {
            i = (i + (++step)) & mask;
            if (i == last) {
                return ht->n_buckets;
            }
        }
        return i;
    } else {
        return 0;
    }
}

#ifdef A4_HT_NO_VALUE
static inline A4_ERRNO A4_HT_INSERT(A4_HT_NAME* ht, A4_HT_KEY key, size_t* idx) {
    return A4_HT_INSERT_HINT_HASH(ht, key, A4_HT_KEY_HASH(key), idx);
}
#else
static inline A4_ERRNO A4_HT_INSERT(A4_HT_NAME* ht, A4_HT_KEY key, A4_HT_VALUE value, size_t* idx) {
    return A4_HT_INSERT_HINT_HASH(ht, key, A4_HT_KEY_HASH(key), value, idx);
}
#endif

#ifdef A4_HT_NO_VALUE
static A4_ERRNO A4_HT_INSERT_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, size_t* idx)
#else
static A4_ERRNO A4_HT_INSERT_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, A4_HT_VALUE value, size_t* idx)
#endif
{
    assert(ht);

    uint64_t x;

    if (ht->n_occupied >= ht->upper_bound) { /* update the hash table */
        A4_Errno err;
        if (ht->n_buckets > (ht->size << 1u)) {
            err = A4_HT_RESIZE(ht, ht->n_buckets - 1);
        } else {
            err = A4_HT_RESIZE(ht, ht->n_buckets + 1);
        }
        if (err != A4_SUCCESS) {
            if (idx) *idx = ht->n_buckets;
            return err;
        }
    } /* TODO: to implement automatically shrinking; resize() already support shrinking */
    {
        uint64_t k, i, site, last, mask = ht->n_buckets - 1, step = 0;
        x = site = ht->n_buckets;
        k = hash;
        i = k & mask;
        if (__A4_HT_ISEMPTY(ht->flags, i)) {
            x = i; /* for speed up */
        } else {
            last = i;
            while (!__A4_HT_ISEMPTY(ht->flags, i) && (__A4_HT_ISDEL(ht->flags, i) || !A4_HT_KEY_EQ(ht->keys[i], key))) {
                if (__A4_HT_ISDEL(ht->flags, i)) site = i;
                i = (i + (++step)) & mask;
                if (i == last) {
                    x = site;
                    break;
                }
            }
            if (x == ht->n_buckets) {
                if (__A4_HT_ISEMPTY(ht->flags, i) && site != ht->n_buckets) {
                    x = site;
                } else {
                    x = i;
                }
            }
        }
    }
    if (__A4_HT_ISEMPTY(ht->flags, x)) { /* not present at all */
        ht->keys[x] = key;
#ifndef A4_HT_NO_VALUE
        ht->values[x] = value;
#endif
        __A4_HT_SET_ISBOTH_FALSE(ht->flags, x);
        ++ht->size;
        ++ht->n_occupied;
    } else if (__A4_HT_ISDEL(ht->flags, x)) { /* deleted */
        ht->keys[x] = key;
#ifndef A4_HT_NO_VALUE
        ht->values[x] = value;
#endif
        __A4_HT_SET_ISBOTH_FALSE(ht->flags, x);
        ++ht->size;
    } else { /* override */
        A4_HT_KEY_MOVE(ht->keys[x], key);
#ifndef A4_HT_NO_VALUE
        A4_HT_VALUE_MOVE(ht->values[x], value);
#endif
    }

    if (idx) *idx = x;
    return A4_SUCCESS;
}

static A4_ERRNO A4_HT_RESIZE(A4_HT_NAME* ht, size_t new_n_buckets) {
    assert(ht);

    size_t j = 1;
    uint32_t* new_flags = 0;
    {
        __A4_HT_ROUNDUP64(new_n_buckets);
        if (new_n_buckets < 4) new_n_buckets = 4;
        if (ht->size >= (size_t) (new_n_buckets * 0.77 + 0.5)) {
            j = 0;  /* required size is too small -- don't do anything, just leave the hashtable as it is */
        } else { /* hash table size to be changed (shrink or expand); rehash */
            new_flags = (uint32_t*) malloc(__A4_HT_FSIZE(new_n_buckets) * sizeof(uint32_t));
            if (!new_flags) {
                return A4_E_MEMORY;
            }
            memset(new_flags, 0xAA, __A4_HT_FSIZE(new_n_buckets) * sizeof(uint32_t));
            if (ht->n_buckets < new_n_buckets) {    /* expand */
                A4_HT_KEY* new_keys = (A4_HT_KEY*) realloc((void*) ht->keys, new_n_buckets * sizeof(A4_HT_KEY));
                if (!new_keys) {
                    free(new_flags);
                    return A4_E_MEMORY;
                }
                ht->keys = new_keys;
#ifndef A4_HT_NO_VALUE
                A4_HT_VALUE* new_vals = (A4_HT_VALUE*) realloc((void*) ht->values, new_n_buckets * sizeof(A4_HT_VALUE));
                if (!new_vals) {
                    free(new_flags);
                    return A4_E_MEMORY;
                }
                ht->values = new_vals;
#endif
            } /* otherwise shrink */
        }
    }
    if (j) { /* rehashing is needed */
        for (j = 0; j != ht->n_buckets; ++j) {
            if (__A4_HT_ISEITHER(ht->flags, j) == 0) {
                A4_HT_KEY key = ht->keys[j];
#ifndef A4_HT_NO_VALUE
                A4_HT_VALUE val = ht->values[j];
#endif
                size_t new_mask;
                new_mask = new_n_buckets - 1;
                __A4_HT_SET_ISDEL_TRUE(ht->flags, j);
                while (1) { /* kick-out process; sort of like in Cuckoo hashing */
                    size_t k, i, step = 0;
                    k = A4_HT_KEY_HASH(key);
                    i = k & new_mask;
                    while (!__A4_HT_ISEMPTY(new_flags, i)) {
                        i = (i + (++step)) & new_mask;
                    }
                    __A4_HT_SET_ISEMPTY_FALSE(new_flags, i);
                    if (i < ht->n_buckets && __A4_HT_ISEITHER(ht->flags, i) == 0) { /* kick out the existing element */
                        {
                            A4_HT_KEY tmp = ht->keys[i];
                            ht->keys[i] = key;
                            key = tmp;
                        }
#ifndef A4_HT_NO_VALUE
                        {
                            A4_HT_VALUE tmp = ht->values[i];
                            ht->values[i] = val;
                            val = tmp;
                        }
#endif
                        __A4_HT_SET_ISDEL_TRUE(ht->flags, i); /* mark it as deleted in the old hash table */
                    } else { /* write the element and jump out of the loop */
                        ht->keys[i] = key;
#ifndef A4_HT_NO_VALUE
                        ht->values[i] = val;
#endif
                        break;
                    }
                }
            }
        }
        if (ht->n_buckets > new_n_buckets) { /* shrink the hash table */
            ht->keys = (A4_HT_KEY*) realloc((void*) ht->keys, new_n_buckets * sizeof(A4_HT_KEY));
#ifndef A4_HT_NO_VALUE
            ht->values = (A4_HT_VALUE*) realloc((void*) ht->values, new_n_buckets * sizeof(A4_HT_VALUE));
#endif
        }
        free(ht->flags); /* free the working space */
        ht->flags = new_flags;
        ht->n_buckets = new_n_buckets;
        ht->n_occupied = ht->size;
        ht->upper_bound = (size_t) (ht->n_buckets * 0.77 + 0.5);
    }
    return A4_SUCCESS;
}

static inline bool A4_HT_CONTAINS(const A4_HT_NAME* ht, A4_HT_KEY key) {
    return A4_HT_CONTAINS_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX(ht, key));
}
static inline bool A4_HT_CONTAINS_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash) {
    return A4_HT_CONTAINS_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX_HINT_HASH(ht, key, hash));
}
static inline bool A4_HT_CONTAINS_HINT_IDX(const A4_HT_NAME* ht, A4_HT_KEY key, size_t idx) {
    assert(ht);

    if (idx == ht->n_buckets) {
        return false;
    }

    assert(idx < ht->n_buckets && (__A4_HT_ISEITHER(ht->flags, idx) || A4_HT_KEY_EQ(key, ht->keys[idx])));

    return !__A4_HT_ISEITHER(ht->flags, idx);
}

#ifndef A4_HT_NO_VALUE
static inline A4_HT_VALUE A4_HT_GET(const A4_HT_NAME* ht, A4_HT_KEY key) {
    return A4_HT_GET_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX(ht, key));
}
static inline A4_HT_VALUE A4_HT_GET_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash) {
    return A4_HT_GET_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX_HINT_HASH(ht, key, hash));
}
static inline A4_HT_VALUE A4_HT_GET_HINT_IDX(const A4_HT_NAME* ht, A4_HT_KEY key, size_t idx) {
    A4_HT_VALUE result;
    A4_Errno err = A4_HT_SAFE_GET_HINT_IDX(ht, key, idx, &result);
    assert(err == A4_SUCCESS);
    return result;
}
#endif

#ifndef A4_HT_NO_VALUE
static inline A4_ERRNO A4_HT_SAFE_GET(const A4_HT_NAME* ht, A4_HT_KEY key, A4_HT_VALUE* result) {
    return A4_HT_SAFE_GET_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX(ht, key), result);
}
static inline A4_ERRNO A4_HT_SAFE_GET_HINT_HASH(const A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, A4_HT_VALUE* result) {
    return A4_HT_SAFE_GET_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX_HINT_HASH(ht, key, hash), result);
}
static inline A4_ERRNO A4_HT_SAFE_GET_HINT_IDX(const A4_HT_NAME* ht, A4_HT_KEY key, size_t idx, A4_HT_VALUE* result) {
    if (A4_HT_CONTAINS_HINT_IDX(ht, key, idx)) {
        *result = ht->values[idx];
        return A4_SUCCESS;
    } else {
        return A4_E_INDEX_ERROR;
    }
}
#endif

static inline A4_ERRNO A4_HT_POP(A4_HT_NAME* ht, A4_HT_KEY key, A4_HT_POP_RESULT* result) {
    return A4_HT_POP_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX(ht, key), result);
}
static inline A4_ERRNO A4_HT_POP_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash, A4_HT_POP_RESULT* result) {
    return A4_HT_POP_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX_HINT_HASH(ht, key, hash), result);
}
static inline A4_ERRNO A4_HT_POP_HINT_IDX(A4_HT_NAME* ht, A4_HT_KEY key, size_t idx, A4_HT_POP_RESULT* result) {
    if (A4_HT_CONTAINS_HINT_IDX(ht, key, idx)) {
#ifndef A4_HT_NO_VALUE
        *result = (A4_HT_POP_RESULT) {
            .key = ht->keys[idx],
            .value = ht->values[idx],
        };
#else
        *result = ht->keys[idx];
#endif
        __A4_HT_SET_ISDEL_TRUE(ht->flags, idx);
        --ht->size;

        return A4_SUCCESS;
    } else {
        return A4_E_INDEX_ERROR;
    }
}

static inline A4_ERRNO A4_HT_REMOVE(A4_HT_NAME* ht, A4_HT_KEY key) {
    return A4_HT_REMOVE_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX(ht, key));
}
static inline A4_ERRNO A4_HT_REMOVE_HINT_HASH(A4_HT_NAME* ht, A4_HT_KEY key, uint64_t hash) {
    return A4_HT_REMOVE_HINT_IDX(ht, key, A4_HT_CALCULATE_IDX_HINT_HASH(ht, key, hash));
}
static inline A4_ERRNO A4_HT_REMOVE_HINT_IDX(A4_HT_NAME* ht, A4_HT_KEY key, size_t idx) {
    A4_HT_POP_RESULT result;
    A4_Errno err = A4_HT_POP_HINT_IDX(ht, key, idx, &result);
    if (err == A4_SUCCESS) {
#ifndef A4_HT_NO_VALUE
        A4_HT_KEY_DTOR(result.key);
        A4_HT_VALUE_DTOR(result.value);
#else
        A4_HT_KEY_DTOR(result);
#endif
    }
    return err;
}

static inline void A4_HT_CLEAR(A4_HT_NAME* ht) {
    assert(ht);

    if (ht->flags) {
#if !defined(A4_HT_KEY_NO_DTOR) || !defined(A4_HT_VALUE_NO_DTOR)
        for (size_t i = 0; i < ht->n_buckets; ++i) {
            if (__A4_HT_ISEITHER(ht->flags, i))
                continue;
            A4_HT_KEY_DTOR(ht->keys[i]);
#ifndef A4_HT_NO_VALUE
            A4_HT_VALUE_DTOR(ht->values[i]);
#endif
        }
#endif
        memset(ht->flags, 0xAA, __A4_HT_FSIZE(ht->n_buckets) * sizeof(uint32_t));
        ht->size = ht->n_occupied = 0;
    }
}

#undef __A4_HT_ISEMPTY
#undef __A4_HT_ISDEL
#undef __A4_HT_ISEITHER
#undef __A4_HT_SET_ISDEL_FALSE
#undef __A4_HT_SET_ISEMPTY_FALSE
#undef __A4_HT_SET_ISBOTH_FALSE
#undef __A4_HT_SET_ISDEL_TRUE
#undef __A4_HT_FSIZE
#undef __A4_HT_ROUNDUP64

#undef A4_HT_NAME
#undef A4_HT_KEY
#undef A4_HT_VALUE
#ifdef A4_HT_NO_VALUE
#    undef A4_HT_NO_VALUE
#endif
#undef A4_HT_KEY_DTOR
#ifdef A4_HT_KEY_NO_DTOR
#    undef A4_HT_KEY_NO_DTOR
#endif
#undef A4_HT_KEY_MOVE
#undef A4_HT_VALUE_DTOR
#ifdef A4_HT_VALUE_NO_DTOR
#    undef A4_HT_VALUE_NO_DTOR
#endif
#undef A4_HT_VALUE_MOVE
#undef A4_HT_KEY_HASH
#undef A4_HT_KEY_EQ

#undef A4_HT_POP_RESULT
#undef A4_HT_NEW
#undef A4_HT_DELETE
#undef A4_HT_SIZE
#undef A4_HT_EMPTY
#undef A4_HT_CALCULATE_IDX
#undef A4_HT_CALCULATE_IDX_HINT_HASH
#undef A4_HT_INSERT
#undef A4_HT_INSERT_HINT_HASH
#undef A4_HT_RESIZE
#undef A4_HT_CONTAINS
#undef A4_HT_CONTAINS_HINT_HASH
#undef A4_HT_CONTAINS_HINT_IDX
#undef A4_HT_GET
#undef A4_HT_GET_HINT_HASH
#undef A4_HT_GET_HINT_IDX
#undef A4_HT_SAFE_GET
#undef A4_HT_SAFE_GET_HINT_HASH
#undef A4_HT_SAFE_GET_HINT_IDX
#undef A4_HT_POP
#undef A4_HT_POP_HINT_HASH
#undef A4_HT_POP_HINT_IDX
#undef A4_HT_REMOVE
#undef A4_HT_REMOVE_HINT_HASH
#undef A4_HT_REMOVE_HINT_IDX
#undef A4_HT_CLEAR
