#ifndef INC_4441CB085EC64BED92745F1AD45E93A1
#define INC_4441CB085EC64BED92745F1AD45E93A1

#include "fwd.h"

#include <antlr4/util/intset.h>

#include <antlr4/common.h>

#include <stdbool.h>


// ATN Transitions
// --------------------------------------------------------------------------------------------------------------------

/// Type of ATN transition.
a4_enum(A4_ATNTransitionType) {
    A4_ATNTT_INVALID = 0,
    A4_ATNTT_EPSILON = 1,
    A4_ATNTT_RANGE = 2,
    A4_ATNTT_RULE = 3,
    A4_ATNTT_PREDICATE = 4,
    A4_ATNTT_ATOM = 5,
    A4_ATNTT_ACTION = 6,
    A4_ATNTT_SET = 7,
    A4_ATNTT_NOT_SET = 8,
    A4_ATNTT_WILDCARD = 9,
    A4_ATNTT_PRECEDENCE = 10,

    A4_ATNTT_MAX_VAL = A4_ATNTT_PRECEDENCE
};

/// Stringify type of ATN transition.
const char* A4_ATNTransitionTypeName(A4_ATNTransitionType type);

/**
 * An ATN transition between any two ATN states.
 *
 * This is a one way link. It emanates from a state (usually via a list of transitions) and has a target state.
 *
 * Since we never have to change the ATN transitions once we construct it, we can fix these transitions as specific
 * classes. The DFA transitions on the other hand need to update the labels as it adds transitions to
 * the states. We'll use the term Edge for the DFA to distinguish them from ATN transitions.
 *
 * Struct hierarchy:
 *
 * A4_ATNTransition
 *  │
 *  ├──A4_ATNEpsilonTransition
 *  │
 *  ├──A4_ATNRangeTransition
 *  │
 *  ├──A4_ATNRuleTransition
 *  │
 *  ├──A4_ATNAbstractPredicateTransition
 *  │   │
 *  │   ├──A4_ATNPredicateTransition
 *  │   │
 *  │   └──A4_ATNPrecedencePredicateTransition
 *  │
 *  ├──A4_ATNAtomTransition
 *  │
 *  ├──A4_ATNActionTransition
 *  │
 *  ├──A4_ATNSetTransition
 *  │   │
 *  │   └──A4_ATNNotSetTransition
 *  │
 *  └──A4_ATNWildcardTransition
 */
struct A4_ATNTransition {
    /// Type of this transition.
    A4_ATNTransitionType type;

    /// This is an epsilon transition.
    bool is_epsilon;

    /// Target of this transition.
    A4_ATNState* target;
};

/// A4_ATNTT_EPSILON
struct A4_ATNEpsilonTransition {
    A4_ATNTransition base;

    /// The rule index of a precedence rule for which this transition is returning from,
    /// where the precedence value is 0; otherwise, 0xFFFF.
    uint16_t outermost_precedence_return;
};

/// A4_ATNTT_RANGE
struct A4_ATNRangeTransition {
    A4_ATNTransition base;

    uint16_t from;

    uint16_t to;
};

/// A4_ATNTT_RULE
struct A4_ATNRuleTransition {
    A4_ATNTransition base;

    /// What node to begin computations following ref to rule.
    A4_ATNState* follow_state;

    /// Ptr to the rule definition object for this rule ref.
    uint16_t rule_index;

    uint16_t precedence;
};

/// A4_ATNTT_PREDICATE, A4_ATNTT_PRECEDENCE
struct A4_ATNAbstractPredicateTransition {
    A4_ATNTransition base;
};

/// A4_ATNTT_PREDICATE
struct A4_ATNPredicateTransition {
    A4_ATNAbstractPredicateTransition base;

    int rule_index;

    int pred_index;

    bool is_ctx_dependent;
};

/// A4_ATNTT_PRECEDENCE
struct A4_ATNPrecedencePredicateTransition {
    A4_ATNAbstractPredicateTransition base;

    uint16_t precedence;
};

/// A4_ATNTT_ATOM
struct A4_ATNAtomTransition {
    A4_ATNTransition base;

    /// The token type or character value; or, signifies special label.
    uint16_t label;
};

/// A4_ATNTT_ACTION
struct A4_ATNActionTransition {
    A4_ATNTransition base;

    uint16_t rule_index;

    uint16_t action_index;

    bool is_ctx_dependent;
};

/// A4_ATNTT_SET, A4_ATNTT_NOT_SET
struct A4_ATNSetTransition {
    A4_ATNTransition base;

    A4_IntSet* set;
};

/// A4_ATNTT_NOT_SET
struct A4_ATNNotSetTransition {
    A4_ATNSetTransition base;
};

/// A4_ATNTT_WILDCARD
struct A4_ATNWildcardTransition {
    A4_ATNTransition base;
};

A4_DOWNCAST_FUNC(EpsilonTransition, A4_ATNTransition, A4_ATNEpsilonTransition, A4_ATNTT_EPSILON)
A4_DOWNCAST_FUNC(RangeTransition, A4_ATNTransition, A4_ATNRangeTransition, A4_ATNTT_RANGE)
A4_DOWNCAST_FUNC(RuleTransition, A4_ATNTransition, A4_ATNRuleTransition, A4_ATNTT_RULE)
A4_DOWNCAST_FUNC(AbstractPredicateTransition, A4_ATNTransition, A4_ATNAbstractPredicateTransition, A4_ATNTT_PREDICATE, A4_ATNTT_PRECEDENCE)
A4_DOWNCAST_FUNC(PredicateTransition, A4_ATNTransition, A4_ATNPredicateTransition, A4_ATNTT_PREDICATE)
A4_DOWNCAST_FUNC(PrecedencePredicateTransition, A4_ATNTransition, A4_ATNPrecedencePredicateTransition, A4_ATNTT_PRECEDENCE)
A4_DOWNCAST_FUNC(AtomTransition, A4_ATNTransition, A4_ATNAtomTransition, A4_ATNTT_ATOM)
A4_DOWNCAST_FUNC(ActionTransition, A4_ATNTransition, A4_ATNActionTransition, A4_ATNTT_ACTION)
A4_DOWNCAST_FUNC(SetTransition, A4_ATNTransition, A4_ATNSetTransition, A4_ATNTT_SET, A4_ATNTT_NOT_SET)
A4_DOWNCAST_FUNC(NotSetTransition, A4_ATNTransition, A4_ATNNotSetTransition, A4_ATNTT_NOT_SET)
A4_DOWNCAST_FUNC(WildcardTransition, A4_ATNTransition, A4_ATNWildcardTransition, A4_ATNTT_WILDCARD)

/// Allocate new transition based on its type. Returns NULL when trying to allocate A4_ATNST_INVALID state.
A4_ATNTransition* A4_NODISCARD A4_ATNTransition_New(A4_MemoryPool* pool, A4_ATNTransitionType type,
    A4_ATN* atn, A4_ATNState* target, uint16_t arg1, uint16_t arg2, uint16_t arg3);

A4_ATNEpsilonTransition* A4_NODISCARD A4_ATNEpsilonTransition_New(A4_MemoryPool* pool, A4_ATNState* target);
A4_ATNRangeTransition* A4_NODISCARD A4_ATNRangeTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t from, uint16_t to);
A4_ATNRuleTransition* A4_NODISCARD A4_ATNRuleTransition_New(A4_MemoryPool* pool, A4_ATNRuleStartState* rule_start, uint16_t rule_index, uint16_t precedence, A4_ATNState* follow_state);
A4_ATNPredicateTransition* A4_NODISCARD A4_ATNPredicateTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t rule_index, uint16_t pred_index, bool is_ctx_dependent);
A4_ATNPrecedencePredicateTransition* A4_NODISCARD A4_ATNPrecedencePredicateTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t precedence);
A4_ATNAtomTransition* A4_NODISCARD A4_ATNAtomTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t label);
A4_ATNActionTransition* A4_NODISCARD A4_ATNActionTransition_New(A4_MemoryPool* pool, A4_ATNState* target, uint16_t rule_index, uint16_t action_index, bool is_ctx_dependent);
A4_ATNSetTransition* A4_NODISCARD A4_ATNSetTransition_New(A4_MemoryPool* pool, A4_ATNState* target);
A4_ATNNotSetTransition* A4_NODISCARD A4_ATNNotSetTransition_New(A4_MemoryPool* pool, A4_ATNState* target);
A4_ATNWildcardTransition* A4_NODISCARD A4_ATNWildcardTransition_New(A4_MemoryPool* pool, A4_ATNState* target);

#endif /* UUID */
