#include "atn.h"

#include "state.h"
#include "transition.h"
#include "lexer_action.h"
#include "../util/uuid.h"

#include <memory.h>


#define A4_LIST_NAME ATNStateList
#define A4_LIST_PAYLOAD A4_ATNState*
#define A4_LIST_NO_DTOR
#include "../util/list.inl"

a4_struct(LoopBackStateNumber) {
    A4_ATNLoopEndState* state;
    uint16_t sn;
};
#define A4_LIST_NAME LoopBackStateNumbersList
#define A4_LIST_PAYLOAD LoopBackStateNumber
#define A4_LIST_NO_DTOR
#include "../util/list.inl"

a4_struct(EndStateNumber) {
    A4_ATNBlockStartStateBase* state;
    uint16_t sn;
};
#define A4_LIST_NAME EndStateNumbersList
#define A4_LIST_PAYLOAD EndStateNumber
#define A4_LIST_NO_DTOR
#include "../util/list.inl"

#define A4_LIST_NAME IntSetList
#define A4_LIST_PAYLOAD A4_IntSet*
#define A4_LIST_NO_DTOR
#include "../util/list.inl"

#define A4_LIST_NAME TransitionList
#define A4_LIST_PAYLOAD A4_ATNTransition*
#define A4_LIST_NO_DTOR
#include "../util/list.inl"


#define ERR(x) do { err = x; goto cleanup; } while (0)
#define CHCK(x) do { if (bcc + (x) > bc_end) ERR(A4_E_BC_INVALID); } while (0)
#define READ(x) do { CHCK(x); bco = bcc; bcc += (x); } while (0)
#define CALL(x) do { err = (x); if (err != A4_SUCCESS) ERR(err); } while (0)

size_t A4_ATN_FeatureIndex(const A4_UUID feature, const A4_UUID* features) {
    size_t i = 0;

    while (!A4_UUID_IsZero(features[i])) {
        if (A4_UUID_Eq(feature, features[i])) {
            return i;
        } else {
            i += 1;
        }
    }

    return ~((size_t) 0);
}

bool A4_ATN_FeatureSupported(const A4_UUID version, const A4_UUID feature, const A4_UUID* features) {
    size_t feature_index = A4_ATN_FeatureIndex(feature, features);
    if (feature_index == ~((size_t) 0)) return false;
    size_t version_index = A4_ATN_FeatureIndex(version, features);
    if (version_index == ~((size_t) 0)) return false;
    return version_index >= feature_index;
}

static A4_ERRNO ReadSets(const uint16_t* bc_end, uint16_t** bcc_state, uint16_t** bco_state, IntSetList* res, bool utf32, A4_MemoryPool* pool);
static A4_ERRNO CheckATN(A4_ATN* atn);

A4_ERRNO A4_ATN_Load(A4_ATN** result, A4_MemoryPool* pool, const uint16_t* bc_raw, size_t bc_size) {
    A4_Errno err = A4_SUCCESS;

    *result = NULL;

    // Vars

    uint16_t* bc = NULL;  // Bytecode.
    uint16_t* bc_end = NULL;  // Bytecode end.
    uint16_t* bcc = NULL;  // Current bytecode head position.
    uint16_t* bco = NULL;  // Old bytecode head position.

    ATNStateList* states = NULL;
    LoopBackStateNumbersList* loop_back_state_numbers = NULL;
    EndStateNumbersList* end_state_numbers = NULL;
    IntSetList* sets = NULL;
    A4_IntSet** sets_array = NULL;
    TransitionList** state_to_transitions = NULL;

    // Constants

    // WARNING: these UUIDs should be equal to those in java code.
    // See antlr4/runtime/Java/src/org/antlr/v4/runtime/atn/ATNDeserializer.java

    const A4_UUID BASE_SERIALIZED_UUID = A4_UUID_Read("33761B2D-78BB-4A43-8B0B-4F5BEE8AACF3");
    const A4_UUID ADDED_PRECEDENCE_TRANSITIONS = A4_UUID_Read("1DA0C57D-6C06-438A-9B27-10BCB3CE0F61");
    const A4_UUID ADDED_LEXER_ACTIONS = A4_UUID_Read("AADB8D7E-AEEF-4415-AD2B-8204D6CF042E");
    const A4_UUID ADDED_UNICODE_SMP = A4_UUID_Read("59627784-3BE5-417A-B9EB-8131A7286089");

    const A4_UUID SUPPORTED_UUIDS[] = {
        BASE_SERIALIZED_UUID,
        ADDED_PRECEDENCE_TRANSITIONS,
        ADDED_LEXER_ACTIONS,
        ADDED_UNICODE_SMP,
        A4_UUID_Zero()
    };

    // Init bytecode

    bc = malloc(bc_size * sizeof(uint16_t));
    if (!bc) ERR(A4_E_MEMORY);
    memcpy(bc, bc_raw, bc_size * sizeof(uint16_t));

    bcc = bco = bc;
    bc_end = bc + bc_size;

    for (size_t i = 1; i < bc_size; ++i)
        bc[i] -= 2;

    // Check supported versions

    READ(1);
    if (*bco != 3) ERR(A4_E_BC_NOT_SUPPORTED);

    READ(8);
    A4_UUID ver = A4_UUID_ReadBinary(bco);
    if (A4_ATN_FeatureIndex(ver, SUPPORTED_UUIDS) == ~((size_t) 0)) ERR(A4_E_BC_NOT_SUPPORTED);

    // I don't thing we want to support older versions of bc. How would one obtain generated code for C target with
    // old version of bc?
    bool supports_precedence_predicates = A4_ATN_FeatureSupported(ver, ADDED_PRECEDENCE_TRANSITIONS, SUPPORTED_UUIDS);
    if (!supports_precedence_predicates) ERR(A4_E_BC_NOT_SUPPORTED);
    bool supports_lexer_actions = A4_ATN_FeatureSupported(ver, ADDED_LEXER_ACTIONS, SUPPORTED_UUIDS);
    if (!supports_lexer_actions) ERR(A4_E_BC_NOT_SUPPORTED);
    bool supports_unicode_smp = A4_ATN_FeatureSupported(ver, ADDED_UNICODE_SMP, SUPPORTED_UUIDS);
    if (!supports_unicode_smp) ERR(A4_E_BC_NOT_SUPPORTED);

    // Allocate ATN

    A4_ATN* atn = A4_POOL_ALLOC(pool, A4_ATN);
    if (!atn) ERR(A4_E_MEMORY);
    memset(atn, 0, sizeof(A4_ATN));
    atn->pool = pool;

    // Read ATN header

    READ(1);
    atn->grammar_type = (A4_ATNType) (*bco);
    if (atn->grammar_type > A4_ATNT_MAX_VAL) ERR(A4_E_BC_INVALID);

    READ(1);
    atn->max_token_type = *bco;

    // Read states

    READ(1);
    uint16_t n_states = *bco;

    states = ATNStateList_New();
    loop_back_state_numbers = LoopBackStateNumbersList_New();
    end_state_numbers = EndStateNumbersList_New();

    for (size_t i = 0; i < n_states; ++i) {
        READ(1);
        A4_ATNStateType type = (A4_ATNStateType) (*bco);
        if (type > A4_ATNST_MAX_VAL) ERR(A4_E_BC_INVALID);

        if (type == A4_ATNST_INVALID) {
            // Having invalid states lying around is a perfect way to shoot your leg, but since the original
            // antlr implementation have this logic, i'll keep it. For now, at least.
            // TODO: should we rather fail if there are invalid states?
            CALL(ATNStateList_Append(states, NULL));
            continue;
        }

        READ(1);
        uint16_t rule_index = *bco;  // 0xFFFF = invalid state

        A4_ATNState* state = A4_ATNState_New(pool, type, (uint16_t) ATNStateList_Size(states), rule_index);
        if (!state) ERR(A4_E_MEMORY);

        if (A4_IsLoopEndState(state)) {
            READ(1);
            uint16_t loop_back_state_number = *bco;

            A4_ATNLoopEndState* loop_end_state = A4_ToLoopEndState(state);

            CALL(
                LoopBackStateNumbersList_Append(loop_back_state_numbers, (LoopBackStateNumber) {
                    .state = loop_end_state,
                    .sn = loop_back_state_number
                })
            );
        } else if (A4_IsBlockStartStateBase(state)) {
            READ(1);
            uint16_t end_state_number = *bco;

            A4_ATNBlockStartStateBase* block_start_state_base = A4_ToBlockStartStateBase(state);

            CALL(
                EndStateNumbersList_Append(end_state_numbers, (EndStateNumber) {
                    .state = block_start_state_base,
                    .sn = end_state_number
                })
            );
        }

        CALL(ATNStateList_Append(states, state));
    }

    // Fill states array

    atn->states.size = ATNStateList_Size(states);
    atn->states.data = ATNStateList_CopyToPool(states, pool);
    if (!atn->states.data) ERR(A4_E_MEMORY);

    // Patch states, set up crossrefs between them

    A4_LIST_FOREACH(LoopBackStateNumbersList, loop_back_state_numbers, it) {
        if (it->payload.sn >= atn->states.size) ERR(A4_E_BC_INVALID);
        it->payload.state->loopback_state = atn->states.data[it->payload.sn];
    }

    A4_LIST_FOREACH(EndStateNumbersList, end_state_numbers, it) {
        if (it->payload.sn >= atn->states.size) ERR(A4_E_BC_INVALID);
        if (!A4_IsBlockEndState(atn->states.data[it->payload.sn])) ERR(A4_E_BC_INVALID);
        it->payload.state->end_state = A4_ToBlockEndState(atn->states.data[it->payload.sn]);
    }

    READ(1);
    uint16_t num_non_greedy_states = *bco;
    for (size_t i = 0; i < num_non_greedy_states; ++i) {
        READ(1);
        uint16_t st = *bco;
        if (st >= atn->states.size) ERR(A4_E_BC_INVALID);
        if (!A4_IsDecisionState(atn->states.data[st])) ERR(A4_E_BC_INVALID);
        A4_ToDecisionState(atn->states.data[st])->non_greedy = true;
    }

    READ(1);
    uint16_t num_precedence_states = *bco;
    for (size_t i = 0; i < num_precedence_states; ++i) {
        READ(1);
        uint16_t st = *bco;
        if (st >= atn->states.size) ERR(A4_E_BC_INVALID);
        if (!A4_IsRuleStartState(atn->states.data[st])) ERR(A4_E_BC_INVALID);
        A4_ToRuleStartState(atn->states.data[st])->is_left_recursive_rule = true;
    }

    // Read rules

    READ(1);
    uint16_t num_rules = *bco;

    if (atn->grammar_type == A4_ATNT_LEXER) {
        atn->rule_to_token_type = A4_POOL_CALLOC(pool, uint16_t, num_rules);
        if (!atn->rule_to_token_type) ERR(A4_E_MEMORY);
    }

    atn->rule_to_start_state = A4_POOL_CALLOC(pool, A4_ATNRuleStartState*, num_rules);
    if (!atn->rule_to_start_state) ERR(A4_E_MEMORY);

    for (size_t i = 0; i < num_rules; ++i) {
        READ(1);
        uint16_t st = *bco;

        if (st >= atn->states.size) ERR(A4_E_BC_INVALID);
        if (!A4_IsRuleStartState(atn->states.data[st])) ERR(A4_E_BC_INVALID);
        atn->rule_to_start_state[i] = A4_ToRuleStartState(atn->states.data[st]);

        if (atn->grammar_type == A4_ATNT_LEXER) {
            READ(1);
            uint16_t token_type = *bco;  // 0xFFFF = EOF

            atn->rule_to_token_type[i] = token_type;
        }
    }

    atn->rule_to_stop_state = A4_POOL_CALLOC(pool, A4_ATNRuleStopState*, num_rules);
    if (!atn->rule_to_stop_state) ERR(A4_E_MEMORY);

    for (size_t i = 0; i < atn->states.size; ++i) {
        if (!A4_IsRuleStopState(atn->states.data[i]))
            continue;
        A4_ATNRuleStopState* state = A4_ToRuleStopState(atn->states.data[i]);
        atn->rule_to_stop_state[state->base.rule_index] = state;
        atn->rule_to_start_state[state->base.rule_index]->stop_state = state;
    }

    // Read modes

    READ(1);
    uint16_t num_modes = *bco;

    atn->mode_to_start_state.size = num_modes;
    atn->mode_to_start_state.data = A4_POOL_CALLOC(pool, A4_ATNTokenStartState*, num_modes);
    if (!atn->mode_to_start_state.data) ERR(A4_E_MEMORY);

    for (size_t i = 0; i < num_modes; ++i) {
        READ(1);
        uint16_t st = *bco;
        if (st >= atn->states.size) ERR(A4_E_BC_INVALID);
        if (!A4_IsTokenStartState(atn->states.data[st])) ERR(A4_E_BC_INVALID);
        atn->mode_to_start_state.data[i] = A4_ToTokenStartState(atn->states.data[st]);
    }

    // Read interval sets

    sets = IntSetList_New();
    if (!sets) ERR(A4_E_MEMORY);

    CALL(ReadSets(bc_end, &bcc, &bco, sets, false, pool));
    CALL(ReadSets(bc_end, &bcc, &bco, sets, true, pool));

    // will need O(1) lookup
    sets_array = malloc(sizeof(A4_IntSet*) * IntSetList_Size(sets));
    if (!sets_array) ERR(A4_E_MEMORY);
    {
        size_t i = 0;
        A4_LIST_FOREACH(IntSetList, sets, it) {
            sets_array[i++] = it->payload;
        }
    }

    // Read ATN edges (note: order in which edges transitions in state matter!)

    READ(1);
    uint16_t num_edges = *bco;

    state_to_transitions = malloc(sizeof(TransitionList*) * atn->states.size);
    if (!state_to_transitions) ERR(A4_E_MEMORY);

    for (size_t i = 0; i < atn->states.size; ++i) {
        state_to_transitions[i] = TransitionList_New();
        if (!state_to_transitions[i]) ERR(A4_E_MEMORY);
    }

    for (size_t i = 0; i < num_edges; ++i) {
        READ(6);
        uint16_t src = *(bco + 0);
        if (src >= atn->states.size) ERR(A4_E_BC_INVALID);
        uint16_t trg = *(bco + 1);
        if (trg >= atn->states.size) ERR(A4_E_BC_INVALID);
        A4_ATNTransitionType type = (A4_ATNTransitionType) *(bco + 2);
        if (type > A4_ATNTT_MAX_VAL) ERR(A4_E_BC_INVALID);
        uint16_t arg1 = *(bco + 3);
        uint16_t arg2 = *(bco + 4);
        uint16_t arg3 = *(bco + 5);

        A4_ATNTransition* transition = A4_ATNTransition_New(pool, type, atn, atn->states.data[trg], arg1, arg2, arg3);

        if (transition->type == A4_ATNTT_SET || transition->type == A4_ATNTT_NOT_SET) {
            if (arg1 >= IntSetList_Size(sets)) ERR(A4_E_BC_INVALID);
            A4_ToSetTransition(transition)->set = sets_array[arg1];
        }

        CALL(TransitionList_Append(state_to_transitions[src], transition));
    }

    // Transitions from rule stop state into whatever follows the rule are not serialized. Instead,
    // they're restored from rule transitions.
    for (size_t i = 0; i < atn->states.size; ++i) {
        A4_LIST_FOREACH(TransitionList, state_to_transitions[i], it) {
            if (!A4_IsRuleTransition(it->payload)) {
                continue;
            }

            A4_ATNRuleTransition* transition = A4_ToRuleTransition(it->payload);

            uint16_t outermost_precedence_return = 0xFFFF;

            if (atn->rule_to_start_state[transition->base.target->rule_index]->is_left_recursive_rule) {
                if (transition->precedence == 0) {
                    outermost_precedence_return = transition->base.target->rule_index;
                }
            }

            A4_ATNEpsilonTransition* return_transition = A4_ATNEpsilonTransition_New(pool, transition->follow_state);
            if (!return_transition) ERR(A4_E_MEMORY);
            return_transition->outermost_precedence_return = outermost_precedence_return;

            uint16_t sn = atn->rule_to_stop_state[transition->base.target->rule_index]->base.state_number;

            CALL(TransitionList_Append(state_to_transitions[sn], (A4_ATNTransition*) return_transition));
        }
    }

    // Copy all transition lists into pool, populate state list of transitions, setup transitions metadata
    for (size_t i = 0; i < atn->states.size; ++i) {
        A4_ATNState* state = atn->states.data[i];
        if (state_to_transitions[i]) {
            state->transitions.size = TransitionList_Size(state_to_transitions[i]);
            state->transitions.data = TransitionList_CopyToPool(state_to_transitions[i], pool);
            if (!state->transitions.data) ERR(A4_E_MEMORY);

            if (state->transitions.size > 0) {
                state->epsilon_only_transitions = state->transitions.data[0]->is_epsilon;
                for (size_t j = 1; j < state->transitions.size; ++j) {
                    if (state->epsilon_only_transitions != state->transitions.data[j]->is_epsilon) {
                        ERR(A4_E_BC_INVALID);
                    }
                }
            }
        }
    }

    // Set up back references
    for (size_t i = 0; i < atn->states.size; ++i) {
        if (A4_IsBlockStartStateBase(atn->states.data[i])) {
            A4_ATNBlockStartStateBase* state = A4_ToBlockStartStateBase(atn->states.data[i]);
            if (state->end_state == NULL) ERR(A4_E_BC_INVALID);
            if (state->end_state->start_state != NULL) ERR(A4_E_BC_INVALID);
            state->end_state->start_state = state;
        }

        if (A4_IsPlusLoopBackState(atn->states.data[i])) {
            A4_ATNPlusLoopBackState* state = A4_ToPlusLoopBackState(atn->states.data[i]);
            for (size_t j = 0; j < state->base.base.transitions.size; ++j) {
                A4_ATNState* target = state->base.base.transitions.data[j]->target;
                if (A4_IsPlusBlockStartState(target)) {
                    A4_ToPlusBlockStartState(target)->loopback_state = state;
                }
            }
        } else if (A4_IsStarLoopBackState(atn->states.data[i])) {
            A4_ATNStarLoopBackState* state = A4_ToStarLoopBackState(atn->states.data[i]);
            for (size_t j = 0; j < state->base.transitions.size; ++j) {
                A4_ATNState* target = state->base.transitions.data[j]->target;
                if (A4_IsStarLoopEntryState(target)) {
                    A4_ToStarLoopEntryState(target)->loopback_state = state;
                }
            }
        }
    }

    // Read decisions

    READ(1);
    uint16_t num_decisions = *bco;

    atn->decision_to_state.size = num_decisions;
    atn->decision_to_state.data = A4_POOL_CALLOC(pool, A4_ATNDecisionState*, num_decisions);
    if (!atn->decision_to_state.data) ERR(A4_E_MEMORY);

    for (uint16_t i = 0; i < num_decisions; ++i) {
        READ(1);
        uint16_t sn = *bco;
        if (sn >= atn->states.size) ERR(A4_E_BC_INVALID);
        if (!A4_IsDecisionState(atn->states.data[sn])) ERR(A4_E_BC_INVALID);
        A4_ATNDecisionState* state = A4_ToDecisionState(atn->states.data[sn]);
        atn->decision_to_state.data[i] = state;
        state->decision = i;
    }

    // Read lexer actions
    if (atn->grammar_type == A4_ATNT_LEXER) {
        READ(1);
        uint16_t num_lexer_actions = *bco;

        atn->lexer_actions.size = num_lexer_actions;
        atn->lexer_actions.data = A4_POOL_CALLOC(pool, A4_LexerAction*, num_lexer_actions);
        if (!atn->lexer_actions.data) ERR(A4_E_MEMORY);

        for (size_t i = 0; i < num_lexer_actions; ++i) {
            READ(1);
            A4_LexerActionType type = *bco;
            if (type > A4_LAT_MAX_VAL) ERR(A4_E_BC_INVALID);
            READ(2);
            uint16_t arg1 = *(bco + 0);
            uint16_t arg2 = *(bco + 1);

            atn->lexer_actions.data[i] = A4_LexerAction_New(pool, type, arg1, arg2);
        }
    }

    // Mark precedence decisions

    for (size_t i = 0; i < atn->states.size; ++i) {
        A4_ATNState* state = atn->states.data[i];
        if (!A4_IsStarLoopEntryState(state))
            continue;
        if (atn->rule_to_start_state[state->rule_index]->is_left_recursive_rule) {
            A4_ATNState* maybe_loop_end_state = state->transitions.data[state->transitions.size - 1]->target;
            if (A4_IsLoopEndState(maybe_loop_end_state)) {
                if (maybe_loop_end_state->epsilon_only_transitions &&
                        A4_IsRuleStopState(maybe_loop_end_state->transitions.data[0]->target)) {
                    A4_ToStarLoopEntryState(state)->is_precedence_decision = true;
                }
            }
        }
    }

    CALL(CheckATN(atn));

    *result = atn;

cleanup:
    ATNStateList_Delete(states);
    LoopBackStateNumbersList_Delete(loop_back_state_numbers);
    EndStateNumbersList_Delete(end_state_numbers);
    IntSetList_Delete(sets);
    free(sets_array);
    if (state_to_transitions) {
        for (size_t i = 0; i < atn->states.size; ++i) {
            TransitionList_Delete(state_to_transitions[i]);
        }
    }
    free(state_to_transitions);
    free(bc);
    return err;
}


// bcc_state, bco_state, res should be initialized
static A4_ERRNO ReadSets(const uint16_t* bc_end, uint16_t** bcc_state, uint16_t** bco_state, IntSetList* res, bool utf32, A4_MemoryPool* pool) {
    A4_Errno err = A4_SUCCESS;

    uint16_t* bcc = *bcc_state;
    uint16_t* bco = *bco_state;

    A4_IntSet* set = A4_IntSet_New();
    if (!set) ERR(A4_E_MEMORY);

    READ(1);
    uint16_t num_sets = *bco;

    for (size_t i = 0; i < num_sets; ++i) {
        READ(1);
        uint16_t num_intervals = *bco;

        READ(1);
        bool contains_eof = *bco != 0;

        if (contains_eof) {
            CALL(A4_IntSet_Add(set, 0));
        }

        for (int j = 0; j < num_intervals; ++j) {
            uint32_t rune_start;

            READ(1);
            rune_start = *bco;

            if (utf32) {
                READ(1);
                rune_start |= ((uint32_t)*bco) << 16ul;
            }

            uint32_t rune_end;

            READ(1);
            rune_end = *bco;

            if (utf32) {
                READ(1);
                rune_end |= ((uint32_t)*bco) << 16ul;
            }

            CALL(A4_IntSet_AddInterval(set, rune_start, rune_end));
        }

        A4_IntSet* pool_set = A4_IntSet_CopyToPool(set, pool);
        if (!pool_set) ERR(A4_E_MEMORY);

        CALL(IntSetList_Append(res, pool_set));

        A4_IntSet_Clear(set);
    }

cleanup:
    *bcc_state = bcc;
    *bco_state = bco;
    A4_IntSet_Delete(set);
    return err;
}

#define COND(x) do { if (!(x)) return A4_E_ATN_INVALID; } while (0)

static A4_ERRNO CheckATN(A4_ATN* atn) {
    for (size_t i = 0; i < atn->states.size; ++i) {
        A4_ATNState* state = atn->states.data[i];

        if (!state) continue;

        COND(state->epsilon_only_transitions || state->transitions.size <= 1);

        if (A4_IsPlusBlockStartState(state)) {
            COND(A4_ToPlusBlockStartState(state)->loopback_state != NULL);
        }

        if (A4_IsStarLoopEntryState(state)) {
            A4_ATNStarLoopEntryState* star_loop_entry_state = A4_ToStarLoopEntryState(state);

            COND(star_loop_entry_state->loopback_state != NULL);
            COND(star_loop_entry_state->base.base.transitions.size == 2);

            if (A4_IsStarBlockStartState(star_loop_entry_state->base.base.transitions.data[0]->target)) {
                COND(A4_IsLoopEndState(star_loop_entry_state->base.base.transitions.data[1]->target));
                COND(!star_loop_entry_state->base.non_greedy);
            } else if (A4_IsLoopEndState(star_loop_entry_state->base.base.transitions.data[0]->target)) {
                COND(A4_IsStarBlockStartState(star_loop_entry_state->base.base.transitions.data[1]->target));
                COND(star_loop_entry_state->base.non_greedy);
            } else {
                COND(0);
            }
        }

        if (A4_IsStarLoopBackState(state)) {
            COND(state->transitions.size == 1);
            COND(A4_IsStarLoopEntryState(state->transitions.data[0]->target));
        }

        if (A4_IsLoopEndState(state)) {
            COND(A4_ToLoopEndState(state)->loopback_state != NULL);
        }

        if (A4_IsRuleStartState(state)) {
            COND(A4_ToRuleStartState(state)->stop_state != NULL);
        }

        if (A4_IsBlockStartStateBase(state)) {
            COND(A4_ToBlockStartStateBase(state)->end_state != NULL);
        }

        if (A4_IsBlockEndState(state)) {
            COND(A4_ToBlockEndState(state)->start_state != NULL);
        }

        if (A4_IsDecisionState(state)) {
            A4_ATNDecisionState* decision_state = A4_ToDecisionState(state);
            COND(decision_state->base.transitions.size <= 1 || decision_state->decision >= 0);
        } else {
            COND(state->transitions.size <= 1 || A4_IsRuleStopState(state));
        }
    }

    return A4_SUCCESS;
}
