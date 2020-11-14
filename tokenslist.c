#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>

#include "tokenslist.h"
#include "tokenFunc.h"
#include "logging.h"
#include "debugmalloc.h"

/**
 * @brief Create a new (empty) TokensList object
 * @returns the new TokensList* or NULL on error
 */
TokensList* tokenslist_new() {
    TokensList *ret = (TokensList*)malloc(sizeof(TokensList));
    
    if (ret==NULL) {
        ERROR("Memory allocation error in tokenslist_new()!\n");
        return NULL;
    }
    
    ret->head = NULL;
    ret->tail = NULL;    
    return ret;
}

/**
 * @brief Append a token to the list
 * @returns 0 on success or -1 on error
 */
int tokenslist_add(TokensList *list, Token t) {
    TokensListElement *elem = (TokensListElement*)malloc(sizeof(TokensListElement));
    if (elem==NULL) {
        ERROR("Memory allocation error in tokenslist_add()!\n");
        return -1;
    }
    elem->next = NULL;
    elem->prev = NULL;
    elem->token = t;
    if (list->head==NULL) {
        list->head = elem;
        list->tail = elem;
        return 0;
    }
    list->tail->next = elem;
    elem->prev = list->tail;
    list->tail = elem;
    return 0;
}

/**
 * Remove a token from the list
 */
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

/**
 * Free ALL memory associated with the TokensList object
 * Pointer should be nulled after this!
 */
void tokenslist_free(TokensList *list) {
    if (!list) return;
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
        for (TokensListElement *ptr = list->head; ptr!=NULL; ptr = ptr->next)
            token_print(&(ptr->token));
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