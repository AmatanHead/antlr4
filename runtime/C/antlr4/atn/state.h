#ifndef INC_929034E018364AA98B72E65E7B6FC478
#define INC_929034E018364AA98B72E65E7B6FC478

#include "../memory/pool.h"
#include "../util/bitset.h"
#include "../common.h"

#include "fwd.h"

#include <stdbool.h>


// ATN States
// --------------------------------------------------------------------------------------------------------------------

/// Type of ATN state.
a4_enum(A4_ATNStateType) {
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

    A4_ATNST_MAX_VAL = A4_ATNST_LOOP_END
};

/// Stringify type of ATN state.
const char* A4_ATNStateTypeName(A4_ATNStateType type);

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
    A4_ATNStateType type;

    /// Id of this state.
    uint16_t state_number;

    /// Id of production rul tied to this state.
    uint16_t rule_index;

    /// True if this state has only epsilon transitions.
    bool epsilon_only_transitions;

    /// Transitions from this state.
    struct {
        A4_ATNTransition** data;
        size_t size;
    } transitions;

    /// Used to cache lookahead during parsing, not used during construction.
    A4_BitSet* next_token_within_rule;
};

/// A4_ATNST_BASIC
struct A4_ATNBasicState {
    A4_ATNState base;
};

/// A4_ATNST_RULE_START
struct A4_ATNRuleStartState {
    A4_ATNState base;

    A4_ATNRuleStopState* stop_state;

    bool is_left_recursive_rule;
};

/// A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START,
/// A4_ATNST_TOKEN_START, A4_ATNST_STAR_LOOP_ENTRY, A4_ATNST_PLUS_LOOP_BACK
struct A4_ATNDecisionState {
    A4_ATNState base;

    uint16_t decision;

    bool non_greedy;
};

/// A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START
struct A4_ATNBlockStartStateBase {
    A4_ATNDecisionState base;

    A4_ATNBlockEndState* end_state;
};

/// A4_ATNST_BLOCK_START
struct A4_ATNBlockStartState {
    A4_ATNBlockStartStateBase base;
};

/// A4_ATNST_PLUS_BLOCK_START
struct A4_ATNPlusBlockStartState {
    A4_ATNBlockStartStateBase base;

    A4_ATNPlusLoopBackState* loopback_state;
};

/// A4_ATNST_STAR_BLOCK_START
struct A4_ATNStarBlockStartState {
    A4_ATNBlockStartStateBase base;
};

/// A4_ATNST_TOKEN_START
struct A4_ATNTokenStartState {
    A4_ATNDecisionState base;
};

/// A4_ATNST_STAR_LOOP_ENTRY
struct A4_ATNStarLoopEntryState {
    A4_ATNDecisionState base;

    A4_ATNStarLoopBackState* loopback_state;

    /// Indicates whether this state can benefit from a precedence DFA during SLL decision making.
    /// This is a computed property that is calculated during ATN deserialization.
    bool is_precedence_decision;
};

/// A4_ATNST_PLUS_LOOP_BACK
struct A4_ATNPlusLoopBackState {
    A4_ATNDecisionState base;
};

/// A4_ATNST_RULE_STOP
struct A4_ATNRuleStopState {
    A4_ATNState base;
};

/// A4_ATNST_BLOCK_END
struct A4_ATNBlockEndState {
    A4_ATNState base;

    A4_ATNBlockStartStateBase* start_state;
};

/// A4_ATNST_STAR_LOOP_BACK
struct A4_ATNStarLoopBackState {
    A4_ATNState base;
};

/// A4_ATNST_LOOP_END
struct A4_ATNLoopEndState {
    A4_ATNState base;

    A4_ATNState* loopback_state;
};

A4_DOWNCAST_FUNC(BasicState, A4_ATNState, A4_ATNBasicState, A4_ATNST_BASIC)
A4_DOWNCAST_FUNC(RuleStartState, A4_ATNState, A4_ATNRuleStartState, A4_ATNST_RULE_START)
A4_DOWNCAST_FUNC(DecisionState, A4_ATNState, A4_ATNDecisionState, A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START, A4_ATNST_TOKEN_START, A4_ATNST_STAR_LOOP_ENTRY, A4_ATNST_PLUS_LOOP_BACK)
A4_DOWNCAST_FUNC(BlockStartStateBase, A4_ATNState, A4_ATNBlockStartStateBase, A4_ATNST_BLOCK_START, A4_ATNST_PLUS_BLOCK_START, A4_ATNST_STAR_BLOCK_START)
A4_DOWNCAST_FUNC(BlockStartState, A4_ATNState, A4_ATNBlockStartState, A4_ATNST_BLOCK_START)
A4_DOWNCAST_FUNC(PlusBlockStartState, A4_ATNState, A4_ATNPlusBlockStartState, A4_ATNST_PLUS_BLOCK_START)
A4_DOWNCAST_FUNC(StarBlockStartState, A4_ATNState, A4_ATNStarBlockStartState, A4_ATNST_STAR_BLOCK_START)
A4_DOWNCAST_FUNC(TokenStartState, A4_ATNState, A4_ATNTokenStartState, A4_ATNST_TOKEN_START)
A4_DOWNCAST_FUNC(StarLoopEntryState, A4_ATNState, A4_ATNStarLoopEntryState, A4_ATNST_STAR_LOOP_ENTRY)
A4_DOWNCAST_FUNC(PlusLoopBackState, A4_ATNState, A4_ATNPlusLoopBackState, A4_ATNST_PLUS_LOOP_BACK)
A4_DOWNCAST_FUNC(RuleStopState, A4_ATNState, A4_ATNRuleStopState, A4_ATNST_RULE_STOP)
A4_DOWNCAST_FUNC(BlockEndState, A4_ATNState, A4_ATNBlockEndState, A4_ATNST_BLOCK_END)
A4_DOWNCAST_FUNC(StarLoopBackState, A4_ATNState, A4_ATNStarLoopBackState, A4_ATNST_STAR_LOOP_BACK)
A4_DOWNCAST_FUNC(LoopEndState, A4_ATNState, A4_ATNLoopEndState, A4_ATNST_LOOP_END)

/// Allocate new state based on its type. Returns NULL when trying to allocate A4_ATNST_INVALID state.
A4_ATNState* A4_NODISCARD A4_ATNState_New(A4_MemoryPool* pool, A4_ATNStateType type, uint16_t state_number, uint16_t rule_index);

A4_ATNBasicState* A4_NODISCARD A4_ATNBasicState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNRuleStartState* A4_NODISCARD A4_ATNRuleStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNBlockStartState* A4_NODISCARD A4_ATNBlockStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNPlusBlockStartState* A4_NODISCARD A4_ATNPlusBlockStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNStarBlockStartState* A4_NODISCARD A4_ATNStarBlockStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNTokenStartState* A4_NODISCARD A4_ATNTokenStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNStarLoopEntryState* A4_NODISCARD A4_ATNStarLoopEntryState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNPlusLoopBackState* A4_NODISCARD A4_ATNPlusLoopBackState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNRuleStopState* A4_NODISCARD A4_ATNRuleStopState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNBlockEndState* A4_NODISCARD A4_ATNBlockEndState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNStarLoopBackState* A4_NODISCARD A4_ATNStarLoopBackState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);
A4_ATNLoopEndState* A4_NODISCARD A4_ATNLoopEndState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index);

#endif /* UUID */
