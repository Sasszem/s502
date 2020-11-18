#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

#include "map.h"
#include "tokenslist.h"
#include "state.h"
#include "loadfile.h"
#include "pass_one.h"
#include "instructions.h"

int main(int argc, char**argv) {
    if (argc>=2) {
        int a;
        sscanf(argv[1], "%d", &a);
        logging_level(a);
    }
    State *state = state_new();
    if (!state) goto ERR_INIT;
    if (state_load_instr(state, "opcodes.csv")<0) goto ERR_INIT;
    LOG(2, "Init done!\n");

    TokensList *list = load_file("test.asm");
    if (!list) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }

    if (pass_one(state, list)<0) {
        FAIL("Compilation failed!\n");
        tokenslist_free(list);
        list = NULL;
        state_free(state);
        state = NULL;
        return -1;
    }
    
    LOG(2, "Now dunping tha file: \n");
    LOGDO(2, tokenslist_debug_print(list));
    LOG(2, "Now tha defines:\n");
    LOGDO(2, map_debug_print(state->defines));
    LOG(2, "Cleaning up...\n");
    tokenslist_free(list);
    state_free(state);
    state = NULL;
    list = NULL;

    return 0;
    
    ERR_INIT:
        FAIL("Initialization failed!\n");
        state_free(state);
        return -1;
}