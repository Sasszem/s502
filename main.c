#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>
#include "debugmalloc.h"

#include "logging.h"

#include "map.h"
#include "tokenslist.h"
#include "state.h"
#include "loadfile.h"
#include "preprocess.h"

int main() {
    State *state = state_make();
    TokensList *list = load_file("test.asm");

    if (preprocess(state, list)<0) {
        tokenslist_delete(list);
        free(list);
        state_delete(state);
        free(state);
        return -1;
    }

    if (list==NULL) {
        state_delete(state);
        free(state);
        return -1;
    }
    
    LOG("Now dunping tha file: \n");
    LOGDO(tokenslist_debug_print(list));
    LOG("Now tha defines:\n");
    LOGDO(map_debug_print(&(state->defines)));
    LOG("Cleaning up...\n");
    tokenslist_delete(list);
    state_delete(state);
    free(state);
    free(list);
    return 0;
}