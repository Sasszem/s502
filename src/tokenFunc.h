#ifndef GUARD_TOKEN
#define GUARD_TOKEN


#include "token_t.h"
#include "instructions.h"
#include "state.h"

/**
 * @memberof Token
 * @brief Pretty-print one token, with its source and length
 */
void token_print(Token* token);

/**
 * @memberof Token
 * @brief Parse token - test if it's an opcode, a label or a directive
 * @param t token to recognize - will be modified in-place
 * @returns 0 on success, -1 on error
 */
int token_recognize(Token* t);

/**
 * @memberof Token
 * @brief analyze instruction token (instruction, addressmode & operand)
 * @returns 0 on success, -1 on error
 */
int token_analyze_instruction(State* s, Token* t);

/**
 * @memberof Token
 * @brief compile token into binary data
 * @param dataptr char** to return data to
 * @returns the number of bytes in dataptr
 */
int token_compile(State* s, Token* t, char** dataptr);

/**
 * @memberof Token
 * @brief parse the operand of the instruction as a number
 *
 * Modifies the token in-places.
 * Does NOT fail if operand is an undefined label!
 */
int token_get_operand(State* s, Token* t);

#endif