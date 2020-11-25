#include "number.h"
#include <string.h>
#include "logging.h"

int is_valid_define_char(char c) {
    return  ('a'<=c && c<='z') ||
            ('A'<=c && c<='Z') ||
            ('0'<=c && c<='9') ||
            c=='_';
}

/**
 * Convert a hex char to a digit
 * Handles upper or lowercase chars
 * Returns NUMBER_ERROR on failed conversion
 */
int number_char_to_digit(char c) {
    if ('0'<=c && c<='9')
        return c-'0';
    if ('a'<=c && c<='f')
        c += 'A'-'a';
    if ('A'<=c && c<='F')
        return c-'A'+10;
    return NUMBER_ERROR;
}

/**
 * Parse a number from a string
 * Handles base marker $ symbol
 * Returns NUMBER_ERROR on failed parsing
 */
int number_parse_number(char *str, int count) {
    int base = 10;
    int ptr = 0;
    int num = 0;
    if (str[0]=='$') {
        base = 16;
        ptr++;
    }
    while (ptr<count) {
        num *= base;
        int digit = number_char_to_digit(str[ptr]);
        if (digit==NUMBER_ERROR || digit >=base) {
            ERROR("Can not interpret number: '%.*s'\n", count, str);
            return NUMBER_ERROR;
        }
        
        num += digit;
        ptr++;
    }
    return num;    
}

/**
 * interpret a string as a constant, label or number
 * - get the value of a constant
 * - parse number
 * Returns NUMBER_ERROR on error, or NUMBER_LABEL_NODEF if number is a valid label with no value
 */
int number_get_number(State *s, char *str, int count) {
    int ptr = 0;
    if (str[ptr]=='#')
        ptr++;
    if (str[ptr]=='@') {
        // constant parsing
        char number[MAP_MAX_KEY_LEN];
        strncpy(number, str+ptr+1, count-ptr-1);
        number[count-ptr-1] = 0;
        int n = map_get(s->defines, number);
        if (n==-1) {
            ERROR("Undefined constant: %.*s\n", count-ptr-1, str+ptr+1);
            return -1;
        }
        return n;
    }
    if (str[ptr]=='&') {
        // label parsing
        char number[MAP_MAX_KEY_LEN];
        strncpy(number, str+ptr+1, count-ptr-1);
        number[count-ptr-1] = 0;
        int n = map_get(s->labels, number);
        if (n==-1) {
            LOG(2, "Undefined label: %.*s\n", count-ptr-1, str+ptr+1);
            return NUMBER_LABEL_NODEF;
        }
        return n;
    }
    // pass error (NUMBER_ERROR value)
    return number_parse_number(str+ptr, count-ptr);
}