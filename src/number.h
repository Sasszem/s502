#ifndef GUARD_NUMBER
#define GUARD_NUMBER

#include "state.h"

/**
 * @file
 * @brief Number module to parse numbers
 *
 * Handles number in decimal and hex, labels, constants and modifiers
 */

enum {
    /// Could not parse a number or constant is undefined
    NUMBER_ERROR = -1,
    /// Undefined label
    NUMBER_LABEL_NODEF = -2,
};

/**
 * @brief interpret a string as a constant, label or number
 * @param s State to read constant / label values from
 * @param str string to interpret
 *
 * - get the value of a constant or label
 * - parse number
 * - handles modifier symbols<br>
 * Returns NUMBER_ERROR on error, or NUMBER_LABEL_NODEF if number is a valid label with no value
 */
int number_get_number(State* s, char* str);


/**
 * @brief Convert a hex char to a digit
 * @param c char to convert
 * @returns value or NUMBER_ERROR on some error
 *
 * Handles upper or lowercase chars<br>
 * Returns NUMBER_ERROR on failed conversion
 */
int number_char_to_digit(char c);

#endif