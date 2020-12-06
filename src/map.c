#include "debugmalloc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "logging.h"
#include "map.h"

/**
 * @file
 * @brief Map member functions
 *
 * See map.h for Map class!
 */

Map* map_new() {
    Map* ret = (Map*)malloc(sizeof(Map));
    if (ret == NULL) {
        ERROR("Memory allocation error in map_new()!\n");
        return NULL;
    }
    ret->head = NULL;
    return ret;
}

/**
 * @memberof Map
 * @private
 * @brief Find a key in a map
 * @returns NULL if not found
 * @param map map to search in
 * @param key key to search for
 */
struct MapEntry* map_find(Map* map, char* key) {
    for (struct MapEntry* ptr = map->head; ptr != NULL; ptr = ptr->next)
        if (strncmp(ptr->name, key, MAP_MAX_KEY_LEN) == 0)
            return ptr;
    return NULL;
}


int map_set(Map* map, char* name, int value) {
    struct MapEntry* ptr = map_find(map, name);
    if (ptr == 0) {
        ptr = (struct MapEntry*)malloc(sizeof(struct MapEntry));
        if (ptr == NULL) {
            ERROR("Memory allocation error in map_set()!\n");
            return -1;
        }
        ptr->next = map->head;
        map->head = ptr;
        strncpy(ptr->name, name, MAP_MAX_KEY_LEN - 1);
        ptr->name[MAP_MAX_KEY_LEN - 1] = 0;
    }

    ptr->value = value;
    return 0;
}


void map_free(Map* map) {
    if (!map) return;
    struct MapEntry* ptr;
    while (map->head != NULL) {
        ptr = map->head->next;
        free(map->head);
        map->head = ptr;
    }
    map->head = NULL;
    free(map);
}


int map_get(Map* map, char* name) {
    LOG(5, "Getting %s\n", name);
    struct MapEntry* p = map_find(map, name);
    if (p == NULL)
        return -1;
    return p->value;

}


void map_debug_print(Map* map) {
    for (struct MapEntry* ptr = map->head; ptr != NULL; ptr = ptr->next)
        printf("\t%s:\t\t%d\n", ptr->name, ptr->value);
}