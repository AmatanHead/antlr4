#ifndef _E77A4425_8696_476D_899D_53426759EB5D
#define _E77A4425_8696_476D_899D_53426759EB5D

#include "../common.h"

#include "state.h"

#include "fwd.h"


/**
 * A tuple: (ATN state, predicted alt, syntactic, semantic context).
 *
 * The syntactic context is a graph-structured stack node whose path(s) to the root is the rule invocation(s) chain
 * used to arrive at the state. The semantic context is the tree of semantic predicates encountered
 * before reaching an ATN state.
 *
 * Note: some properties are not included into this struct yet. I'm concentrating on lexer implementation so I omit
 * everything that's not required for lexer.
 */
a4_struct(A4_ATNConfig) {
    /// Id of the ATN state associated with this configuration.
    uint16_t state_id;

    /// What alt (or lexer rule) is predicted by this configuration.
    uint16_t alt;

    /// We cannot execute predicates dependent upon local context unless we know for sure we are in the correct
    /// context. Because there is no way to do this efficiently, we simply cannot evaluate dependent predicates
    /// unless we are in the rule that initially invokes the ATN simulator.
    bool reaches_into_outer_context;
};

static_assert(sizeof(A4_ATNConfig) == 6, "please, keep memory footprint of this struct as low as possible");

#endif /* UUID */
