#include "tokenFunc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "number.h"
#include "state.h"
#include "logging.h"
#include "debugmalloc.h"
/**
 * Pretty-print one token, with its source and length
 */
void token_print(Token* token) {
    printf("\t%s:%d:%d\t\t'%.*s'\n", token->source.fname, token->source.lineno, token->len, token->len, token->stripped);
}

/**
 * @brief find and link the instruction entry for a token
 * @return 0 on success, -1 on error
 */
int token_link_instruction(State* s, Token* token) {
    token->instr.inst = instruction_find(s->instr, token->stripped);
    if (token->instr.inst == NULL) return -1;
    return 0;
}

/*
Addressing modes

The 6502 supports 13 addressing modes:

Accumlator      OPC A

Absolute        OPC $AABB
Absolute, X     OPC $AABB, X
Absolute, Y     OPC $AABB, Y

Immidiate       OPC #$BB
Implied         OPC

Indirect        OPC ($AABB)
Indirect, X     OPC ($BB, X)
Indirect, Y     OCP ($BB), Y

Relative        OPC $BB

Zeropage        OPC *$BB
Zeropage, X     OPC *$BB, X
Zeropage, Y     OPC *$BB, Y

There are a few caviats though

First, relative indexing (which is used with jumps) should actually take a 16 bit address, and the assembler should take the difference or return an error, but it should NOT support setting the operand by hand
In either case, just by its form, relative and absolute is not easily distinguishable

BUT!
There is no instruction with a relative A-mode and also any other mode, so if the instruction CAN be rel, it MUST be it

Also, in the place of any number, there can also be a label or a define location

For these reasons, addr mode identification works the following way:

1) Token ends after OPC (len=3)         => Implied

2) Token len is 5,
ends with  'A' or 'a'                   => Accumlator

3) 4th char is a #                      => Immidiate

4) Can it be relative?
(based on the mnemonic)                 => Relative

(we now got rid of most simple cases)

5) 4th char is a *
- count X, Y and , chars
- validate combinations                 => Zeropage and variants

6) 4th char is a (
- count X, Y, comma and )
- validate                              => Indirect and variants

7) It's some kind of absolute
- count X, Y and comma
- validate                              => Absolute and variants


This _should_ work, but is not perfect. It should be possible to refactor this into a state machine and clean it up this way.

*/

/**
 * Determine the address mode of a token
 * Returns 0 on success and -1 on error
 * (modifies the token in-place)
 */
int token_get_addressmode(Token* t) {

    // step 1 - implied
    if (t->len == 3) {
        t->instr.addressmode = ADRM_IMP;
        return 0;
    }


    // step 2 - acc
    if (t->len == 5 && util_match_char(t->stripped[4], 'a')) {
        t->instr.addressmode = ADRM_ACC;
        return 0;
    }


    // step 3 - imm
    if (t->stripped[4] == '#') {
        t->instr.addressmode = ADRM_IMM;
        return 0;
    }


    // step 4 - relative
    if (t->instr.inst->opcs[ADRM_REL] != OPC_INVALID) {
        // this can ONLY be a relative
        t->instr.addressmode = ADRM_REL;
        return 0;
    }



    // step 5,6,7 have a few checks in common
    // so we can avoid code duplication by fatoring that out

    int s_x = 0, s_y = 0, s_close = 0, s_sep = 0; // seen x, y, close or coma

    for (int i = 0; i < t->len; i++) {
        if (util_match_char(t->stripped[i], 'x') && s_sep)
            s_x = 1;
        if (util_match_char(t->stripped[i], 'y') && s_sep)
            s_y = 1;
        if (t->stripped[i] == ',')
            s_sep = 1;
        if (t->stripped[i] == ')')
            s_close = 1;
    }

    // must have a separator if it has an index
    if ((s_x || s_y) && !s_sep)
        return -1;

    // can not have both indexes
    if (s_x && s_y)
        return -1;

    // end of common chekcs


    // step 5 - zeropage
    if (t->stripped[4] == '*') {
        // must NOT have a close
        if (s_close)
            return -1;

        // no index - normal zpg
        if (!(s_x || s_y)) {
            t->instr.addressmode = ADRM_ZPG;
            return 0;
        }

        // Set according to index
        t->instr.addressmode = s_x ? ADRM_ZPG_X : ADRM_ZPG_Y;
        return 0;
    }

    // step 6 - indirect
    if (t->stripped[4] == '(') {
        // must have a close )
        if (!s_close)
            return -1;

        // no index - normal inderect
        if (!(s_x || s_y)) {
            t->instr.addressmode = ADRM_IND;
            return 0;
        }

        // Set according to index
        t->instr.addressmode = s_x ? ADRM_IND_X : ADRM_IND_Y;
        return 0;
    }


    // step 7 - absolute
    {
        // must not have a close )
        if (s_close)
            return -1;

        // no index - normal absolute
        if (!(s_x || s_y)) {
            t->instr.addressmode = ADRM_ABS;
            return 0;
        }

        // Set according to index
        t->instr.addressmode = s_x ? ADRM_ABS_X : ADRM_ABS_Y;
        return 0;
    }

    return -1;
}

int token_analyze_instruction(State* s, Token* t) {
    if (token_link_instruction(s, t) < 0) {
        ERROR("Unknown instruction!\n");
        goto ERR;


    }
    if (token_get_addressmode(t) < 0) {
        ERROR("Can not determine instruction address mode!\n");
        goto ERR;
    }
    if (t->instr.inst->opcs[t->instr.addressmode] == OPC_INVALID) {
        ERROR("Invalid instruction-addressmode combination!\n");
        ERROR("A-mode: %s\n", ADRM_NAMES[t->instr.addressmode]);
        goto ERR;
    }

    t->binSize = 1 + ADRM_SIZES[t->instr.addressmode];

    return 0;


ERR:
    token_print(t);
    FAIL("Error while analyzing token: \n");
    return -1;
}

/**
 * @brief Parse token - test if it's an opcode, a label or a preprocessor statement
 * @param t token to recognize - will be modified in-place
 * @returns 0 on success, -1 on error
 */
int token_recognize(Token* t) {
    // how many token types does it fit
    int found = 0;

    // directive - starts with a dot
    if (t->stripped[0] == '.') {
        t->type = TT_PREPROC;
        found++;
    }

    // label - ends with a ':'
    if (t->stripped[t->len - 1] == ':') {
        t->type = TT_LABEL;
        found++;
    }

    // instruction - 3rd char is a space or len is 3
    if (t->stripped[3] == ' ' || t->stripped[3] == '\0') {
        t->type = TT_INSTR;
        t->instr.number = -1;
        found++;
    }

    // 0 or more than one match is a problem
    if (found != 1) {
        ERROR("Can not recognize token:\n");
        token_print(t);
        return -1;
    }

    LOG(4, "Recognized token as %d:\n", t->type);
    LOGDO(4, token_print(t));

    return 0;
}

int token_get_operand(State *s, Token* t) {
    if (t->binSize==1) {
        t->instr.number = 0;
        return 0;
    }

    char buf[MAP_MAX_KEY_LEN];
    char *begin = &t->stripped[4];
    char *end;
    for (; *begin != 0 && (*begin == ' ' || *begin == '*' || *begin=='(' || *begin=='#'); begin++);
    for (end = begin; *end!=0 && *end!=')' && *end!=',' && *end!=' '; end++); 
    end--;
    strncpy(buf, begin, end-begin+1);
    int n = number_get_number(s, buf, end-begin+1);
    if (n==NUMBER_ERROR) {
        FAIL("Opcode operand parsing failed!\n");
        return -1;
    }
    if (n==NUMBER_LABEL_NODEF) {
        return 0;
    }
    t->instr.number = n;
    return 0;
}

int token_compile(Token *t, char** dataptr) {
    if (t->type!=TT_INSTR) {
        ERROR("Not implemented yet!\n");
        return -1;
    }
    int size = 1 + ADRM_SIZES[t->instr.addressmode];
    char *data = malloc(size);
    *dataptr = data;
    if (t->instr.addressmode==ADRM_REL) {
        //ERROR("Not implemented!\n");
        //return 0;
        int n = t->instr.number - t->instr.address - 2;
        
        if (-128>n || 127<n) {
            ERROR("Relative addressing jump too far!\n");
            printf("Target: %x, from: %x (diff: %x)\n", t->instr.number, t->instr.address, n);
            token_print(t);
            return -1;
        }
        t->instr.number = n;
    }
    data[0] = t->instr.inst->opcs[t->instr.addressmode];
    if (size>1) {
        data[1] = t->instr.number & 0xff;
    }
    if (size>2) {
        data[2] = (t->instr.number>>8) & 0xff;
    }
    return 0;
}