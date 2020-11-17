#include "tokenFunc.h"
#include <stdio.h>
#include "util.h"

#include "state.h"

/**
 * Pretty-print one token, with its source and length
 */
void token_print(Token *token) {
    printf("\t%s:%d:%d\t\t'%.*s'\n", token->source.fname, token->source.lineno, token->len ,token->len, token->stripped);
}

/**
 * @brief find and link the instruction entry for a token
 * @return 0 on success, -1 on error
 */
int token_link_instruction(State *s, Token *token) {
    token->fields.instr.inst = instruction_find(s->instr, token->stripped);
    if (token->fields.instr.inst==NULL) return -1;
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
int token_get_addressmode(Token *t) {

    // step 1 - implied
    if (t->len == 3) {
        t->fields.instr.addressmode = ADRM_IMP;
        return 0;
    }


    // step 2 - acc
    if (t->len==5 && util_match_char(t->stripped[4], 'a')) {
        t->fields.instr.addressmode = ADRM_ACC;
        return 0;
    }


    // step 3 - imm
    if (t->stripped[4] == '#') {
        t->fields.instr.addressmode = ADRM_IMM;
        return 0;
    }


    // step 4 - relative
    if (t->fields.instr.inst->opcs[ADRM_REL]!=OPC_INVALID) {
        // this can ONLY be a relative
        t->fields.instr.addressmode = ADRM_REL;
        return 0;
    }



    // step 5,6,7 have a few checks in common
    // so we can avoid code duplication by fatoring that out

    int s_x = 0, s_y = 0, s_close=0, s_sep = 0; // seen x, y, close or coma
        
    for (int i = 0; i<t->len; i++) {
        if (util_match_char(t->stripped[i], 'x') && s_sep)
            s_x = 1;
        if (util_match_char(t->stripped[i], 'y') && s_sep)
            s_y = 1;
        if (t->stripped[i]==',')
            s_sep = 1;
        if (t->stripped[i]==')')
            s_close = 1;
    }

    // must have a separator if it has an index
    if ((s_x || s_y) && !s_sep)
        return -1;
        
    // can not have both indexes
    if (s_x&&s_y)
        return -1;

    // end of common chekcs


    // step 5 - zeropage
    if (t->stripped[4] == '*') {
        // must NOT have a close
        if (s_close)
            return -1;

        // no index - normal zpg
        if (!(s_x || s_y)) {
            t->fields.instr.addressmode = ADRM_ZPG;
            return 0;
        }

        // Set according to index
        t->fields.instr.addressmode = s_x ? ADRM_ZPG_X : ADRM_ZPG_Y; 
        return 0;
    }

    // step 6 - indirect
    if (t->stripped[4] == '(') {
        // must have a close )
        if (!s_close)
            return -1;
        
        // no index - normal inderect
        if (!(s_x || s_y)) {
            t->fields.instr.addressmode = ADRM_IND;
            return 0;
        }

        // Set according to index
        t->fields.instr.addressmode = s_x ? ADRM_IND_X : ADRM_IND_Y; 
        return 0;
    }


    // step 7 - absolute
    {
        // must not have a close )
        if (s_close)
            return -1;
        
        // no index - normal absolute
        if (!(s_x || s_y)) {
            t->fields.instr.addressmode = ADRM_ABS;
            return 0;
        }

        // Set according to index
        t->fields.instr.addressmode = s_x ? ADRM_ABS_X : ADRM_ABS_Y; 
        return 0;
    }

    return -1;
}