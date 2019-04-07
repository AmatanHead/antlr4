#ifndef INC_DEFBCB5369044410A590B4936E16496E
#define INC_DEFBCB5369044410A590B4936E16496E

#include "fwd.h"

#include <antlr4/util/pool.h>

#include <antlr4/common.h>

#include <stdbool.h>


// Lexer actions
// --------------------------------------------------------------------------------------------------------------------

///  Represents the serialization type of a lexer action.
a4_enum(A4_LexerActionType) {
    A4_LAT_CHANNEL = 0,
    A4_LAT_CUSTOM = 1,
    A4_LAT_MODE = 2,
    A4_LAT_MORE = 3,
    A4_LAT_POP_MODE = 4,
    A4_LAT_PUSH_MODE = 5,
    A4_LAT_SKIP = 6,
    A4_LAT_TYPE = 7,

    A4_LAT_INDEXED_CUSTOM = -1,  // Virtual action, not serialized as part of the ATN.

    A4_LAT_MAX_VAL = A4_LAT_TYPE
};

/**
 * Represents a single action which can be executed following the successful match of a lexer rule.
 * Lexer actions are used for both embedded action syntax and ANTLR 4's new lexer command syntax.
 */
struct A4_LexerAction {
    A4_LexerActionType type;
};

/// A4_LAT_CHANNEL
struct A4_LexerChannelAction {
    A4_LexerAction base;

    uint16_t channel;
};

/// A4_LAT_CUSTOM
struct A4_LexerCustomAction {
    A4_LexerAction base;

    uint16_t rule_index;

    uint16_t action_index;
};

/// A4_LAT_MODE
struct A4_LexerModeAction {
    A4_LexerAction base;

    uint16_t mode;
};

/// A4_LAT_MORE
struct A4_LexerMoreAction {
    A4_LexerAction base;
};

/// A4_LAT_POP_MODE
struct A4_LexerPopModeAction {
    A4_LexerAction base;
};

/// A4_LAT_PUSH_MODE
struct A4_LexerPushModeAction {
    A4_LexerAction base;

    uint16_t mode;
};

/// A4_LAT_SKIP
struct A4_LexerSkipAction {
    A4_LexerAction base;
};

/// A4_LAT_TYPE
struct A4_LexerTypeAction {
    A4_LexerAction base;

    uint16_t type;
};

/// A4_LAT_INDEXED_CUSTOM
struct A4_LexerIndexedCustomAction {
    A4_LexerAction base;

    uint16_t offset;

    A4_LexerAction* action;
};

A4_DOWNCAST_FUNC(LexerChannelAction, A4_LexerAction, A4_LexerChannelAction, A4_LAT_CHANNEL)
A4_DOWNCAST_FUNC(LexerCustomAction, A4_LexerAction, A4_LexerCustomAction, A4_LAT_CUSTOM)
A4_DOWNCAST_FUNC(LexerModeAction, A4_LexerAction, A4_LexerModeAction, A4_LAT_MODE)
A4_DOWNCAST_FUNC(LexerMoreAction, A4_LexerAction, A4_LexerMoreAction, A4_LAT_MORE)
A4_DOWNCAST_FUNC(LexerPopModeAction, A4_LexerAction, A4_LexerPopModeAction, A4_LAT_POP_MODE)
A4_DOWNCAST_FUNC(LexerPushModeAction, A4_LexerAction, A4_LexerPushModeAction, A4_LAT_PUSH_MODE)
A4_DOWNCAST_FUNC(LexerSkipAction, A4_LexerAction, A4_LexerSkipAction, A4_LAT_SKIP)
A4_DOWNCAST_FUNC(LexerTypeAction, A4_LexerAction, A4_LexerTypeAction, A4_LAT_TYPE)
A4_DOWNCAST_FUNC(LexerIndexedCustomAction, A4_LexerAction, A4_LexerIndexedCustomAction, A4_LAT_INDEXED_CUSTOM)

A4_LexerAction* A4_NODISCARD A4_LexerAction_New(A4_MemoryPool* pool, A4_LexerActionType type, uint16_t arg1, uint16_t arg2);

A4_LexerChannelAction* A4_NODISCARD A4_LexerChannelAction_New(A4_MemoryPool* pool, uint16_t channel);
A4_LexerCustomAction* A4_NODISCARD A4_LexerCustomAction_New(A4_MemoryPool* pool, uint16_t rule_index, uint16_t action_index);
A4_LexerModeAction* A4_NODISCARD A4_LexerModeAction_New(A4_MemoryPool* pool, uint16_t mode);
A4_LexerMoreAction* A4_NODISCARD A4_LexerMoreAction_New(A4_MemoryPool* pool);
A4_LexerPopModeAction* A4_NODISCARD A4_LexerPopModeAction_New(A4_MemoryPool* pool);
A4_LexerPushModeAction* A4_NODISCARD A4_LexerPushModeAction_New(A4_MemoryPool* pool, uint16_t mode);
A4_LexerSkipAction* A4_NODISCARD A4_LexerSkipAction_New(A4_MemoryPool* pool);
A4_LexerTypeAction* A4_NODISCARD A4_LexerTypeAction_New(A4_MemoryPool* pool, uint16_t type);

#endif /* UUID */
