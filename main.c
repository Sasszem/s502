#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

#include "map.h"
#include "tokenslist.h"
#include "state.h"
#include "loadfile.h"
#include "pass_one.h"
#include "instructions.h"

#include "tokenFunc.h"
int pass_two(State *s, TokensList *t) {
    for (TokensListElement* ptr = t->head; ptr!=NULL; ptr = ptr->next) {
        int ret = token_get_operand(s, ptr->token);
        if (ret<0 || ptr->token->fields.instr.number<0) {
            if (ret==0) {
                ERROR("Undefined label reference!\n");
            }
            token_print(ptr->token);
            FAIL("Pass two failed!\n");
            return -1;
        } 
    }
    return 0;
}

int token_compile(Token *t, char** dataptr) {
    if (t->type!=TT_INSTR) {
        ERROR("Not implemented yet!\n");
        return 0;
    }
    int size = 1 + ADRM_SIZES[t->fields.instr.addressmode];
    char *data = malloc(size);
    *dataptr = data;
    data[0] = t->fields.instr.inst->opcs[t->fields.instr.addressmode];
    if (size>1) {
        data[1] = t->fields.instr.number & 0xff;
    }
    if (size>2) {
        data[2] = (t->fields.instr.number>>8) & 0xff;
    }
    return size;
}


int write_data(State *s, TokensList *t) {
    FILE *f = fopen("out", "wb");
    if (!f) {
        
    }
    for (TokensListElement *ptr = t->head; ptr!=NULL; ptr = ptr->next) {
        char *data;
        int n = token_compile(ptr->token, &data);
        token_print(ptr->token);
        printf("Data: ");
        for (int i = 0; i<n; i++) {
            printf("%02hhx ", data[i]);
        }
        printf("\n");
        free(data);
    }
    return 0;
}

int main(int argc, char**argv) {
    logging_level(0);
    State *state = state_new();
    if (!state) goto ERR_INIT;
    if (state_load_instr(state, "opcodes.csv")<0) goto ERR_INIT;
    if (state_parse_commandline(state, argc, argv)<0) goto ERR_INIT;
    
    LOG(2, "Init done!\n");

    TokensList *list = load_file("test.asm");
    if (!list) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }

    if (pass_one(state, list)<0) {
        FAIL("Compilation failed!\n");
        tokenslist_free(list);
        list = NULL;
        state_free(state);
        state = NULL;
        return -1;
    }
    
    if (pass_two(state, list)<0) {
        FAIL("Compilation failed!\n");
        tokenslist_free(list);
        list = NULL;
        state_free(state);
        state = NULL;
        return -1;
    }
    write_data(state, list);

    LOG(2, "Now dunping tha file: \n");
    LOGDO(2, tokenslist_debug_print(list));
    LOG(2, "Now tha defines:\n");
    LOGDO(2, map_debug_print(state->defines));
    LOG(2, "And the labels:\n");
    LOGDO(2, map_debug_print(state->labels));
    LOG(2, "Cleaning up...\n");
    tokenslist_free(list);
    state_free(state);
    state = NULL;
    list = NULL;

    return 0;
    
    ERR_INIT:
        FAIL("Initialization failed!\n");
        state_free(state);
        return -1;
}