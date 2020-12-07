#ifndef GUARD_STATE
#define GUARD_STATE

#include "map.h"
#include "tokenslist.h"
#include "instructions.h"

/**
 * @file
 * @brief implement State class
 */

enum {
    ///max string length for input/output files
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
    /// defined constants
    Map* defines;
    /// label locations
    Map* labels;
    /// tokens
    TokensList* tokens;
    /// instruction data
    Instruction* instr;
    /// PC (starts at 0)
    int PC;
    /// input file name
    char infile[STATE_MAX_STRING_LEN];
    /// output file name
    char outfile[STATE_MAX_STRING_LEN];
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
 * @param fname file path relative to CWD
 * @param s state to load into
 * @returns 0 on success, -1 on error
 */
int state_load_instr(State* s, char* fname);

/**
 * @memberof State
 * @brief free a State object and all associated memory
 * @param s state to free
 *
 * Pointer should be NULLed after this!
 */
void state_free(State* s);

/**
 * @memberof State
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
 *
 * last parameter is input file
 *
 * prints usage if has a problem
 */
int state_parse_commandline(State* s, int argc, char** argv);

#endif