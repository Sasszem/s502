#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "logging.h"
#include "map.h"
#include "debugmalloc.h"



struct MapEntry* map_find(Map *d, char *key) {
    struct MapEntry *ptr = d->head;
    while(ptr!=NULL) {
        if (strncmp(ptr->name, key, DEFINE_MAX_LEN)==0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}


void map_set(Map *d, char *name, int value) {
    struct MapEntry *ptr = map_find(d, name);
    if (ptr==0) {
        ptr = (struct MapEntry*)malloc(sizeof(struct MapEntry));    
        ptr->next = NULL;
        if (d->head==NULL) {
            d->head = ptr;
            d->tail = ptr;
        } else {
            d->tail->next = ptr;
            d->tail = ptr; 
        }
        strncpy(ptr->name, name, DEFINE_MAX_LEN);
    }

    ptr->value = value;

}

void map_empty(Map *d) {
    struct MapEntry *ptr;
    while (d->head!=NULL) {
        ptr = d->head->next;
        free(d->head);
        d->head = ptr;
    }
    d->head = NULL;
    d->tail = NULL;
}



/**
 * Get the value of a define by name
 * Returns -1 on not found
 */
int map_get(Map *d, char *name) {
    LOG("Getting %s\n", name);
    struct MapEntry *p = map_find(d, name);
    if (p==NULL)
        return -1;
    return p->value;
   
}


void map_debug_print(Map *d) {
    struct MapEntry *ptr = d->head;
    while(ptr!=NULL) {
        printf("\t%s:\t\t%d\n", ptr->name, ptr->value);
        ptr = ptr->next;
    }
}

void defines_test() {
    Map d = {NULL, NULL};
    map_set(&d, "DEF1", 1234);
    map_set(&d, "DEF2", 4567);
    map_debug_print(&d);
    LOG("DEF1: %d\n", map_get(&d, "DEF1"));
    map_set(&d, "DEF1", 4321);
    LOGDO(map_debug_print(&d));
    LOG("DEF1: %d\n", map_get(&d, "DEF1"));
    map_empty(&d);
    LOG("DELETE\n");
    LOG("DEF1: %d\n", map_get(&d, "DEF1"));
    LOGDO(map_debug_print(&d));
}
