#pragma once

#include "../memory/pool.h"

#include <assert.h>
#include <stdbool.h>


struct A4_ATNState;
struct A4_ATNTransition;


// ATN States
// --------------------------------------------------------------------------------------------------------------------

/// Type of ATN state.
enum A4_ATNStateType {
    A4_ATNST_INVALID = 0,
    A4_ATNST_BASIC = 1,
    A4_ATNST_RULE_START = 2,
    A4_ATNST_BLOCK_START = 3,
    A4_ATNST_PLUS_BLOCK_START = 4,
    A4_ATNST_STAR_BLOCK_START = 5,
    A4_ATNST_TOKEN_START = 6,
    A4_ATNST_RULE_STOP = 7,
    A4_ATNST_BLOCK_END = 8,
    A4_ATNST_STAR_LOOP_BACK = 9,
    A4_ATNST_STAR_LOOP_ENTRY = 10,
    A4_ATNST_PLUS_LOOP_BACK = 11,
    A4_ATNST_LOOP_END = 12,
};

/// Stringify type of ATN state.
const char* A4_ATNStateTypeName(enum A4_ATNStateType type);

/**
 *
 */
struct A4_ATNState {
    /// Type of this state.
    enum A4_ATNStateType type;

    /// Id of this state.
    int state_number;

    /// Id of production rul tied to this state.
    int rule_index;

    /// True if this state has only epsilon transitions.
    bool epsilon_only_transitions;

    /// Transitions from this state.
    struct {
        struct A4_ATNTransition** data;
        unsigned size;
    } transitions;

    /// Used to cache lookahead during parsing, not used during construction.
    // nextTokenWithinRule
};


// ATN Transitions
// --------------------------------------------------------------------------------------------------------------------

/// Type of ATN transition.
enum A4_ATNTransitionType {
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
};

/// Stringify type of ATN transition.
const char* A4_ATNTransitionTypeName(enum A4_ATNTransitionType type);

/**
 * An ATN transition between any two ATN states.
 *
 * This is a one way link. It emanates from a state (usually via a list of transitions) and has a target state.
 *
 * Since we never have to change the ATN transitions once we construct it, we can fix these transitions as specific
 * classes. The DFA transitions on the other hand need to update the labels as it adds transitions to
 * the states. We'll use the term Edge for the DFA to distinguish them from ATN transitions.
 */
struct A4_ATNTransition {
    /// Type of this transition.
    enum A4_ATNTransitionType type;

    /// This is an epsilon transition.
    bool is_epsilon;

    /// Target of this transition.
    struct A4_ATNState* target;
};

/// A4_ATNTT_EPSILON.
struct A4_ATNEpsilonTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_RANGE.
struct A4_ATNRangeTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_RULE.
struct A4_ATNRuleTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_PREDICATE.
struct A4_ATNPredicateTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_ATOM.
struct A4_ATNAtomTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_ACTION.
struct A4_ATNActionTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_SET and A4_ATNTT_NOT_SET transitions.
struct A4_ATNSetTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_PRECEDENCE.
struct A4_ATNPrecedenceTransition {
    struct A4_ATNTransition base;
};


// ATN
// --------------------------------------------------------------------------------------------------------------------

/// Represents the type of recognizer an ATN applies to.
enum A4_ATNType {
    /// A lexer grammar.
    A4_ATNT_LEXER,
    /// A parser grammar.
    A4_ATNT_PARSER,
};

/**
 *
 */
struct A4_ATN {
//    /// Pool that's used to allocate and manage ATN state.
//    struct A4_MemoryPool*   pool;

    /// Grammar type.
    enum A4_ATNType         grammar_type;
    /// The maximum value for any symbol recognized by a transition in the ATN.
    unsigned                max_token_type;
};
