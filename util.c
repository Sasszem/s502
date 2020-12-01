#include "debugmalloc.h"
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "logging.h"

/**
 * Return a pointer to the end of the string segment
 * (the next space or 0 terminator after ptr)
 */
char* util_find_string_segment(char* ptr) {
    char* end = ptr;
    while (*end != ' ' && *end != '\0') end++;
    return end;
}

int util_match_char(char a, char b) {
    return a == b ||
        (
            (
                (('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z')) && // a is a letter
                (('a' <= b && b <= 'z') || ('A' <= b && b <= 'Z'))    // b is a letter
                ) &&
            (a - b == 'A' - 'a' || a - b == 'a' - 'A')
            );
}


char** util_split_string(char* str, int* n) {
    // function to split a string into substrings on spaces
    // uses an ugly trick to avoid multiple buffers
    // (we NEED to store a copy of the original data bc we must write 0 terminators)
    int l = strlen(str);

    // count segments
    int m = 1;
    for (int i = 0; i < l; i++)
        if (str[i] == ' ') m++;
    *n = m;

    // the buffers stores the *char[]-s and then the copy of the data
    char** r = malloc(sizeof(char*) * m + l + 1);

    char* buf = (char*)r + sizeof(char*) * m;
    strcpy(buf, str);

    // first segemtn is the begining of the string
    r[0] = buf;
    // fill the table and write 0 terminators
    int j = 1;
    for (int i = 0; i < l; i++)
        if (str[i] == ' ') {
            buf[i] = 0;
            r[j++] = buf + i + 1;
        }
    return r;
}
