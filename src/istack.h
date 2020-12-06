#ifndef GUARD_ISTACK
#define GUARD_ISTACK

/**
 * @file
 * @brief iStack class for storing non-negative integers
 */

 /**
  * @brief very simple int stack
  * @class istack_el
  *
  * Simple stack for storing integers<br>
  * Implemented with a one-sentinel singly linked list
  */
typedef struct _istack {
    /// stored value
    int val;
    /// pointer to next element or NULL
    struct _istack* next;
} istack_el;

typedef istack_el* istack_ptr;

/**
 * @memberof istack_el
 * @brief create an empty istack
 * @returns istack pointer or NULL on error
 *
 * Constructor for istack.<br>
 * Returns head, which will be unchanged since it's a sentinel node.
 */
istack_ptr istack_new();

/**
 * @memberof istack_el
 * @brief check if the istack is empty or not
 * @param istack the istack to check
 * @returns 1 if empty, 0 if not
 *
 * Simple check of the emptiness of the stack.<br>
 * Simply looks at the next pointer
 */
int istack_empty(istack_ptr istack);

/**
 * @memberof istack_el
 * @brief push one element to the istack
 * @param istack istack to push to
 * @param val value to push
 * @returns 0 on success, -1 on error
 *
 * Push one element on the stack.<br>
 * Does not modify head pointer.
 */
int istack_push(istack_ptr istack, int val);


/**
 * @memberof istack_el
 * @brief pop the top of the istack
 * @param istack the istack to pop from
 * @returns 0 on success, -1 on error (popping from an empty istack)
 */
int istack_pop(istack_ptr istack);

/**
 * @memberof istack_el
 * @brief get the top element of the istack
 * @param istack the istack to get the top of
 * @param def default value if the istack is empty
 * @returns top of the istack or \p def
 */
int istack_top(istack_ptr istack, int def);

/**
 * @memberof istack_el
 * @brief free all memory associated with a istack
 * @param istack istack pointer
 *
 * Destructor of istack.<br>
 * Frees all memory, including head (sentinel) node.<br>
 * Invalidates all references to any elements (including head), so they all should be NULLed!
 */
void istack_free(istack_ptr istack);

#endif