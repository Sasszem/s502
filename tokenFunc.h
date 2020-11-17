#ifndef GUARD_TOKEN
#define GUARD_TOKEN


#include "token_t.h"
#include "instructions.h"
#include "state.h"

void token_print(Token *token);
int token_get_addressmode(Token *t);
int token_link_instruction(State *s, Token *token);

#endif