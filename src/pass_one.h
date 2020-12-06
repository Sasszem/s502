#ifndef GUARD_PASS_ONE
#define GUARD_PASS_ONE

#include "tokenslist.h"
#include "state.h"

/**
 * @brief Compilation pass 1
 * @param s compiler state
 * @returns 0 on success, -1 on error
 *
 * See documentation on pass 1 for further info!
 */
int pass_one(State* s);

#endif