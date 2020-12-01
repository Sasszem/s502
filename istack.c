#include "debugmalloc.h"

#include <stdlib.h>

#include "logging.h"
#include "istack.h"

istack_ptr istack_new() {
    istack_ptr head = malloc(sizeof(istack_el));
    if (!head) {
        ERROR("Memory allocation error in istack_new()!\n");
        return NULL;
    }
    head->next = NULL;
    return head;
}


int istack_empty(istack_ptr istack) {
    return (!istack->next);
};


int istack_push(istack_ptr istack, int val) {
    istack_ptr elem = malloc(sizeof(istack_el));
    if (!elem) {
        ERROR("Memory allocation error in istack_push()!\n");
        return -1;
    }
    elem->val = val;
    elem->next = istack->next;
    istack->next = elem;
    return 0;
}


int istack_pop(istack_ptr istack) {
    if (istack_empty(istack)) return -1;
    istack_ptr elem = istack->next;
    istack->next = elem->next;
    free(elem);
    return 0;
}


int istack_top(istack_ptr istack, int def) {
    if (istack_empty(istack)) return def;
    return istack->next->val;
};


void istack_free(istack_ptr istack) {
    while (!istack_empty(istack)) istack_pop(istack);
    free(istack);
}
