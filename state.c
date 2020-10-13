
#include "state.h"
#include <stdlib.h>
#include "debugmalloc.h"

State* state_make() {
    State *ret = (State*)malloc(sizeof(State));
    
    ret->defines.head = NULL;
    ret->defines.tail = NULL;
    
    ret->labels.head = NULL;
    ret->labels.tail = NULL;

    ret->tokens.head = NULL;
    ret->tokens.tail = NULL;

    return ret;
}


void state_delete(State *s) {
    map_empty(&(s->defines));
    tokenslist_delete(&(s->tokens));
    map_empty(&(s->labels));
}


