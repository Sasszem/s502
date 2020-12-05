#ifndef GUARD_NUMBER
#define GUARD_NUMBER

#include "state.h"

enum {
    NUMBER_ERROR = -1,
    NUMBER_LABEL_NODEF = -2,
};

/**
 * @brief interpret a string as a constant, label or number
 * @param s State to read constant / label values from
 * @param str string to interpret
 * @param count length of string to parse
 *
 * String is not required to be 0-terminated.
 * - get the value of a constant or label
 * - parse number
 * - handles modifier symbols
 * Returns NUMBER_ERROR on error, or NUMBER_LABEL_NODEF if number is a valid label with no value
 */
int number_get_number(State* s, char* str);


int number_parse_number(char* str);

void number_print_err(int num, char* str);

/**
 * @brief Convert a hex char to a digit
 * @param c char to convert
 * @returns value or NUMBER_ERROR on some error
 *
 * Handles upper or lowercase chars
 * Returns NUMBER_ERROR on failed conversion
 */
int number_char_to_digit(char c);

#endif