#ifndef GUARD_MAP
#define GUARD_MAP

/**
 * @file
 * @brief Map class
 *
 * Map class to store key-value pairs<br>
 * Used for constants and labels
 */

enum {
    /// Key buffer size for Map
    MAP_MAX_KEY_LEN = 32
};

/// One entry in a map
struct MapEntry {
    /// the value the entry holds
    int value;
    /// key of the entry
    char name[MAP_MAX_KEY_LEN];
    /// pointer to the next entry or NULL
    struct MapEntry* next;
};

/**
 * @class Map
 * @brief Simple key->value map (str->int)
 *
 * Implemented with a linked list.<br>
 * Should only store non-negative values, as "NOT FOUND" value is -1
 */
typedef struct {
    struct MapEntry* head;
} Map;

/**
 * @memberof Map
 * @brief Create a new map with 0 elements
 * @return the new map or NULL on error
 */
Map* map_new();

/**
 * @memberof Map
 * @brief Sets a key in the map
 * @param map map to set key in
 * @param name key to set
 * @param value non-negative value to set for the key
 * @return 0 on success, -1 on error
 */
int map_set(Map* map, char* name, int value);

/**
 * @memberof Map
 * @brief Get the value of a key
 * @returns the value of the key or -1 on not found
 * @param map the map to search in
 * @param name key to search for
 */
int map_get(Map* map, char* name);


/**
 * @memberof Map
 * @brief Free the map with all associated memory
 * @param map the map to free
 *
 * Frees the map itself, so it's pointer will be invalid and should be NULLed!
 */
void map_free(Map* map);

/**
 * @memberof Map
 * @brief Print all key-value pairs of the map
 * @param map the map to print
 *
 * Intended for debugging purposes
 */
void map_debug_print(Map* map);

#endif