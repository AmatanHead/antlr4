#pragma once

#include "../util/bitset.h"
#include "../common.h"

#include "fwd.h"

#include <stdbool.h>


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
