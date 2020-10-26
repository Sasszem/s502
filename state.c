
#include "state.h"
#include <stdlib.h>
#include "debugmalloc.h"

/**
 * Create a new State object
 */
State* state_new() {
    State *ret = (State*)malloc(sizeof(State));
    
    ret->defines = map_new();
    ret->labels = map_new();
    ret->tokens = NULL;

    return ret;
}

/**
 * Free a State object
 * Pointer should be nulled after this!
 */
void state_free(State *s) {
    map_free(s->defines);
    tokenslist_free(s->tokens);
    map_free(s->labels);
    free(s);
}


