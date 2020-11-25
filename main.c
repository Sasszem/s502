#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>

#include "logging.h"
#include "pass_twothree.h"
#include "state.h"
#include "loadfile.h"
#include "pass_one.h"
#include "instructions.h"

#include "tokenFunc.h"

int main(int argc, char**argv) {
    logging_level(0);
    State *state = state_new();
    if (!state) goto ERR_INIT;
    if (state_load_instr(state, "opcodes.csv")<0) goto ERR_INIT;
    if (state_parse_commandline(state, argc, argv)<0) goto ERR_INIT;
    
    LOG(2, "Init done!\n");

    state->tokens = load_file(state->infile);
    if (!state->tokens) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }

    if (pass_one(state)<0) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }
    
    if (pass_two(state)<0) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }
    if (write_data(state)<0) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;  
    }

    LOG(2, "Now dunping tha file: \n");
    LOGDO(2, tokenslist_debug_print(state->tokens));
    LOG(2, "Now tha defines:\n");
    LOGDO(2, map_debug_print(state->defines));
    LOG(2, "And the labels:\n");
    LOGDO(2, map_debug_print(state->labels));
    LOG(2, "Cleaning up...\n");
    state_free(state);
    state = NULL;

    return 0;
    
    ERR_INIT:
        FAIL("Initialization failed!\n");
        state_free(state);
        return -1;
}