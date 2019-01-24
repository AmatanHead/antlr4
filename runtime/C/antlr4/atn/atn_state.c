#include "atn_state.h"

const char* A4_ATNStateTypeName(enum A4_ATNStateType type) {
    switch (type) {
        case A4_ATNST_INVALID:
            return "INVALID";
        case A4_ATNST_BASIC:
            return "BASIC";
        case A4_ATNST_RULE_START:
            return "RULE_START";
        case A4_ATNST_BLOCK_START:
            return "BLOCK_START";
        case A4_ATNST_PLUS_BLOCK_START:
            return "PLUS_BLOCK_START";
        case A4_ATNST_STAR_BLOCK_START:
            return "STAR_BLOCK_START";
        case A4_ATNST_TOKEN_START:
            return "TOKEN_START";
        case A4_ATNST_RULE_STOP:
            return "RULE_STOP";
        case A4_ATNST_BLOCK_END:
            return "BLOCK_END";
        case A4_ATNST_STAR_LOOP_BACK:
            return "STAR_LOOP_BACK";
        case A4_ATNST_STAR_LOOP_ENTRY:
            return "STAR_LOOP_ENTRY";
        case A4_ATNST_PLUS_LOOP_BACK:
            return "PLUS_LOOP_BACK";
        case A4_ATNST_LOOP_END:
            return "LOOP_END";
    }

    A4_UNREACHABLE();
}

A4_ATNState* A4_NODISCARD A4_ATNState_New(A4_MemoryPool* pool, A4_ATNStateType type, uint16_t state_number, uint16_t rule_index) {
    switch (type) {
        case A4_ATNST_INVALID:
            return NULL;
        case A4_ATNST_BASIC:
            return (A4_ATNState*) A4_ATNBasicState_New(pool, state_number, rule_index);
        case A4_ATNST_RULE_START:
            return (A4_ATNState*) A4_ATNRuleStartState_New(pool, state_number, rule_index);
        case A4_ATNST_BLOCK_START:
            return (A4_ATNState*) A4_ATNBlockStartState_New(pool, state_number, rule_index);
        case A4_ATNST_PLUS_BLOCK_START:
            return (A4_ATNState*) A4_ATNPlusBlockStartState_New(pool, state_number, rule_index);
        case A4_ATNST_STAR_BLOCK_START:
            return (A4_ATNState*) A4_ATNStarBlockStartState_New(pool, state_number, rule_index);
        case A4_ATNST_TOKEN_START:
            return (A4_ATNState*) A4_ATNTokenStartState_New(pool, state_number, rule_index);
        case A4_ATNST_STAR_LOOP_ENTRY:
            return (A4_ATNState*) A4_ATNStarLoopEntryState_New(pool, state_number, rule_index);
        case A4_ATNST_PLUS_LOOP_BACK:
            return (A4_ATNState*) A4_ATNPlusLoopBackState_New(pool, state_number, rule_index);
        case A4_ATNST_RULE_STOP:
            return (A4_ATNState*) A4_ATNRuleStopState_New(pool, state_number, rule_index);
        case A4_ATNST_BLOCK_END:
            return (A4_ATNState*) A4_ATNBlockEndState_New(pool, state_number, rule_index);
        case A4_ATNST_STAR_LOOP_BACK:
            return (A4_ATNState*) A4_ATNStarLoopBackState_New(pool, state_number, rule_index);
        case A4_ATNST_LOOP_END:
            return (A4_ATNState*) A4_ATNLoopEndState_New(pool, state_number, rule_index);
    }

    A4_UNREACHABLE();
}


static inline A4_ATNState A4_NODISCARD A4_ATNState_Init(A4_ATNStateType type, uint16_t state_number, uint16_t rule_index) {
    return (A4_ATNState) {
        .type = type,
        .state_number = state_number,
        .rule_index = rule_index,
        .epsilon_only_transitions = false,
        .transitions = {NULL, 0},
        .next_token_within_rule = NULL,
    };
}

static inline A4_ATNDecisionState A4_NODISCARD A4_ATNDecisionState_Init(A4_ATNStateType type, uint16_t state_number, uint16_t rule_index) {
    return (A4_ATNDecisionState) {
        .base = A4_ATNState_Init(type, state_number, rule_index),
        .decision = 0xFFFF,
        .non_greedy = false,
    };
}

static inline A4_ATNBlockStartStateBase A4_NODISCARD A4_ATNBlockStartStateBase_Init(A4_ATNStateType type, uint16_t state_number, uint16_t rule_index) {
    return (A4_ATNBlockStartStateBase) {
        .base = A4_ATNDecisionState_Init(type, state_number, rule_index),
        .end_state = NULL,
    };
}


A4_ATNBasicState* A4_NODISCARD A4_ATNBasicState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNBasicState* state = A4_POOL_ALLOC(pool, A4_ATNBasicState);
    if (!state) return NULL;

    *state = (A4_ATNBasicState) {
        .base = A4_ATNState_Init(A4_ATNST_BASIC, state_number, rule_index),
    };

    return state;
}

A4_ATNRuleStartState* A4_NODISCARD A4_ATNRuleStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNRuleStartState* state = A4_POOL_ALLOC(pool, A4_ATNRuleStartState);
    if (!state) return NULL;

    *state = (A4_ATNRuleStartState) {
        .base = A4_ATNState_Init(A4_ATNST_RULE_START, state_number, rule_index),
        .stop_state = NULL,
        .is_left_recursive_rule = false,
    };

    return state;
}

A4_ATNBlockStartState* A4_NODISCARD A4_ATNBlockStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNBlockStartState* state = A4_POOL_ALLOC(pool, A4_ATNBlockStartState);
    if (!state) return NULL;

    *state = (A4_ATNBlockStartState) {
        .base = A4_ATNBlockStartStateBase_Init(A4_ATNST_BLOCK_START, state_number, rule_index),
    };

    return state;
}

A4_ATNPlusBlockStartState* A4_NODISCARD A4_ATNPlusBlockStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNPlusBlockStartState* state = A4_POOL_ALLOC(pool, A4_ATNPlusBlockStartState);
    if (!state) return NULL;

    *state = (A4_ATNPlusBlockStartState) {
        .base = A4_ATNBlockStartStateBase_Init(A4_ATNST_PLUS_BLOCK_START, state_number, rule_index),
        .loopback_state = NULL,
    };

    return state;
}

A4_ATNStarBlockStartState* A4_NODISCARD A4_ATNStarBlockStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNStarBlockStartState* state = A4_POOL_ALLOC(pool, A4_ATNStarBlockStartState);
    if (!state) return NULL;

    *state = (A4_ATNStarBlockStartState) {
        .base = A4_ATNBlockStartStateBase_Init(A4_ATNST_STAR_BLOCK_START, state_number, rule_index),
    };

    return state;
}

A4_ATNTokenStartState* A4_NODISCARD A4_ATNTokenStartState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNTokenStartState* state = A4_POOL_ALLOC(pool, A4_ATNTokenStartState);
    if (!state) return NULL;

    *state = (A4_ATNTokenStartState) {
        .base = A4_ATNDecisionState_Init(A4_ATNST_TOKEN_START, state_number, rule_index),
    };

    return state;
}

A4_ATNStarLoopEntryState* A4_NODISCARD A4_ATNStarLoopEntryState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNStarLoopEntryState* state = A4_POOL_ALLOC(pool, A4_ATNStarLoopEntryState);
    if (!state) return NULL;

    *state = (A4_ATNStarLoopEntryState) {
        .base = A4_ATNDecisionState_Init(A4_ATNST_STAR_LOOP_ENTRY, state_number, rule_index),
        .is_precedence_decision = false,
        .loopback_state = NULL,
    };

    return state;
}

A4_ATNPlusLoopBackState* A4_NODISCARD A4_ATNPlusLoopBackState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNPlusLoopBackState* state = A4_POOL_ALLOC(pool, A4_ATNPlusLoopBackState);
    if (!state) return NULL;

    *state = (A4_ATNPlusLoopBackState) {
        .base = A4_ATNDecisionState_Init(A4_ATNST_PLUS_LOOP_BACK, state_number, rule_index),
    };

    return state;
}

A4_ATNRuleStopState* A4_NODISCARD A4_ATNRuleStopState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNRuleStopState* state = A4_POOL_ALLOC(pool, A4_ATNRuleStopState);
    if (!state) return NULL;

    *state = (A4_ATNRuleStopState) {
        .base = A4_ATNState_Init(A4_ATNST_RULE_STOP, state_number, rule_index),
    };

    return state;
}

A4_ATNBlockEndState* A4_NODISCARD A4_ATNBlockEndState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNBlockEndState* state = A4_POOL_ALLOC(pool, A4_ATNBlockEndState);
    if (!state) return NULL;

    *state = (A4_ATNBlockEndState) {
        .base = A4_ATNState_Init(A4_ATNST_BLOCK_END, state_number, rule_index),
        .start_state = NULL,
    };

    return state;
}

A4_ATNStarLoopBackState* A4_NODISCARD A4_ATNStarLoopBackState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNStarLoopBackState* state = A4_POOL_ALLOC(pool, A4_ATNStarLoopBackState);
    if (!state) return NULL;

    *state = (A4_ATNStarLoopBackState) {
        .base = A4_ATNState_Init(A4_ATNST_STAR_LOOP_BACK, state_number, rule_index),
    };

    return state;
}

A4_ATNLoopEndState* A4_NODISCARD A4_ATNLoopEndState_New(A4_MemoryPool* pool, uint16_t state_number, uint16_t rule_index) {
    A4_ATNLoopEndState* state = A4_POOL_ALLOC(pool, A4_ATNLoopEndState);
    if (!state) return NULL;

    *state = (A4_ATNLoopEndState) {
        .base = A4_ATNState_Init(A4_ATNST_LOOP_END, state_number, rule_index),
        .loopback_state = NULL,
    };

    return state;
}
