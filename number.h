#ifndef GUARD_NUMBER
#define GUARD_NUMBER

#include "state.h"
int number_get_number(State *s, char *str, int count);
int number_parse_number(char *str, int count);
int number_char_to_digit(char c);
#endif