#ifndef GUARD_STATE
#define GUARD_STATE

#include "map.h"
#include "tokenslist.h"
#include "instructions.h"

enum {
    STATE_MAX_STRING_LEN = 32
};


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
    char infile[STATE_MAX_STRING_LEN], outfile[STATE_MAX_STRING_LEN];
} State;


State* state_new();
int state_load_instr(State* s, char *fname);
void state_free(State *s);
int state_parse_commandline(State *s, int argc, char** argv);

#endif