#ifndef GUARD_DIRECTIVE
#define GUARD_DIRECTIVE

#include "tokenslist.h"
#include "state.h"

/**
 * @file
 * @brief step 1 and 3 processing for directive tokens
 *
 * Public interface for directive.h
 */

 /**
  * @brief Internal command type for directives
  *
  * Token processor functions return these to signal different commands to pass one
  */
enum DIRCommand {
    /// An error occured, stop compilation
    DIR_STOP = -1,
    /// Nothing special, do nothing
    DIR_NOP = 0,
    /// A conditional evaluated to true
    DIR_IF_TRUE,
    /// A conditional evaluated to false
    DIR_IF_FALSE,
    /// An endif was encountered, should pop from state stack
    DIR_ENDIF,
};

/**
 * @brief process a directive token
 * @param s state of the compiler
 * @param ptr ptr to the current token in the list
 * @param skip disable compilation flag (1=disabled)
 * @returns DIRCommand to pass 1
 *
 * Identifies directive type, and runs a processor function on it.
 */
enum DIRCommand do_directive_token(State* s, TokensListElement* ptr, int skip);

/**
 * @brief Compile a directive into binary data
 * @param dataptr return buffer for data
 * @param s assembler state
 * @param t token to compile
 * @returns number of bytes in buffer or -1 on error
 *
 * Simply relays it to compile_pad or compile_data
 */
int directive_compile(State* s, Token* t, char** dataptr);

#endif