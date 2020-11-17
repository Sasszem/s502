#ifndef GUARD_ISTACK
#define GUARD_ISTACK

/**
 * @brief very simple int istack
 */
typedef struct _istack {
    int val;
    struct _istack *next; 
} istack_el, *istack_ptr;

/**
 * @brief create an empty istack
 * @returns istack pointer or NULL on error
 */
istack_ptr istack_new();

/**
 * @brief check if the istack is empty or not
 * @param istack the istack to check
 * @returns 1 if empty, 0 if not
 */
int istack_empty(istack_ptr istack);

/**
 * @brief push one element to the istack
 * @param istack istack to push to
 * @param val value to push
 * @returns 0 on success, -1 on error
 */
int istack_push(istack_ptr istack, int val);


/**
 * @brief pop the top of the istack
 * @param istack the istack to pop from
 * @returns 0 on success, -1 on error (popping from an empty istack)
 */
int istack_pop(istack_ptr istack);

/**
 * @brief get the top element of the istack
 * @param istack the istack to get the top of
 * @param def default value if the istack is empty
 * @returns top of the istack or def
 */
int istack_top(istack_ptr istack, int def);

/**
 * @brief free all memory associated with a istack
 * @param istack istack pointer. Will be invalid after istack_free!
 */
void istack_free(istack_ptr istack);

#endif