#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>

#include "tokenslist.h"
#include "logging.h"
#include "debugmalloc.h"
/**
 * Create a new (empty) linekd list for tokens 
 */
TokensList* tokenslist_make() {
    TokensList *ret = (TokensList*)malloc(sizeof(TokensList));
    ret->head = NULL;
    ret->tail = NULL;    
    return ret;
}

/**
 * Add a token to a linekd list of tokens
 */
void tokenslist_add(TokensList *list, Token t) {
    TokensListElement *elem = (TokensListElement*)malloc(sizeof(TokensListElement));
    elem->next = NULL;
    elem->prev = NULL;
    elem->token = t;
    if (list->head==NULL) {
        list->head = elem;
        list->tail = elem;
        return;
    }
    list->tail->next = elem;
    elem->prev = list->tail;
    list->tail = elem;
}

void tokenslist_remove(TokensList *list, TokensListElement *el) {
    if (list->head == el)
        list->head = el->next;
    if (list->tail == el)
        list->tail = el->prev;
    
    if (el->next!=NULL)
        el->next->prev = el->prev;
    if (el->prev!=NULL) {
        el->prev->next = el->next;
    }
    free(el);
}

void tokenslist_delete(TokensList *list) {
    while (list->head!=NULL) {
        TokensListElement *n = list->head->next;
        free(list->head);
        list->head = n;
    }
}


/**
 * Pretty-print one token, with its source and length
 */
void token_print(Token *token) {
    printf("\t%s:%d:%d\t\t%.*s\n", token->source.fname, token->source.lineno, token->len ,token->len, token->stripped);
}


/**
 * Pretty-print all tokens in a list
 */
void tokenslist_debug_print(TokensList *list) {
    LOG("Dumping code:\n");

    // pretty suprised this is valid...
    LOGDO(
        TokensListElement *ptr = list->head;

        while (ptr!=NULL) {
            token_print(&(ptr->token));
            ptr = ptr->next;
        }
    );
}