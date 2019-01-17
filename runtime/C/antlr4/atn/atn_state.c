#include "atn_state.h"

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

    A4_UNREACHABLE();
}
