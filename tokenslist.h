#ifndef GUARD_TOKENSLIST
#define GUARD_TOKENSLIST

#include "token_t.h"

/**
 * @brief An element of a TokensList
 */
typedef struct TokensListElement {
    Token *token;
    struct TokensListElement *next, *prev;
} TokensListElement;

/**
 * @brief A doubly-linked list for storing Tokens
 * @class TokensList
 */
typedef struct {
    /// head (first element) of the list
    TokensListElement *head;
    /// tail (last element) pointer
    TokensListElement *tail;
} TokensList;

/**
 * @memberof TokensList
 * @brief Create a new (empty) TokensList object
 * @returns the new TokensList* or NULL on error
 */
TokensList* tokenslist_new();

/**
 * @memberof TokensList
 * @brief Append a token to the list
 * @returns 0 on success or -1 on error
 */
int tokenslist_add(TokensList *list, Token t);

/**
 * @memberof TokensList
 * @brief remove a token from the list
 * @returns the next element of the list (can be NULL)
 * 
 * The reason why we return is that we use it in an iteration over the same list
 */
TokensListElement* tokenslist_remove(TokensList *list, TokensListElement *el);

/**
 * @memberof TokensList
 * @brief Insert the contents of SRC into another list after an element
 * 
 * Keeps the original element in place
 */
void tokenslist_insert(TokensList *list, TokensListElement *target, TokensList *src);

/**
 * @memberof TokensList
 * @brief free ALL memory associated with the TokensList object
 * 
 * Pointer should be NULLed after this!
 */
void tokenslist_free(TokensList *list);

/**
 * @memberof TokensList
 * @brief Pretty-print all tokens in a list
 * 
 * Intended for debugging
 */
void tokenslist_debug_print(TokensList *list);

/**
 * @memberof TokensList
 * @brief Do token recognition on all tokens in a list
 * @param t tokenslist to use
 * @returns 0 on success, -1 on error
 */
int tokenslist_recognize(TokensList* t);

#endif