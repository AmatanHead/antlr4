#include "atn.h"

const char* A4_ATNStateTypeName(enum A4_ATNStateType type) {
    switch (type) {
        case A4_ATNST_INVALID: return "INVALID";
        case A4_ATNST_BASIC: return "BASIC";
        case A4_ATNST_RULE_START: return "RULE_START";
        case A4_ATNST_BLOCK_START: return "BLOCK_START";
        case A4_ATNST_PLUS_BLOCK_START: return "PLUS_BLOCK_START";
        case A4_ATNST_STAR_BLOCK_START: return "STAR_BLOCK_START";
        case A4_ATNST_TOKEN_START: return "TOKEN_START";
        case A4_ATNST_RULE_STOP: return "RULE_STOP";
        case A4_ATNST_BLOCK_END: return "BLOCK_END";
        case A4_ATNST_STAR_LOOP_BACK: return "STAR_LOOP_BACK";
        case A4_ATNST_STAR_LOOP_ENTRY: return "STAR_LOOP_ENTRY";
        case A4_ATNST_PLUS_LOOP_BACK: return "PLUS_LOOP_BACK";
        case A4_ATNST_LOOP_END: return "LOOP_END";
    }

    __builtin_unreachable();
}

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

    __builtin_unreachable();
}
