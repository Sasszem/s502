#ifndef GUARD_TOKENSLIST
#define GUARD_TOKENSLIST

#include "token.h"

typedef struct TokensListElement {
    Token token;
    struct TokensListElement *next, *prev;
} TokensListElement;

typedef struct {
    TokensListElement *head;
    TokensListElement *tail;
} TokensList;

TokensList* tokenslist_new();
void tokenslist_add(TokensList *list, Token t);
TokensListElement* tokenslist_remove(TokensList *list, TokensListElement *el);
void tokenslist_insert(TokensList *list, TokensListElement *target, TokensList *src);
void tokenslist_free(TokensList *list);
void tokenslist_debug_print(TokensList *list);

#endif