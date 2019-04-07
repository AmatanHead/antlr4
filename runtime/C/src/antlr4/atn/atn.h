#ifndef INC_7E2FEC60F9D947ACA2424326E9166128
#define INC_7E2FEC60F9D947ACA2424326E9166128

#include "fwd.h"

#include <antlr4/util/bitset.h>
#include <antlr4/util/pool.h>

#include <antlr4/common.h>

#include <stdbool.h>


// ATN
// --------------------------------------------------------------------------------------------------------------------

/// Represents the type of recognizer an ATN applies to.
a4_enum(A4_ATNType) {
    /// A lexer grammar.
    A4_ATNT_LEXER = 0,

    /// A parser grammar.
    A4_ATNT_PARSER = 1,

    A4_ATNT_MAX_VAL = A4_ATNT_PARSER
};

/**
 * ATN (augmented transition network) is an internal representation of a parser.
 */
struct A4_ATN {
    /// Pool that's used to allocate and manage all objects in the ATN (including the ATN struct itself).
    A4_MemoryPool* pool;

    /// Grammar type.
    A4_ATNType grammar_type;

    /// The maximum value for any symbol recognized by a transition in the ATN.
    uint16_t max_token_type;

    /// List of ATN states.
    struct {
        A4_ATNState** data;
        size_t size;
    } states;

    /// Each subrule/rule is a decision point and we must track them so we can go back later and build DFA
    /// predictors for them. This includes all the rules, subrules, optional blocks, ()+, ()* etc...
    struct {
        A4_ATNDecisionState** data;
        size_t size;
    } decision_to_state;

    /// Maps from rule index to starting state number.
    A4_ATNRuleStartState** rule_to_start_state;

    /// Maps from rule index to stop state number.
    A4_ATNRuleStopState** rule_to_stop_state;

    /// For lexer ATNs, this maps the rule index to the resulting token type.
    /// For parser ATNs, this maps the rule index to the generated bypass token type if the rule bypass transitions
    /// are enabled, otherwise, this is NULL.
    uint16_t* rule_to_token_type;

    /// For lexer ATNs, this is an array of lexer action objects which may be referenced by action
    /// transitions in the ATN.
    struct {
        A4_LexerAction** data;
        size_t size;
    } lexer_actions;

    /// Maps mode index to mode start state.
    struct {
        A4_ATNTokenStartState** data;
        size_t size;
    } mode_to_start_state;
};

/// Load ATN from bytecode. If loading fails, pool might get polluted by the half-loaded ATN.
/// Generating ATN with bypass transitions is not supported at the moment.
A4_ERRNO A4_ATN_Load(A4_ATN** result, A4_MemoryPool* pool, const uint16_t* bytecode, size_t bytecode_size);

#endif /* UUID */