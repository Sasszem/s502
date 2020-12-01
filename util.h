#ifndef GUARD_UTIL
#define GUARD_UTIL

char* util_find_string_segment(char* ptr);
int util_match_char(char a, char b);



/**
 * @brief split a string into segments on spaces
 * @param str string to split
 * @param n int pointer to return segment count to
 * @returns an array of substrings
 *
 * Uses a not-so-nice trick to enable easier handling
 * The returned *char[] also contains a buffer with the actual substring data
 * This also means simply freeing it is all what is needed to be done on the callee side
 */
char** util_split_string(char* str, int* n);

#endif
