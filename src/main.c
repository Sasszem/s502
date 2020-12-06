#define __USE_MINGW_ANSI_STDIO 1

#include "logging.h"
#include "pass_twothree.h"
#include "state.h"
#include "loadfile.h"
#include "pass_one.h"
#include "tokenFunc.h"

/**
 * @file
 * @brief Entry point
 *
 * Entry point of the application tying all the steps together
 */

/**
 * @brief standard libc main function
 * @param argc commandline arg count
 * @param argv commandline arguments
 * @returns exit code (0 on success, -1 on error)
 */
int main(int argc, char** argv) {
    logging_level(1);
    State* state = state_new();
    if (!state) goto ERR_INIT;
    if (state_load_instr(state, "opcodes.csv") < 0) goto ERR_INIT;
    if (state_parse_commandline(state, argc, argv) < 0) goto ERR_INIT;

    LOG(2, "Init done!\n");

    state->tokens = load_file(state->infile);
    if (!state->tokens) goto ERR_COMP;
    if (pass_one(state) < 0) goto ERR_COMP;
    if (pass_two(state) < 0) goto ERR_COMP;
    if (write_data(state) < 0) goto ERR_COMP;


    LOG(2, "Now dunping the file: \n");
    LOGDO(2, tokenslist_debug_print(state->tokens));
    LOG(2, "Now the defined constants:\n");
    LOGDO(2, map_debug_print(state->defines));
    LOG(2, "And the labels:\n");
    LOGDO(2, map_debug_print(state->labels));
    LOG(2, "Cleaning up...\n");
    state_free(state);

    return 0;

ERR_INIT:
    FAIL("Initialization failed!\n");
    state_free(state);
    return -1;

ERR_COMP:
    FAIL("Compilation failed!\n");
    state_free(state);
    state = NULL;
    return -1;
}