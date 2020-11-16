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

int util_match_char(char a, char b) {
    return a==b || 
        (
            (
                (('a'<=a && a<='z')||('A'<=a && a<='Z')) && // a is a letter
                (('a'<=b && b<='z')||('A'<=b && b<='Z'))    // b is a letter
            ) && 
            (a-b=='A'-'a' || a-b=='a'-'A')
        );
}
