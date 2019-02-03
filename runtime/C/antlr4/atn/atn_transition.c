#include "atn_transition.h"
#include "atn_state.h"
#include "atn.h"
#include "../token.h"

const char* A4_ATNTransitionTypeName(enum A4_ATNTransitionType type) {
    switch (type) {
        case A4_ATNTT_INVALID: return "INVALID";
        case A4_ATNTT_EPSILON: return "EPSILON";
        case A4_ATNTT_RANGE: return "RANGE";
        case A4_ATNTT_RULE: return "RULE";
        case A4_ATNTT_PREDICATE: return "PREDICATE";
        case A4_ATNTT_ATOM: return "ATOM";
        case A4_ATNTT_ACTION: return "ACTION";
        case A4_ATNTT_SET: return "SET";
        case A4_ATNTT_NOT_SET: return "NOT_SET";
        case A4_ATNTT_WILDCARD: return "WILDCARD";
        case A4_ATNTT_PRECEDENCE: return "PRECEDENCE";
    }

    A4_UNREACHABLE();
}

A4_ATNTransition* A4_NODISCARD A4_ATNTransition_New(A4_MemoryPool* pool, A4_ATNTransitionType type,
    A4_ATN* atn, A4_ATNState* target, uint16_t arg1, uint16_t arg2, uint16_t arg3) {

    switch (type) {
        case A4_ATNTT_INVALID:
            return NULL;
        case A4_ATNTT_EPSILON:
            return (A4_ATNTransition*) A4_ATNEpsilonTransition_New(pool, target);
        case A4_ATNTT_RANGE:
            if (arg3 != 0) {
                return (A4_ATNTransition*) A4_ATNRangeTransition_New(pool, target, A4_TOKEN_EOF, arg2);
            } else {
                return (A4_ATNTransition*) A4_ATNRangeTransition_New(pool, target, arg1, arg2);
            }
        case A4_ATNTT_RULE:
            if (arg1 >= atn->states.size) return NULL;
            if (!A4_IsRuleStartState(atn->states.data[arg1])) return NULL;
            return (A4_ATNTransition*) A4_ATNRuleTransition_New(pool, A4_ToRuleStartState(atn->states.data[arg1]), arg2, arg3, target);
        case A4_ATNTT_PREDICATE:
            return (A4_ATNTransition*) A4_ATNPredicateTransition_New(pool, target, arg1, arg2, arg3 != 0);
        case A4_ATNTT_ATOM:
            if (arg3 != 0) {
                return (A4_ATNTransition*) A4_ATNAtomTransition_New(pool, target, A4_TOKEN_EOF);
            } else {
                return (A4_ATNTransition*) A4_ATNAtomTransition_New(pool, target, arg1);
            }
        case A4_ATNTT_ACTION:
            return (A4_ATNTransition*) A4_ATNActionTransition_New(pool, target, arg1, arg2, arg3 != 0);
        case A4_ATNTT_SET:
            return (A4_ATNTransition*) A4_ATNSetTransition_New(pool, target);
        case A4_ATNTT_NOT_SET:
            return (A4_ATNTransition*) A4_ATNNotSetTransition_New(pool, target);
        case A4_ATNTT_WILDCARD:
            return (A4_ATNTransition*) A4_ATNWildcardTransition_New(pool, target);
        case A4_ATNTT_PRECEDENCE:
            return (A4_ATNTransition*) A4_ATNPrecedencePredicateTransition_New(pool, target, arg1);
    }

    A4_UNREACHABLE();
}

static inline A4_ATNTransition A4_NODISCARD A4_ATNTransition_Init(A4_ATNTransitionType type, bool is_epsilon, A4_ATNState* target) {
    return (A4_ATNTransition) {
        .type = type,
        .is_epsilon = is_epsilon,
        .target = target,
    };
}

static inline A4_ATNAbstractPredicateTransition A4_NODISCARD A4_ATNAbstractPredicateTransition_Init(A4_ATNTransitionType type, bool is_epsilon, A4_ATNState* target) {
    return (A4_ATNAbstractPredicateTransition) {
        .base = A4_ATNTransition_Init(type, is_epsilon, target),
    };
}

static inline A4_ATNSetTransition A4_NODISCARD A4_ATNSetTransition_Init(A4_ATNTransitionType type, A4_ATNState* target) {
    return (A4_ATNSetTransition) {
        .base = A4_ATNTransition_Init(type, false, target),
        .set = NULL,
    };
}


A4_ATNEpsilonTransition* A4_NODISCARD A4_ATNEpsilonTransition_New(A4_MemoryPool* pool, A4_ATNState* target) {
    A4_ATNEpsilonTransition* transition = A4_POOL_ALLOC(pool, A4_ATNEpsilonTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNEpsilonTransition) {
        .base = A4_ATNTransition_Init(A4_ATNTT_EPSILON, true, target),
        .outermost_precedence_return = 0xFFFF,
    };
    return transition;
}

A4_ATNRangeTransition* A4_NODISCARD A4_ATNRangeTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t from, uint16_t to) {
    A4_ATNRangeTransition* transition = A4_POOL_ALLOC(pool, A4_ATNRangeTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNRangeTransition) {
        .base = A4_ATNTransition_Init(A4_ATNTT_RANGE, false, target),
        .from = from,
        .to = to,
    };
    return transition;
}

A4_ATNRuleTransition* A4_NODISCARD A4_ATNRuleTransition_New(A4_MemoryPool* pool, A4_ATNRuleStartState* rule_start, uint16_t rule_index, uint16_t precedence, A4_ATNState* follow_state) {
    A4_ATNRuleTransition* transition = A4_POOL_ALLOC(pool, A4_ATNRuleTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNRuleTransition) {
        .base = A4_ATNTransition_Init(A4_ATNTT_RULE, true, (A4_ATNState*) rule_start),
        .rule_index = rule_index,
        .precedence = precedence,
        .follow_state = follow_state,
    };
    return transition;
}

A4_ATNPredicateTransition* A4_NODISCARD A4_ATNPredicateTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t rule_index, uint16_t pred_index, bool is_ctx_dependent) {
    A4_ATNPredicateTransition* transition = A4_POOL_ALLOC(pool, A4_ATNPredicateTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNPredicateTransition) {
        .base = A4_ATNAbstractPredicateTransition_Init(A4_ATNTT_PREDICATE, true, target),
        .rule_index = rule_index,
        .pred_index = pred_index,
        .is_ctx_dependent = is_ctx_dependent,
    };
    return transition;
}

A4_ATNPrecedencePredicateTransition* A4_NODISCARD A4_ATNPrecedencePredicateTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t precedence) {
    A4_ATNPrecedencePredicateTransition* transition = A4_POOL_ALLOC(pool, A4_ATNPrecedencePredicateTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNPrecedencePredicateTransition) {
        .base = A4_ATNAbstractPredicateTransition_Init(A4_ATNTT_PRECEDENCE, true, target),
        .precedence = precedence,
    };
    return transition;
}

A4_ATNAtomTransition* A4_NODISCARD A4_ATNAtomTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t label) {
    A4_ATNAtomTransition* transition = A4_POOL_ALLOC(pool, A4_ATNAtomTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNAtomTransition) {
        .base = A4_ATNTransition_Init(A4_ATNTT_ATOM, false, target),
        .label = label,
    };
    return transition;
}

A4_ATNActionTransition* A4_NODISCARD A4_ATNActionTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t rule_index, uint16_t action_index, bool is_ctx_dependent) {
    A4_ATNActionTransition* transition = A4_POOL_ALLOC(pool, A4_ATNActionTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNActionTransition) {
        .base = A4_ATNTransition_Init(A4_ATNTT_ACTION, true, target),
        .rule_index = rule_index,
        .action_index = action_index,
        .is_ctx_dependent = is_ctx_dependent,
    };
    return transition;
}

A4_ATNSetTransition* A4_NODISCARD A4_ATNSetTransition_New(A4_MemoryPool* pool, A4_ATNState* target) {
    A4_ATNSetTransition* transition = A4_POOL_ALLOC(pool, A4_ATNSetTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNSetTransition) {
        .base = A4_ATNTransition_Init(A4_ATNTT_SET, false, target),
    };
    return transition;
}

A4_ATNNotSetTransition* A4_NODISCARD A4_ATNNotSetTransition_New(A4_MemoryPool* pool, A4_ATNState* target) {
    A4_ATNNotSetTransition* transition = A4_POOL_ALLOC(pool, A4_ATNNotSetTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNNotSetTransition) {
        .base = A4_ATNSetTransition_Init(A4_ATNTT_NOT_SET, target),
    };
    return transition;
}

A4_ATNWildcardTransition* A4_NODISCARD A4_ATNWildcardTransition_New(A4_MemoryPool* pool, A4_ATNState* target) {
    A4_ATNWildcardTransition* transition = A4_POOL_ALLOC(pool, A4_ATNWildcardTransition);
    if (!transition) return NULL;
    *transition = (A4_ATNWildcardTransition) {
        .base = A4_ATNTransition_Init(A4_ATNTT_WILDCARD, false, target),
    };
    return transition;
}

