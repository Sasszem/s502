#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

#include "map.h"
#include "tokenslist.h"
#include "state.h"
#include "loadfile.h"
#include "preprocess.h"
#include "instructions.h"

int main() {
    State *state = state_new();
    if (!state) goto ERR_INIT;
    if (state_load_instr(state, "opcodes.csv")<0) goto ERR_INIT;
    instruction_print(state->instr);
    LOG("Init done!\n");

    TokensList *list = load_file("test.asm");
    if (!list) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }

    if (preprocess(state, list)<0) {
        FAIL("Compilation failed!\n");
        tokenslist_free(list);
        list = NULL;
        state_free(state);
        state = NULL;
        return -1;
    }
    
    LOG("Now dunping tha file: \n");
    LOGDO(tokenslist_debug_print(list));
    LOG("Now tha defines:\n");
    LOGDO(map_debug_print(state->defines));
    LOG("Cleaning up...\n");
    tokenslist_free(list);
    state_free(state);
    state = NULL;
    list = NULL;

    ERR_INIT:
        FAIL("Initialization failed!\n");
        state_free(state);
        return -1;
}