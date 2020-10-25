#include "token.h"
#include "stdio.h"

/**
 * Pretty-print one token, with its source and length
 */
void token_print(Token *token) {
    printf("\t%s:%d:%d\t\t'%.*s'\n", token->source.fname, token->source.lineno, token->len ,token->len, token->stripped);
}