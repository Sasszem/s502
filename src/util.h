#ifndef GUARD_UTIL
#define GUARD_UTIL

/**
 * @file
 * @brief various utility functions
 */

/**
 * @brief find pointer to next space or null in string
 * @param ptr string to search in
 * @returns address of next space or null character in string
 */
char* util_find_string_segment(char* ptr);

/**
 * @brief Case-insensitive character compare
 * @param a first char to compare
 * @param b second char to compare
 * @returns 0 if the two characters match, 1 otherwise
 */
int util_match_char(char a, char b);

/**
 * @brief case-insensitive memcmp
 * @param count how many bytes to match
 * @param first first string to compare
 * @param second second string to compare
 * @returns 0 if the strings match, 1 otherwise
 */
int util_match_string(char* first, char* second, int count);



/**
 * @brief split a string into segments on spaces
 * @param str string to split
 * @param n int pointer to return segment count to
 * @returns an array of substrings
 *
 * Uses a not-so-nice trick to enable easier handling<br>
 * The returned *char[] also contains a buffer with the actual substring data<br>
 * This also means simply freeing it is all what is needed to be done on the caller side<br>
 */
char** util_split_string(char* str, int* n);

#endif
