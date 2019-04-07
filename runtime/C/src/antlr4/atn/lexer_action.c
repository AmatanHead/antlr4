#include "lexer_action.h"

A4_LexerAction* A4_NODISCARD A4_LexerAction_New(A4_MemoryPool* pool, A4_LexerActionType type, uint16_t arg1, uint16_t arg2) {
    switch (type) {
        case A4_LAT_CHANNEL:
            return (A4_LexerAction*) A4_LexerChannelAction_New(pool, arg1);
        case A4_LAT_CUSTOM:
            return (A4_LexerAction*) A4_LexerCustomAction_New(pool, arg1, arg2);
        case A4_LAT_MODE:
            return (A4_LexerAction*) A4_LexerModeAction_New(pool, arg1);
        case A4_LAT_MORE:
            return (A4_LexerAction*) A4_LexerMoreAction_New(pool);
        case A4_LAT_POP_MODE:
            return (A4_LexerAction*) A4_LexerPopModeAction_New(pool);
        case A4_LAT_PUSH_MODE:
            return (A4_LexerAction*) A4_LexerPushModeAction_New(pool, arg1);
        case A4_LAT_SKIP:
            return (A4_LexerAction*) A4_LexerSkipAction_New(pool);
        case A4_LAT_TYPE:
            return (A4_LexerAction*) A4_LexerTypeAction_New(pool, arg1);
        case A4_LAT_INDEXED_CUSTOM:
            return NULL;
    }

    A4_UNREACHABLE();
}

A4_LexerAction A4_LexerAction_Init(A4_LexerActionType type) {
    return (A4_LexerAction) {
        .type = type,
    };
}

A4_LexerChannelAction* A4_NODISCARD A4_LexerChannelAction_New(A4_MemoryPool* pool, uint16_t channel) {
    A4_LexerChannelAction* action = A4_POOL_ALLOC(pool, A4_LexerChannelAction);
    if (!action) return NULL;
    *action = (A4_LexerChannelAction) {
        .base = A4_LexerAction_Init(A4_LAT_CHANNEL),
        .channel = channel,
    };
    return action;
}

A4_LexerCustomAction* A4_NODISCARD A4_LexerCustomAction_New(A4_MemoryPool* pool, uint16_t rule_index, uint16_t action_index) {
    A4_LexerCustomAction* action = A4_POOL_ALLOC(pool, A4_LexerCustomAction);
    if (!action) return NULL;
    *action = (A4_LexerCustomAction) {
        .base = A4_LexerAction_Init(A4_LAT_CUSTOM),
        .rule_index = rule_index,
        .action_index = action_index,
    };
    return action;
}

A4_LexerModeAction* A4_NODISCARD A4_LexerModeAction_New(A4_MemoryPool* pool, uint16_t mode) {
    A4_LexerModeAction* action = A4_POOL_ALLOC(pool, A4_LexerModeAction);
    if (!action) return NULL;
    *action = (A4_LexerModeAction) {
        .base = A4_LexerAction_Init(A4_LAT_MODE),
        .mode = mode,
    };
    return action;
}

A4_LexerMoreAction* A4_NODISCARD A4_LexerMoreAction_New(A4_MemoryPool* pool) {
    (void)pool;

    static A4_LexerMoreAction val = (A4_LexerMoreAction) {
        .base = {
            .type = A4_LAT_MORE,
        },
    };

    return &val;
}

A4_LexerPopModeAction* A4_NODISCARD A4_LexerPopModeAction_New(A4_MemoryPool* pool) {
    (void)pool;

    static A4_LexerPopModeAction val = (A4_LexerPopModeAction) {
        .base = {
            .type = A4_LAT_MORE,
        },
    };

    return &val;
}

A4_LexerPushModeAction* A4_NODISCARD A4_LexerPushModeAction_New(A4_MemoryPool* pool, uint16_t mode) {
    A4_LexerPushModeAction* action = A4_POOL_ALLOC(pool, A4_LexerPushModeAction);
    if (!action) return NULL;
    *action = (A4_LexerPushModeAction) {
        .base = A4_LexerAction_Init(A4_LAT_PUSH_MODE),
        .mode = mode,
    };
    return action;
}

A4_LexerSkipAction* A4_NODISCARD A4_LexerSkipAction_New(A4_MemoryPool* pool) {
    (void)pool;

    static A4_LexerSkipAction val = (A4_LexerSkipAction) {
        .base = {
            .type = A4_LAT_MORE,
        },
    };

    return &val;
}

A4_LexerTypeAction* A4_NODISCARD A4_LexerTypeAction_New(A4_MemoryPool* pool, uint16_t type) {
    A4_LexerTypeAction* action = A4_POOL_ALLOC(pool, A4_LexerTypeAction);
    if (!action) return NULL;
    *action = (A4_LexerTypeAction) {
        .base = A4_LexerAction_Init(A4_LAT_TYPE),
        .type = type,
    };
    return action;
}
