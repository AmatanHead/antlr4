#ifndef _0C572A40_329C_4718_8FA1_0A177082E49F
#define _0C572A40_329C_4718_8FA1_0A177082E49F

#include "fwd.h"

#include "atn.h"


/**
 * Simulator for lexer ATNs.
 */
a4_struct(A4_ATNLexerSimulator) {
    /// ATN which we're simulating.
    A4_ATN* atn;

    /// Pointer to the lexer that invoke this simulator. Stored here to be passed to executors.
    struct A4_Lexer* lexer;

    /// Lexer action executor.
    A4_ActionExecutor* action_executor;

    /// Lexer semantic predicate executor.
    A4_PredicateExecutor* sempred_executor;
};

#endif /* UUID */
