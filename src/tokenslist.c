#include "debugmalloc.h"

#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>

#include "tokenslist.h"
#include "tokenFunc.h"
#include "logging.h"

/**
 * @file
 * @brief implement TokensList methods
 * @see TokensList
 */

TokensList* tokenslist_new() {
    TokensList* ret = (TokensList*)malloc(sizeof(TokensList));

    if (ret == NULL) {
        ERROR("Memory allocation error in tokenslist_new()!\n");
        return NULL;
    }

    ret->head = NULL;
    ret->tail = NULL;
    return ret;
}


int tokenslist_add(TokensList* list, Token t) {
    TokensListElement* elem = (TokensListElement*)malloc(sizeof(TokensListElement));
    if (elem == NULL) goto ERR_MEM;
    elem->next = NULL;
    elem->prev = NULL;
    elem->token = malloc(sizeof(Token));
    if (!elem->token) goto ERR_MEM;
    *elem->token = t;
    if (list->head == NULL) {
        list->head = elem;
        list->tail = elem;
        return 0;
    }
    list->tail->next = elem;
    elem->prev = list->tail;
    list->tail = elem;
    return 0;

ERR_MEM:
    if (elem) {
        if (elem->token)
            free(elem->token);
        free(elem);
    }
    ERROR("Memory allocation error in tokenslist_add()!\n");
    return -1;
}


TokensListElement* tokenslist_remove(TokensList* list, TokensListElement* el) {
    if (list->head == el)
        list->head = el->next;
    if (list->tail == el)
        list->tail = el->prev;

    if (el->next != NULL)
        el->next->prev = el->prev;
    if (el->prev != NULL) {
        el->prev->next = el->next;
    }
    TokensListElement* next = el->next;
    free(el->token);
    free(el);
    return next;
}


void tokenslist_free(TokensList* list) {
    if (!list) return;
    while (list->head != NULL) {
        tokenslist_remove(list, list->head);
    }
    free(list);
}




void tokenslist_debug_print(TokensList* list) {
    LOG(0, "Dumping code:\n");

    for (TokensListElement* ptr = list->head; ptr != NULL; ptr = ptr->next)
        token_print(ptr->token);
}


void tokenslist_insert(TokensList* list, TokensListElement* pos, TokensList* src) {
    TokensListElement* next = pos->next;

    if (src->head == NULL)
        return;

    // doubly-link starts
    pos->next = src->head;
    src->head->prev = pos;


    src->tail->next = next;
    if (next != NULL)
        next->prev = src->tail;

    src->head = NULL;
    src->tail = NULL;
}


int tokenslist_recognize(TokensList* t) {
    for (TokensListElement* ptr = t->head; ptr != NULL; ptr = ptr->next)
        if (token_recognize(ptr->token) < 0) {
            FAIL("Can not recognize token types!\n");
            return -1;
        }
    return 0;
}