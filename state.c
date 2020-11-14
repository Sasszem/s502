
#include "state.h"
#include <stdlib.h>
#include "logging.h"
#include "debugmalloc.h"

/**
 * @brief Create a new State object
 * @returns the new object or NULL
 */
State* state_new() {
    State *ret = (State*)malloc(sizeof(State));
    
    if (ret==NULL) {
        ERROR("Memory allocation error in state_new()");
        return NULL;
    }

    if ((ret->defines = map_new()) == NULL) {
        FAIL("state_new() failed!\n");
        free(ret);
        return NULL;
    }

    if ((ret->labels = map_new())==NULL) {
        map_free(ret->labels);
        free(ret);
        FAIL("state_new() failed!\n");
        return NULL;
    }
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


