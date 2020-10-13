
#include "token.h"

typedef struct TokensListElement {
    Token token;
    struct TokensListElement *next, *prev;
} TokensListElement;

typedef struct {
    TokensListElement *head;
    TokensListElement *tail;
} TokensList;

TokensList* tokenslist_make();
void tokenslist_add(TokensList *list, Token t);
void tokenslist_remove(TokensList *list, TokensListElement *el);
void tokenslist_delete(TokensList *list);
void token_print(Token *token);
void tokenslist_debug_print(TokensList *list);
