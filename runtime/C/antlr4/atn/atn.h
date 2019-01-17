#pragma once

#include "../memory/pool.h"
#include "../util/bitset.h"
#include "../util/intset.h"
#include "../util/one_of.h"
#include "../common.h"

#include <assert.h>
#include <stdbool.h>


struct A4_ATNState;
struct A4_ATNBasicState;
struct A4_ATNRuleStartState;
struct A4_ATNDecisionState;
struct A4_ATNBlockStartStateBase;
struct A4_ATNBlockStartState;
struct A4_ATNPlusBlockStartState;
struct A4_ATNStarBlockStartState;
struct A4_ATNTokenStartState;
struct A4_ATNStarLoopEntryState;
struct A4_ATNPlusLoopBackState;
struct A4_ATNRuleStopState;
struct A4_ATNBlockEndState;
struct A4_ATNStarLoopBackState;
struct A4_ATNLoopEndState;

struct A4_ATNTransition;
struct A4_ATNTransition;
struct A4_ATNEpsilonTransition;
struct A4_ATNRangeTransition;
struct A4_ATNRuleTransition;
struct A4_ATNAbstractPredicateTransition;
struct A4_ATNPredicateTransition;
struct A4_ATNPrecedencePredicateTransition;
struct A4_ATNAtomTransition;
struct A4_ATNActionTransition;
struct A4_ATNSetTransition;
struct A4_ATNNotSetTransition;
struct A4_ATNWildcardTransition;


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
 * Single state of an ATN.
 *
 * Struct hierarchy:
 *
 * A4_ATNState
 *  │
 *  ├──A4_ATNBasicState
 *  │
 *  ├──A4_ATNRuleStartState
 *  │
 *  ├──A4_ATNDecisionState
 *  │   │
 *  │   ├──A4_ATNBlockStartStateBase
 *  │   │   │
 *  │   │   ├──A4_ATNBlockStartState
 *  │   │   │
 *  │   │   ├──A4_ATNPlusBlockStartState
 *  │   │   │
 *  │   │   └──A4_ATNStarBlockStartState
 *  │   │
 *  │   ├──A4_ATNTokenStartState
 *  │   │
 *  │   ├──A4_ATNStarLoopEntryState
 *  │   │
 *  │   └──A4_ATNPlusLoopBackState
 *  │
 *  ├── A4_ATNRuleStopState
 *  │
 *  ├──A4_ATNBlockEndState
 *  │
 *  ├──A4_ATNStarLoopBackState
 *  │
 *  └──A4_ATNLoopEndState
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
        size_t size;
    } transitions;

    /// Used to cache lookahead during parsing, not used during construction.
    struct A4_BitSet* next_token_within_rule;
};

/// A4_ATNST_BASIC
struct A4_ATNBasicState {
    struct A4_ATNState base;
};

/// A4_ATNST_RULE_START
struct A4_ATNRuleStartState {
    struct A4_ATNState base;

    struct A4_ATNRuleStopState* stop_state;

    bool is_left_recursive_rule;
};

/// A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START,
/// A4_ATNST_TOKEN_START, A4_ATNST_STAR_LOOP_ENTRY, A4_ATNST_PLUS_LOOP_BACK
struct A4_ATNDecisionState {
    struct A4_ATNState base;

    int decision;

    bool non_greedy;
};

/// A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START
struct A4_ATNBlockStartStateBase {
    struct A4_ATNDecisionState base;

    struct A4_ATNBlockEndState* block_end;
};

/// A4_ATNST_BLOCK_START
struct A4_ATNBlockStartState {
    struct A4_ATNBlockStartStateBase base;
};

/// A4_ATNST_PLUS_BLOCK_START
struct A4_ATNPlusBlockStartState {
    struct A4_ATNBlockStartStateBase base;

    struct A4_ATNPlusLoopBackState* loopback_state;
};

/// A4_ATNST_STAR_BLOCK_START
struct A4_ATNStarBlockStartState {
    struct A4_ATNBlockStartStateBase base;
};

/// A4_ATNST_TOKEN_START
struct A4_ATNTokenStartState {
    struct A4_ATNDecisionState base;
};

/// A4_ATNST_STAR_LOOP_ENTRY
struct A4_ATNStarLoopEntryState {
    struct A4_ATNDecisionState base;

    struct A4_ATNStarLoopBackState* loopback_state;

    /// Indicates whether this state can benefit from a precedence DFA during SLL decision making.
    /// This is a computed property that is calculated during ATN deserialization.
    bool is_precedence_decision;
};

/// A4_ATNST_PLUS_LOOP_BACK
struct A4_ATNPlusLoopBackState {
    struct A4_ATNDecisionState base;
};

/// A4_ATNST_RULE_STOP
struct A4_ATNRuleStopState {
    struct A4_ATNState base;
};

/// A4_ATNST_BLOCK_END
struct A4_ATNBlockEndState {
    struct A4_ATNState base;
    struct A4_ATNBlockStartStateBase* start_state;
};

/// A4_ATNST_STAR_LOOP_BACK
struct A4_ATNStarLoopBackState {
    struct A4_ATNState base;
};

/// A4_ATNST_LOOP_END
struct A4_ATNLoopEndState {
    struct A4_ATNState base;

    struct A4_ATNState* loopback_state;
};

A4_DOWNCAST_FUNC(A4_ToBasicState, A4_ATNState, A4_ATNBasicState, A4_ATNST_BASIC)
A4_DOWNCAST_FUNC(A4_ToRuleStartState, A4_ATNState, A4_ATNRuleStartState, A4_ATNST_RULE_START)
A4_DOWNCAST_FUNC(A4_ToDecisionState, A4_ATNState, A4_ATNDecisionState, A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START, A4_ATNST_TOKEN_START, A4_ATNST_STAR_LOOP_ENTRY, A4_ATNST_PLUS_LOOP_BACK)
A4_DOWNCAST_FUNC(A4_ToBlockStartBaseState, A4_ATNState, A4_ATNBlockStartBaseState, A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START)
A4_DOWNCAST_FUNC(A4_ToBlockStartState, A4_ATNState, A4_ATNBlockStartState, A4_ATNST_BLOCK_START)
A4_DOWNCAST_FUNC(A4_ToPlusBlockStartState, A4_ATNState, A4_ATNPlusBlockStartState, A4_ATNST_PLUS_BLOCK_START)
A4_DOWNCAST_FUNC(A4_ToStarBlockStartState, A4_ATNState, A4_ATNStarBlockStartState, A4_ATNST_STAR_BLOCK_START)
A4_DOWNCAST_FUNC(A4_ToTokenStartState, A4_ATNState, A4_ATNTokenStartState, A4_ATNST_TOKEN_START)
A4_DOWNCAST_FUNC(A4_ToStarLoopEntryState, A4_ATNState, A4_ATNStarLoopEntryState, A4_ATNST_STAR_LOOP_ENTRY)
A4_DOWNCAST_FUNC(A4_ToPlusLoopBackState, A4_ATNState, A4_ATNPlusLoopBackState, A4_ATNST_PLUS_LOOP_BACK)
A4_DOWNCAST_FUNC(A4_ToRuleStopState, A4_ATNState, A4_ATNRuleStopState, A4_ATNST_RULE_STOP)
A4_DOWNCAST_FUNC(A4_ToBlockEndState, A4_ATNState, A4_ATNBlockEndState, A4_ATNST_BLOCK_END)
A4_DOWNCAST_FUNC(A4_ToStarLoopBackState, A4_ATNState, A4_ATNStarLoopBackState, A4_ATNST_STAR_LOOP_BACK)
A4_DOWNCAST_FUNC(A4_ToLoopEndState, A4_ATNState, A4_ATNLoopEndState, A4_ATNST_LOOP_END)


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
    enum A4_ATNTransitionType type;

    /// This is an epsilon transition.
    bool is_epsilon;

    /// Target of this transition.
    struct A4_ATNState* target;
};

/// A4_ATNTT_EPSILON
struct A4_ATNEpsilonTransition {
    struct A4_ATNTransition base;

    /// The rule index of a precedence rule for which this transition is returning from,
    /// where the precedence value is 0; otherwise, -1.
    int outermost_precedence_return;
};

/// A4_ATNTT_RANGE
struct A4_ATNRangeTransition {
    struct A4_ATNTransition base;

    int from;

    int to;
};

/// A4_ATNTT_RULE
struct A4_ATNRuleTransition {
    struct A4_ATNTransition base;

    /// What node to begin computations following ref to rule.
    struct A4_ATNState* follow_state;

    /// Ptr to the rule definition object for this rule ref.
    int rule_index;

    int precedence;
};

/// A4_ATNTT_PREDICATE, A4_ATNTT_PRECEDENCE
struct A4_ATNAbstractPredicateTransition {
    struct A4_ATNTransition base;
};

/// A4_ATNTT_PREDICATE
struct A4_ATNPredicateTransition {
    struct A4_ATNAbstractPredicateTransition base;

    int rule_index;

    int pred_index;

    bool is_ctx_dependent;
};

/// A4_ATNTT_PRECEDENCE
struct A4_ATNPrecedencePredicateTransition {
    struct A4_ATNAbstractPredicateTransition base;

    int precedence;
};

/// A4_ATNTT_ATOM
struct A4_ATNAtomTransition {
    struct A4_ATNTransition base;

    /// The token type or character value; or, signifies special label.
    int label;
};

/// A4_ATNTT_ACTION
struct A4_ATNActionTransition {
    struct A4_ATNTransition base;

    int rule_index;

    int action_index;

    bool is_ctx_dependent;
};

/// A4_ATNTT_SET, A4_ATNTT_NOT_SET
struct A4_ATNSetTransition {
    struct A4_ATNTransition base;

    struct A4_IntSet* set;
};

/// A4_ATNTT_NOT_SET
struct A4_ATNNotSetTransition {
    struct A4_ATNSetTransition base;
};

/// A4_ATNTT_WILDCARD
struct A4_ATNWildcardTransition {
    struct A4_ATNTransition base;
};

A4_DOWNCAST_FUNC(A4_ToEpsilonTransition, A4_ATNTransition, A4_ATNEpsilonTransition, A4_ATNTT_EPSILON)
A4_DOWNCAST_FUNC(A4_ToRangeTransition, A4_ATNTransition, A4_ATNRangeTransition, A4_ATNTT_RANGE)
A4_DOWNCAST_FUNC(A4_ToRuleTransition, A4_ATNTransition, A4_ATNRuleTransition, A4_ATNTT_RULE)
A4_DOWNCAST_FUNC(A4_ToAbstractPredicateTransition, A4_ATNTransition, A4_ATNAbstractPredicateTransition, A4_ATNTT_PREDICATE, A4_ATNTT_PRECEDENCE)
A4_DOWNCAST_FUNC(A4_ToPredicateTransition, A4_ATNTransition, A4_ATNPredicateTransition, A4_ATNTT_PREDICATE)
A4_DOWNCAST_FUNC(A4_ToPrecedencePredicateTransition, A4_ATNTransition, A4_ATNPrecedencePredicateTransition, A4_ATNTT_PRECEDENCE)
A4_DOWNCAST_FUNC(A4_ToAtomTransition, A4_ATNTransition, A4_ATNAtomTransition, A4_ATNTT_ATOM)
A4_DOWNCAST_FUNC(A4_ToActionTransition, A4_ATNTransition, A4_ATNActionTransition, A4_ATNTT_ACTION)
A4_DOWNCAST_FUNC(A4_ToSetTransition, A4_ATNTransition, A4_ATNSetTransition, A4_ATNTT_SET, A4_ATNTT_NOT_SET)
A4_DOWNCAST_FUNC(A4_ToNotSetTransition, A4_ATNTransition, A4_ATNNotSetTransition, A4_ATNTT_NOT_SET)
A4_DOWNCAST_FUNC(A4_ToWildcardTransition, A4_ATNTransition, A4_ATNWildcardTransition, A4_ATNTT_WILDCARD)


// Lexer actions
// --------------------------------------------------------------------------------------------------------------------

///  Represents the serialization type of a lexer action.
enum A4_LexerActionType {
    A4_LAT_INVALID = 0,
    A4_LAT_CHANNEL = 1,
    A4_LAT_CUSTOM = 2,
    A4_LAT_MODE = 3,
    A4_LAT_MORE = 4,
    A4_LAT_POP_MODE = 5,
    A4_LAT_PUSH_MODE = 6,
    A4_LAT_SKIP = 7,
    A4_LAT_TYPE = 8,

    A4_LAT_INDEXED_CUSTOM = -1,  // Virtual action, not serialized as part of the ATN.
};

/// Represents a single action which can be executed following the successful match of a lexer rule.
/// Lexer actions are used for both embedded action syntax and ANTLR 4's new lexer command syntax.
struct A4_LexerAction {
    enum A4_LexerActionType type;
};

/// A4_LAT_CHANNEL
struct A4_LexerChannelAction {
    struct A4_LexerAction base;

    int channel;
};

/// A4_LAT_CUSTOM
struct A4_LexerCustomAction {
    struct A4_LexerAction base;

    int rule_index;

    int action_index;
};

/// A4_LAT_MODE
struct A4_LexerModeAction {
    struct A4_LexerAction base;

    int mode;
};

/// A4_LAT_MORE
struct A4_LexerMoreAction {
    struct A4_LexerAction base;
};

/// A4_LAT_POP_MODE
struct A4_LexerPopModeAction {
    struct A4_LexerAction base;
};

/// A4_LAT_PUSH_MODE
struct A4_LexerPushModeAction {
    struct A4_LexerAction base;

    int mode;
};

/// A4_LAT_SKIP
struct A4_LexerSkipAction {
    struct A4_LexerAction base;
};

/// A4_LAT_TYPE
struct A4_LexerTypeAction {
    struct A4_LexerAction base;

    int type;
};

/// A4_LAT_INDEXED_CUSTOM
struct A4_LexerIndexedCustomAction {
    struct A4_LexerAction base;

    int offset;

    struct A4_LexerAction* action;
};

A4_DOWNCAST_FUNC(A4_ToLexerChannelAction, A4_LexerAction, A4_LexerChannelAction, A4_LAT_CHANNEL)
A4_DOWNCAST_FUNC(A4_ToLexerCustomAction, A4_LexerAction, A4_LexerCustomAction, A4_LAT_CUSTOM)
A4_DOWNCAST_FUNC(A4_ToLexerModeAction, A4_LexerAction, A4_LexerModeAction, A4_LAT_MODE)
A4_DOWNCAST_FUNC(A4_ToLexerMoreAction, A4_LexerAction, A4_LexerMoreAction, A4_LAT_MORE)
A4_DOWNCAST_FUNC(A4_ToLexerPopModeAction, A4_LexerAction, A4_LexerPopModeAction, A4_LAT_POP_MODE)
A4_DOWNCAST_FUNC(A4_ToLexerPushModeAction, A4_LexerAction, A4_LexerPushModeAction, A4_LAT_PUSH_MODE)
A4_DOWNCAST_FUNC(A4_ToLexerSkipAction, A4_LexerAction, A4_LexerSkipAction, A4_LAT_SKIP)
A4_DOWNCAST_FUNC(A4_ToLexerTypeAction, A4_LexerAction, A4_LexerTypeAction, A4_LAT_TYPE)
A4_DOWNCAST_FUNC(A4_ToLexerIndexedCustomAction, A4_LexerAction, A4_LexerIndexedCustomAction, A4_LAT_INDEXED_CUSTOM)


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
