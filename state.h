#ifndef GUARD_STATE
#define GUARD_STATE

#include "map.h"
#include "tokenslist.h"

typedef struct {
    Map defines;
    Map labels;
    TokensList tokens;

} State;


State* state_make();
void state_delete(State *s);

#endif