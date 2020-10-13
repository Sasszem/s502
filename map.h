
#define DEFINE_MAX_LEN 16

struct MapEntry {
    int value;
    char name[DEFINE_MAX_LEN];
    struct MapEntry *next;
};

typedef struct {
    struct MapEntry *head, *tail;
} Map;

struct MapEntry* map_find(Map *d, char *key);
void map_set(Map *d, char *name, int value);
void map_empty(Map *d);
int map_get(Map *d, char *name);
void map_debug_print(Map *d);



