#ifndef GUARD_MAP
#define GUARD_MAP

enum { MAP_MAX_KEY_LEN = 16 };

struct MapEntry {
    int value;
    char name[MAP_MAX_KEY_LEN];
    struct MapEntry* next;
};

/**
 * Simple key->value map (str->int) implemented with a linked list
 */
typedef struct {
    struct MapEntry* head, * tail;
} Map;

Map* map_new();
struct MapEntry* map_find(Map* d, char* key);
int map_set(Map* d, char* name, int value);
void map_free(Map* d);
int map_get(Map* d, char* name);
void map_debug_print(Map* d);

#endif