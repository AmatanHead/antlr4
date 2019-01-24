/**
 * Generic list. Including this file generated a list struct and bunch of API methods based on special macro values.
 *
 * Usage:
 *
 * 1. define three macros:
 *   - A4_LIST_NAME -- name of the list class;
 *   - A4_LIST_PAYLOAD -- type of the payload;
 *   - A4_LIST_DTOR -- function to be called whenever payload gets destroyed. Alternatively, define A4_LIST_NO_DTOR
 *     if payload does not require destruction.
 * 2. include `list.inl`;
 * 3. done.
 *
 * For example:
 *
 * @code
 * #define A4_LIST_NAME IntList
 * #define A4_LIST_PAYLOAD int
 * #define A4_LIST_NO_DTOR
 * #include <antlr4/util/list.inl>  // Note: list.inl will undef all macros for you
 *
 * // Now you get the following structs and methods:
 *
 * typedef struct IntList_Elem IntList_Elem;
 * typedef struct IntList IntList;
 * IntList* IntList_New(void);
 * void IntList_Delete(IntList* list);
 * ...
 * @endcode
 */

#ifndef A4_LIST_NAME
#error "A4_LIST_NAME must be defined prior to list.inl inclusion"
#define A4_LIST_NAME UnnamedList
#endif

#ifndef A4_LIST_PAYLOAD
#error "A4_LIST_PAYLOAD must be defined prior to list.inl inclusion"
#define A4_LIST_PAYLOAD void*
#endif

#if defined(A4_LIST_DTOR) && defined(A4_LIST_NO_DTOR)
#    error "only one of A4_LIST_DTOR and A4_LIST_NO_DTOR may be defined at the same time"
#endif
#ifndef A4_LIST_DTOR
#   ifdef A4_LIST_NO_DTOR
#       define A4_LIST_PAYLOAD_NODISCARD
#       define A4_LIST_DTOR(x) (void)x;
#   else
#       error "A4_LIST_DTOR or A4_LIST_NO_DTOR must be defined prior to list.inl inclusion"
#       define A4_LIST_PAYLOAD_NODISCARD A4_NODISCARD
#       define A4_LIST_DTOR(x) (void)x;
#   endif
#else
#   define A4_LIST_PAYLOAD_NODISCARD A4_NODISCARD
#endif

#include "../common.h"
#include "../memory/pool.h"

#include <stddef.h>
#include <errno.h>

#define A4_LIST_ELEM A4_CAT(A4_LIST_NAME, _Elem)
#define A4_LIST_ELEM_NEW A4_CAT(_, A4_CAT(A4_LIST_NAME, _Elem_New))
#define A4_LIST_ELEM_DELETE_PAYLOAD A4_CAT(_, A4_CAT(A4_LIST_NAME, _Elem_DeletePayload))
#define A4_LIST_ELEM_DELETE A4_CAT(_, A4_CAT(A4_LIST_NAME, _Elem_Delete))
#define A4_LIST_ELEM_HAS_NEXT A4_CAT(A4_LIST_NAME, _Elem_HasNext)
#define A4_LIST_ELEM_NEXT A4_CAT(A4_LIST_NAME, _Elem_Next)
#define A4_LIST_ELEM_HAS_PREVIOUS A4_CAT(A4_LIST_NAME, _Elem_HasPrevious)
#define A4_LIST_ELEM_PREVIOUS A4_CAT(A4_LIST_NAME, _Elem_Previous)
#define A4_LIST_ELEM_PAYLOAD A4_CAT(A4_LIST_NAME, _Elem_Payload)
#define A4_LIST_NEW A4_CAT(A4_LIST_NAME, _New)
#define A4_LIST_DELETE A4_CAT(A4_LIST_NAME, _Delete)
#define A4_LIST_SIZE A4_CAT(A4_LIST_NAME, _Size)
#define A4_LIST_EMPTY A4_CAT(A4_LIST_NAME, _Empty)
#define A4_LIST_HEAD A4_CAT(A4_LIST_NAME, _Head)
#define A4_LIST_TAIL A4_CAT(A4_LIST_NAME, _Tail)
#define A4_LIST_FIRST A4_CAT(A4_LIST_NAME, _First)
#define A4_LIST_LAST A4_CAT(A4_LIST_NAME, _Last)
#define A4_LIST_INSERT_BEFORE A4_CAT(A4_LIST_NAME, _InsertBefore)
#define A4_LIST_INSERT_AFTER A4_CAT(A4_LIST_NAME, _InsertAfter)
#define A4_LIST_APPEND A4_CAT(A4_LIST_NAME, _Append)
#define A4_LIST_PREPEND A4_CAT(A4_LIST_NAME, _Prepend)
#define A4_LIST_INSERT_BEFORE_MOVE A4_CAT(A4_LIST_NAME, _InsertBeforeMove)
#define A4_LIST_INSERT_AFTER_MOVE A4_CAT(A4_LIST_NAME, _InsertAfterMove)
#define A4_LIST_APPEND_MOVE A4_CAT(A4_LIST_NAME, _AppendMove)
#define A4_LIST_PREPEND_MOVE A4_CAT(A4_LIST_NAME, _PrependMove)
#define A4_LIST_POP A4_CAT(A4_LIST_NAME, _Pop)
#define A4_LIST_POP_FRONT A4_CAT(A4_LIST_NAME, _PopFront)
#define A4_LIST_POP_BACK A4_CAT(A4_LIST_NAME, _PopBack)
#define A4_LIST_REMOVE A4_CAT(A4_LIST_NAME, _Remove)
#define A4_LIST_REMOVE_FRONT A4_CAT(A4_LIST_NAME, _RemoveFront)
#define A4_LIST_REMOVE_BACK A4_CAT(A4_LIST_NAME, _RemoveBack)
#define A4_LIST_COPY_TO_POOL A4_CAT(A4_LIST_NAME, _CopyToPool)
#define A4_LIST_CLEAR A4_CAT(A4_LIST_NAME, _Clear)


// List element API
// --------------------------------------------------------------------------------------------------------------------

/// List element type.
/// Contents of this struct shall only be changed via API methods below.
a4_struct(A4_LIST_ELEM) {
    A4_LIST_PAYLOAD payload;
    struct A4_LIST_ELEM* _prev;
    struct A4_LIST_ELEM* _next;
};

/// Check if the list element is not the last one in the list.
static inline bool A4_LIST_ELEM_HAS_NEXT(const A4_LIST_ELEM* elem);

/// Get next element of the list.
static inline A4_LIST_ELEM* A4_LIST_ELEM_NEXT(const A4_LIST_ELEM* elem);

/// Check if the list element is not the first one in the list.
static inline bool A4_LIST_ELEM_HAS_PREVIOUS(const A4_LIST_ELEM* elem);

/// Get previous element of the list.
static inline A4_LIST_ELEM* A4_LIST_ELEM_PREVIOUS(const A4_LIST_ELEM* elem);

/// Get list element payload.
static inline A4_LIST_PAYLOAD A4_LIST_ELEM_PAYLOAD(const A4_LIST_ELEM* elem);


// List API
// --------------------------------------------------------------------------------------------------------------------

/// List type.
/// Contents of this struct shall only be changed via API methods below.
a4_struct(A4_LIST_NAME) {
    A4_LIST_ELEM* head;
    A4_LIST_ELEM* tail;
    size_t size;
};

/// Create new empty list.
static inline A4_LIST_NAME* A4_NODISCARD A4_LIST_NEW(void);

/// Delete list and all its contents.
static inline void A4_LIST_DELETE(A4_LIST_NAME* list);

/// Get number of elements in the list.
static inline size_t A4_LIST_SIZE(const A4_LIST_NAME* list);

/// Check if the list is empty.
static inline bool A4_LIST_EMPTY(const A4_LIST_NAME* list);

/// Get pointer to the first element in the list.
static inline A4_LIST_ELEM* A4_LIST_HEAD(const A4_LIST_NAME* list);

/// Get pointer to the last element in the list.
static inline A4_LIST_ELEM* A4_LIST_TAIL(const A4_LIST_NAME* list);

/// Get payload of the first element in the list. List should be non-empty.
static inline A4_LIST_PAYLOAD A4_LIST_FIRST(const A4_LIST_NAME* list);

/// Get payload of the last element in the list. List should be non-empty.
static inline A4_LIST_PAYLOAD A4_LIST_LAST(const A4_LIST_NAME* list);

/// Take ownership over a payload and insert it before the given node. If insertion wasn't successful,
/// ownership over payload is not taken. The behaviour is undefined if given node is not a part of the given list.
static inline A4_ERRNO A4_LIST_INSERT_BEFORE(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload);

/// Take ownership over a payload and insert it after the given node. If insertion wasn't successful,
/// ownership over payload is not taken. The behaviour is undefined if given node is not a part of the given list.
static inline A4_ERRNO A4_LIST_INSERT_AFTER(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload);

/// Take ownership over a payload and insert it to the beginning of the list. If insertion wasn't successful,
/// ownership over payload is not taken.
static inline A4_ERRNO A4_LIST_APPEND(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload);

/// Take ownership over a payload and insert it to the end of the list. If insertion wasn't successful,
/// ownership over payload is not taken.
static inline A4_ERRNO A4_LIST_PREPEND(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload);

/// Take ownership over a payload and insert it before the given node. If insertion wasn't successful,
/// the payload is destroyed.
static inline A4_ERRNO A4_LIST_INSERT_BEFORE_MOVE(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload);

/// Take ownership over a payload and insert it after the given node. If insertion wasn't successful,
/// the payload is destroyed.
static inline A4_ERRNO A4_LIST_INSERT_AFTER_MOVE(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload);

/// Take ownership over a payload and insert it to the beginning of the list. If insertion wasn't successful,
/// the payload is destroyed.
static inline A4_ERRNO A4_LIST_APPEND_MOVE(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload);

/// Take ownership over a payload and insert it to the end of the list. If insertion wasn't successful,
/// the payload is destroyed.
static inline A4_ERRNO A4_LIST_PREPEND_MOVE(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload);

/// Pop an element from the list. Return ownership over the payload to the caller.
static inline A4_LIST_PAYLOAD A4_LIST_PAYLOAD_NODISCARD A4_LIST_POP(A4_LIST_NAME* list, A4_LIST_ELEM* iter);

/// Pop the first element from the list. Return ownership over the payload to the caller.
static inline A4_LIST_PAYLOAD A4_LIST_PAYLOAD_NODISCARD A4_LIST_POP_FRONT(A4_LIST_NAME* list);

/// Pop the last element from the list. Return ownership over the payload to the caller.
static inline A4_LIST_PAYLOAD A4_LIST_PAYLOAD_NODISCARD A4_LIST_POP_BACK(A4_LIST_NAME* list);

/// Remove an element from the list and delete its payload.
static inline void A4_LIST_REMOVE(A4_LIST_NAME* list, A4_LIST_ELEM* iter);

/// Remove the first element from the list and delete its payload.
static inline void A4_LIST_REMOVE_FRONT(A4_LIST_NAME* list);

/// Remove the last element from the list and delete its payload.
static inline void A4_LIST_REMOVE_BACK(A4_LIST_NAME* list);

#ifdef A4_LIST_NO_DTOR
/// Copy list payloads into an array allocated in a pool. Note that this function will only be defined on types with
/// trivial destructors. Other types must be copied to pool manually to ensure lifetime correctness.
static inline A4_LIST_PAYLOAD* A4_LIST_COPY_TO_POOL(const A4_LIST_NAME* list, A4_MemoryPool* pool);
#endif

/// Remove all elements from the list.
static inline void A4_LIST_CLEAR(A4_LIST_NAME* list);


// Implementation details
// --------------------------------------------------------------------------------------------------------------------

/// Take ownership over payload and allocate new list element. If allocation wasn't successful, ownership over payload
/// is not taken.
static inline A4_LIST_ELEM* A4_NODISCARD A4_LIST_ELEM_NEW(A4_LIST_PAYLOAD payload) {
    A4_LIST_ELEM* elem = (A4_LIST_ELEM*)malloc(sizeof(A4_LIST_ELEM));
    if (!elem) return NULL;
    elem->_next = elem->_prev = NULL;
    elem->payload = payload;
    return elem;
}

/// Call destructor on list payload. Used to negate effects of return and goto statements that may be encountered
/// in dtor code.
static inline void A4_LIST_ELEM_DELETE_PAYLOAD(A4_LIST_PAYLOAD payload) {
    // NOTE: if you're seeing warning about unused variable `payload`, it's most likely you've messed up A4_LIST_DTOR.
    A4_LIST_DTOR(payload);
}

/// Destroy list element.
static inline void A4_LIST_ELEM_DELETE(A4_LIST_ELEM* elem) {
    if (elem) A4_LIST_ELEM_DELETE_PAYLOAD(elem->payload);
    free(elem);
}

static inline bool A4_LIST_ELEM_HAS_NEXT(const A4_LIST_ELEM* elem) {
    assert(elem);

    return elem->_next != NULL;
}

static inline A4_LIST_ELEM* A4_LIST_ELEM_NEXT(const A4_LIST_ELEM* elem) {
    assert(elem);

    return elem->_next;
}

static inline bool A4_LIST_ELEM_HAS_PREVIOUS(const A4_LIST_ELEM* elem) {
    assert(elem);

    return elem->_prev != NULL;
}

static inline A4_LIST_ELEM* A4_LIST_ELEM_PREVIOUS(const A4_LIST_ELEM* elem) {
    assert(elem);

    return elem->_prev;
}

static inline A4_LIST_PAYLOAD A4_LIST_ELEM_PAYLOAD(const A4_LIST_ELEM* elem) {
    assert(elem);

    return elem->payload;
}

static inline A4_LIST_NAME* A4_NODISCARD A4_LIST_NEW(void) {
    A4_LIST_NAME* list = (A4_LIST_NAME*)malloc(sizeof(A4_LIST_NAME));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

static inline void A4_LIST_DELETE(A4_LIST_NAME* list) {
    if (list) A4_LIST_CLEAR(list);
    free(list);
}

static inline size_t A4_LIST_SIZE(const A4_LIST_NAME* list) {
    assert(list);

    return list->size;
}

static inline bool A4_LIST_EMPTY(const A4_LIST_NAME* list) {
    return A4_LIST_SIZE(list) == 0;
}

static inline A4_LIST_ELEM* A4_LIST_HEAD(const A4_LIST_NAME* list) {
    assert(list);

    return list->head;
}

static inline A4_LIST_ELEM* A4_LIST_TAIL(const A4_LIST_NAME* list) {
    assert(list);

    return list->tail;
}

static inline A4_LIST_PAYLOAD A4_LIST_FIRST(const A4_LIST_NAME* list) {
    return A4_LIST_ELEM_PAYLOAD(A4_LIST_HEAD(list));
}

static inline A4_LIST_PAYLOAD A4_LIST_LAST(const A4_LIST_NAME* list) {
    return A4_LIST_ELEM_PAYLOAD(A4_LIST_TAIL(list));
}

static inline A4_ERRNO A4_LIST_INSERT_BEFORE(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload) {
    assert(list);

    A4_LIST_ELEM* new_elem = A4_LIST_ELEM_NEW(payload);
    if (!new_elem) return ENOMEM;

    if (!iter) {
        assert(A4_LIST_EMPTY(list));
        assert(list->head == NULL);
        assert(list->tail == NULL);
        list->head = list->tail = new_elem;
    } else {
        assert(!A4_LIST_EMPTY(list));
        assert(list->head != NULL);
        assert(list->tail != NULL);
        new_elem->_next = iter;
        new_elem->_prev = iter->_prev;
        iter->_prev = new_elem;
        if (new_elem->_prev)
            new_elem->_prev->_next = new_elem;
        else
            list->head = new_elem;
    }

    list->size += 1;

    return A4_SUCCESS;
}

static inline A4_ERRNO A4_LIST_INSERT_AFTER(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload) {
    assert(list);

    assert(list);

    A4_LIST_ELEM* new_elem = A4_LIST_ELEM_NEW(payload);
    if (!new_elem) return ENOMEM;

    if (!iter) {
        assert(A4_LIST_EMPTY(list));
        assert(list->head == NULL);
        assert(list->tail == NULL);
        list->head = list->tail = new_elem;
    } else {
        assert(!A4_LIST_EMPTY(list));
        assert(list->head != NULL);
        assert(list->tail != NULL);
        new_elem->_prev = iter;
        new_elem->_next = iter->_next;
        iter->_next = new_elem;
        if (new_elem->_next)
            new_elem->_next->_prev = new_elem;
        else
            list->tail = new_elem;
    }

    list->size += 1;

    return A4_SUCCESS;
}

static inline A4_ERRNO A4_LIST_APPEND(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload) {
    return A4_LIST_INSERT_AFTER(list, A4_LIST_TAIL(list), payload);
}

static inline A4_ERRNO A4_LIST_PREPEND(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload) {
    return A4_LIST_INSERT_BEFORE(list, A4_LIST_HEAD(list), payload);
}

static inline A4_ERRNO A4_LIST_INSERT_BEFORE_MOVE(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload) {
    A4_Errno res = A4_LIST_INSERT_BEFORE(list, iter, payload);
    if (res != A4_SUCCESS) A4_LIST_ELEM_DELETE_PAYLOAD(payload);
    return res;
}

static inline A4_ERRNO A4_LIST_INSERT_AFTER_MOVE(A4_LIST_NAME* list, A4_LIST_ELEM* iter, A4_LIST_PAYLOAD payload) {
    A4_Errno res = A4_LIST_INSERT_AFTER(list, iter, payload);
    if (res != A4_SUCCESS) A4_LIST_ELEM_DELETE_PAYLOAD(payload);
    return res;
}

static inline A4_ERRNO A4_LIST_APPEND_MOVE(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload) {
    return A4_LIST_INSERT_AFTER_MOVE(list, A4_LIST_TAIL(list), payload);
}

static inline A4_ERRNO A4_LIST_PREPEND_MOVE(A4_LIST_NAME* list, A4_LIST_PAYLOAD payload) {
    return A4_LIST_INSERT_BEFORE_MOVE(list, A4_LIST_HEAD(list), payload);
}

static inline A4_LIST_PAYLOAD A4_LIST_PAYLOAD_NODISCARD A4_LIST_POP(A4_LIST_NAME* list, A4_LIST_ELEM* iter) {
    assert(list);
    assert(iter);
    assert(list->size > 0);

    A4_LIST_PAYLOAD payload = iter->payload;

    if (iter->_prev) {
        iter->_prev->_next = iter->_next;
    } else {
        list->head = iter->_next;
    }

    if (iter->_next) {
        iter->_next->_prev = iter->_prev;
    } else {
        list->tail = iter->_prev;
    }

    free(iter);  // free list element without deleting payload

    list->size -= 1;

    return payload;
}

static inline A4_LIST_PAYLOAD A4_LIST_PAYLOAD_NODISCARD A4_LIST_POP_FRONT(A4_LIST_NAME* list) {
    return A4_LIST_POP(list, A4_LIST_HEAD(list));
}

static inline A4_LIST_PAYLOAD A4_LIST_PAYLOAD_NODISCARD A4_LIST_POP_BACK(A4_LIST_NAME* list) {
    return A4_LIST_POP(list, A4_LIST_TAIL(list));
}

static inline void A4_LIST_REMOVE(A4_LIST_NAME* list, A4_LIST_ELEM* iter) {
    A4_LIST_PAYLOAD payload = A4_LIST_POP(list, iter);
    A4_LIST_ELEM_DELETE_PAYLOAD(payload);
}

static inline void A4_LIST_REMOVE_FRONT(A4_LIST_NAME* list) {
    A4_LIST_REMOVE(list, A4_LIST_HEAD(list));
}

static inline void A4_LIST_REMOVE_BACK(A4_LIST_NAME* list) {
    A4_LIST_REMOVE(list, A4_LIST_TAIL(list));
}

#ifdef A4_LIST_NO_DTOR
static inline A4_LIST_PAYLOAD* A4_LIST_COPY_TO_POOL(const A4_LIST_NAME* list, A4_MemoryPool* pool) {
    assert(list);
    assert(pool);

    A4_LIST_PAYLOAD* payloads = A4_POOL_CALLOC(pool, A4_LIST_PAYLOAD, list->size);
    if (!payloads) return NULL;

    A4_LIST_ELEM* cur = list->head;
    size_t i = 0;
    while (cur) {
        payloads[i++] = cur->payload;
        cur = cur->_next;
    }

    return payloads;
}
#endif

static inline void A4_LIST_CLEAR(A4_LIST_NAME* list) {
    assert(list);

    A4_LIST_ELEM* head;
    while ((head = list->head)) {
        list->head = head->_next;
        A4_LIST_ELEM_DELETE(head);
    }

    list->size = 0;
    list->head = list->tail = NULL;
}

#undef A4_LIST_NAME
#undef A4_LIST_PAYLOAD
#ifdef A4_LIST_DTOR
#   undef A4_LIST_DTOR
#endif
#ifdef A4_LIST_NO_DTOR
#   undef A4_LIST_NO_DTOR
#endif
#undef A4_LIST_PAYLOAD_NODISCARD
#undef A4_LIST_ELEM
#undef A4_LIST_ELEM_NEW
#undef A4_LIST_ELEM_DELETE_PAYLOAD
#undef A4_LIST_ELEM_DELETE
#undef A4_LIST_ELEM_HAS_NEXT
#undef A4_LIST_ELEM_NEXT
#undef A4_LIST_ELEM_HAS_PREVIOUS
#undef A4_LIST_ELEM_PREVIOUS
#undef A4_LIST_ELEM_PAYLOAD
#undef A4_LIST_NEW
#undef A4_LIST_DELETE
#undef A4_LIST_SIZE
#undef A4_LIST_EMPTY
#undef A4_LIST_HEAD
#undef A4_LIST_TAIL
#undef A4_LIST_FIRST
#undef A4_LIST_LAST
#undef A4_LIST_INSERT_BEFORE
#undef A4_LIST_INSERT_AFTER
#undef A4_LIST_APPEND
#undef A4_LIST_PREPEND
#undef A4_LIST_INSERT_BEFORE_MOVE
#undef A4_LIST_INSERT_AFTER_MOVE
#undef A4_LIST_APPEND_MOVE
#undef A4_LIST_PREPEND_MOVE
#undef A4_LIST_POP
#undef A4_LIST_POP_FRONT
#undef A4_LIST_POP_BACK
#undef A4_LIST_REMOVE
#undef A4_LIST_REMOVE_FRONT
#undef A4_LIST_REMOVE_BACK
#undef A4_LIST_COPY_TO_POOL
#undef A4_LIST_CLEAR


#ifndef A4_LIST_FOREACH
#define A4_LIST_FOREACH(N, list, elem) \
    for (N##_Elem* elem = N##_Head(list); elem; elem = N##_Elem_Next(elem))
#endif
