#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "logging.h"
#include "map.h"
#include "debugmalloc.h"


/**
 * Create a new map with 0 elements
 */
Map* map_new() {
    Map* ret = (Map*)malloc(sizeof(Map));
    ret->head = NULL;
    ret->tail = NULL;
    return ret;
}

/**
 * Find a key in a map
 * Returns NULL if not found
 */
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

/**
 * Sets a key in the map
 * Creates a new entry if needed
 */
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

/**
 * Free the map with all associated memory
 * Frees the map itself, so it's pointer will be invalid!
 */
void map_free(Map *d) {
    struct MapEntry *ptr;
    while (d->head!=NULL) {
        ptr = d->head->next;
        free(d->head);
        d->head = ptr;
    }
    d->head = NULL;
    d->tail = NULL;
    free(d);
}



/**
 * Get the value of a key
 * Returns -1 on not found
 */
int map_get(Map *d, char *name) {
    LOG("Getting %s\n", name);
    struct MapEntry *p = map_find(d, name);
    if (p==NULL)
        return -1;
    return p->value;
   
}

/**
 * Print all key-value pairs of the map
 * Intended for debugging purposes
 */
void map_debug_print(Map *d) {
    struct MapEntry *ptr = d->head;
    while(ptr!=NULL) {
        printf("\t%s:\t\t%d\n", ptr->name, ptr->value);
        ptr = ptr->next;
    }
}