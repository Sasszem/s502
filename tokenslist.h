#ifndef GUARD_TOKENSLIST
#define GUARD_TOKENSLIST

#include "token_t.h"

/**
 * An element of a TokensList
 */
typedef struct TokensListElement {
    Token *token;
    struct TokensListElement *next, *prev;
} TokensListElement;

/**
 * A doubly-linked list for storing Tokens
 */
typedef struct {
    TokensListElement *head;
    TokensListElement *tail;
} TokensList;

TokensList* tokenslist_new();
int tokenslist_add(TokensList *list, Token t);
TokensListElement* tokenslist_remove(TokensList *list, TokensListElement *el);
void tokenslist_insert(TokensList *list, TokensListElement *target, TokensList *src);
void tokenslist_free(TokensList *list);
void tokenslist_debug_print(TokensList *list);

/**
 * @brief Do token recognition on all tokens in a list
 * @param t tokenslist to use
 * @returns 0 on success, -1 on error
 */
int tokenslist_recognize(TokensList* t);

#endif