#include "util.h"
#include "logging.h"

/**
 * Return a pointer to the end of the string segment
 * (the next space or 0 terminator after ptr)
 */
char* util_find_string_segment(char *ptr) {
    char *end = ptr;
    while(*end!=' ' && *end!='\0') end++;
    return end;
}
