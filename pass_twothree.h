#ifndef GUARD_PASS23
#define GUARD_PASS23

#include "state.h"

/**
 * @brief compilation pass 2
 * @param s current state
 * @returns 0 on success, -1 on error
 *
 * Simply subtitutes remaining operand values (forward labels).
 */
int pass_two(State* s);

/**
 * @brief compilation pass 3
 * @param s current state
 * @returns 0 on success, -1 on error
 *
 * Writes binary data to file
 */
int write_data(State* s);

#endif