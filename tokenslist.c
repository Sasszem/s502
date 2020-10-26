#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>

#include "tokenslist.h"
#include "logging.h"
#include "debugmalloc.h"
/**
 * Create a new (empty) linekd list for tokens 
 */
TokensList* tokenslist_new() {
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

TokensListElement* tokenslist_remove(TokensList *list, TokensListElement *el) {
    if (list->head == el)
        list->head = el->next;
    if (list->tail == el)
        list->tail = el->prev;
    
    if (el->next!=NULL)
        el->next->prev = el->prev;
    if (el->prev!=NULL) {
        el->prev->next = el->next;
    }
    TokensListElement *next = el->next;
    free(el);
    return next;
}

void tokenslist_free(TokensList *list) {
    while (list->head!=NULL) {
        TokensListElement *n = list->head->next;
        free(list->head);
        list->head = n;
    }
    list->head = NULL;
    list->tail = NULL;
    free(list);
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

/**
 * Insert the contents of SRC into another list after POS
 */
void tokenslist_insert(TokensList *list, TokensListElement *pos, TokensList *src) {
    TokensListElement *next = pos->next;

    if (src->head == NULL)
        return;

    // doubly-link starts
    pos->next = src->head;
    src->head->prev = pos;

    
    src->tail->next = next;
    if (next!=NULL)
        next->prev = src->tail;
    
    src->head = NULL;
    src->tail = NULL;
}