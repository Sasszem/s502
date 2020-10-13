#include "util.h"
#include "logging.h"

char* util_find_string_segment(char *ptr) {
    char *end = ptr;
    while(*end!=' ' && *end!='\0') end++;
    return end;
}
