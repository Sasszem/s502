#ifndef GUARD_STATE
#define GUARD_STATE

#include "map.h"
#include "tokenslist.h"
#include "instructions.h"

enum {
    STATE_MAX_STRING_LEN = 32
};


/**
 * 
 * @brief Compiler pseudo-global state
 * @class State
 * 
 * - defines
 * - labels
 * - tokens?
 * - (args)
 * - (settings)
 * - (opcodes)
 * 
 */
typedef struct {
    Map *defines;
    Map *labels;
    TokensList *tokens;
    Instruction *instr;
    int PC;
    char infile[STATE_MAX_STRING_LEN], outfile[STATE_MAX_STRING_LEN];
} State;

/**
 * @memberof State
 * @brief Create a new State object
 * @returns the new object or NULL
 */
State* state_new();

/**
 * @memberof State
 * @brief load instructions from a file
 * @returns 0 on success, -1 on error
 */
int state_load_instr(State* s, char *fname);

/**
 * @brief free a State object and all associated memory
 * @param s state to free
 * 
 * Pointer should be NULLed after this!
 */
void state_free(State *s);

/**
 * @brief parse command line arguments and update state
 * @param s state to update
 * @param argc as in main()
 * @param argv as in main()
 * @returns 0 on success, -1 on error
 * 
 * Undestands:
 * -o --out
 * -d --define
 * -l --log
 * last parameter is input file
 * 
 * prints usage if has a problem 
 */
int state_parse_commandline(State *s, int argc, char** argv);

#endif