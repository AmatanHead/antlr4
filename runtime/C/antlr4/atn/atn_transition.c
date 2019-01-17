#include "atn_transition.h"

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

    A4_UNREACHABLE();
}
