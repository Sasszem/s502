#ifndef GUARD_STATE
#define GUARD_STATE

#include "map.h"
#include "tokenslist.h"
#include "instructions.h"
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
    Instruction *instr;
    int PC;
} State;


State* state_new();
int state_load_instr(State* s, char *fname);
void state_free(State *s);

#endif