#ifndef GUARD_STATE
#define GUARD_STATE

#include "map.h"
#include "tokenslist.h"

/**
 * Compiler pseudo-global state
 * - defines
 * - labels
 * - tokens?
 * - (args)
 * - (settings)
 * - (opcodes)
 */
typedef struct {
    Map *defines;
    Map *labels;
    TokensList *tokens;

} State;


State* state_new();
void state_free(State *s);

#endif