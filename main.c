#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>
#include "debugmalloc.h"

#define LOGGING 1
#include "logging.h"

#include "map.h"
#include "tokenslist.h"
#include "state.h"
#include "loadfile.h"

int main() {
    State *state = state_make();
    TokensList *list = load_file(state, "test.asm");

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