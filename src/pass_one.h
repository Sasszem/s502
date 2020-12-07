#ifndef GUARD_PASS_ONE
#define GUARD_PASS_ONE

#include "tokenslist.h"
#include "state.h"

/**
 * @file
 * @brief processing step one
 *
 * See [documentation on steps](assemble.md) for further info!
 */

/**
 * @brief Compilation pass 1
 * @param s compiler state
 * @returns 0 on success, -1 on error
 *
 * First pass processing for directives, instructions and labels<br>
 * See [documentation on steps](assemble.md) for further info!
 */
int pass_one(State* s);

#endif