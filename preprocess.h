#ifndef GUARD_PREPROCESS
#define GUARD_PREPROCESS

#include "tokenslist.h"
#include "state.h"

/**
 * Internal command type for preprocessor
 * Token processor functions return these to signal different commands to the preprocessor
 */
enum PPCommand {
    PPC_STOP = -1,
    PPC_NOP = 0,
    PPC_IF_TRUE,
    PPC_IF_FALSE,
    PPC_ENDIF,
};

enum PPCommand do_preprocessor_token(State *s, TokensList *list, TokensListElement *ptr, int skip);

#endif