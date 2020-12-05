#include <string.h>

#include "number.h"
#include "logging.h"


int number_char_to_digit(char c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    if ('a' <= c && c <= 'f')
        c += 'A' - 'a';
    if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    return NUMBER_ERROR;
}

int number_parse_number(char* str) {
    int count = strlen(str);

    int base = 10;
    int ptr = 0;
    int num = 0;
    if (str[0] == '$') {
        base = 16;
        ptr++;
    }
    while (ptr < count) {
        num *= base;
        int digit = number_char_to_digit(str[ptr]);
        if (digit == NUMBER_ERROR || digit >= base) {
            ERROR("Can not interpret number: '%.*s'\n", count, str);
            return NUMBER_ERROR;
        }

        num += digit;
        ptr++;
    }
    return num;
}

int number_get_raw(State *s, char* str) {
    if (str[0] == '@' || str[0] == '&') {
        Map* m = str[0] == '@' ? s->defines : s->labels;
        int n = map_get(m, &str[1]);
        if (n < 0) {
            return str[0] == '@' ? NUMBER_ERROR : NUMBER_LABEL_NODEF;
        }
        return n;
    }
    // pass error (NUMBER_ERROR value)
    return number_parse_number(str);
}

int number_get_number(State* s, char* str) {

    int p = (str[0]=='>' ||str[0]=='<')? 1:0;
    int num = number_get_raw(s, &str[p]);
    
    if (num<0) return num;

    num = str[0] == '>' ? num>>8 : num;
    num &= str[0]=='>' || str[0]=='<' ? 0xff : 0xffff;

    return num;
}

void number_print_err(int num, char* str) {
    ERROR("Undefined %s: %s\n", str[0] == '@' ? "constant" : "label", &str[1]);
}