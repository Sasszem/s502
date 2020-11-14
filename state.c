
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

    if ((ret->defines = map_new()) == NULL) goto ERR;
    if ((ret->labels = map_new())==NULL) goto ERR;

    ret->instr = NULL;
    ret->tokens = NULL;
    return ret;

ERR:
    FAIL("state_new() failed!\n");
    state_free(ret);
    return 0;
}

/**
 * @brief load instructions from a file
 * @returns 0 on success, -1 on error
 */
int state_load_instr(State *s, char *fname) {
    s->instr = instruction_load(fname);
    if (s->instr == NULL) return -1;
    return 0;
}

/**
 * Free a State object
 * Pointer should be nulled after this!
 */
void state_free(State *s) {
    if (!s) return;
    map_free(s->defines);
    tokenslist_free(s->tokens);
    map_free(s->labels);
    instruction_free(s->instr);
    free(s);
}


