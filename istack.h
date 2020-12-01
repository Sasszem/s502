#ifndef GUARD_ISTACK
#define GUARD_ISTACK

/**
 * @brief very simple int stack
 * @class istack
 *
 * Simple stack for storing integers
 * Implemented with a one-sentinel singly linked list
 */
typedef struct _istack {
    int val; /// stored value
    struct _istack* next;  /// pointer to next element or NULL
} istack_el, * istack_ptr;

/**
 * @memberof istack
 * @brief create an empty istack
 * @returns istack pointer or NULL on error
 *
 * Constructor for istack.
 * Returns head, which will be unchanged since it's a sentinel node.
 */
istack_ptr istack_new();

/**
 * @memberof istack
 * @brief check if the istack is empty or not
 * @param istack the istack to check
 * @returns 1 if empty, 0 if not
 *
 * Simple check of the emptiness of the stack.
 * Simply looks at the next pointer
 */
int istack_empty(istack_ptr istack);

/**
 * @memberof istack
 * @brief push one element to the istack
 * @param istack istack to push to
 * @param val value to push
 * @returns 0 on success, -1 on error
 *
 * Push one element on the stack.
 * Does not modify head pointer.
 */
int istack_push(istack_ptr istack, int val);


/**
 * @memberof istack
 * @brief pop the top of the istack
 * @param istack the istack to pop from
 * @returns 0 on success, -1 on error (popping from an empty ::istack)
 */
int istack_pop(istack_ptr istack);

/**
 * @memberof istack
 * @brief get the top element of the istack
 * @param istack the istack to get the top of
 * @param def default value if the istack is empty
 * @returns top of the istack or \p def
 */
int istack_top(istack_ptr istack, int def);

/**
 * @brief free all memory associated with a istack
 * @param istack istack pointer
 *
 * Destructor of istack.
 * Frees all memory, including head (sentinel) node.
 * Invalidates all references to any elements (including head), so they all should be NULLed!
 */
void istack_free(istack_ptr istack);

#endif