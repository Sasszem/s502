#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "logging.h"

#include "state.h"
#include "loadfile.h"
#include "pass_one.h"
#include "instructions.h"

#include "tokenFunc.h"
int pass_two(State *s) {
    for (TokensListElement* ptr = s->tokens->head; ptr!=NULL; ptr = ptr->next) {
        int ret = token_get_operand(s, ptr->token);
        if (ret<0 || ptr->token->instr.number<0) {
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
        return -1;
    }
    int size = 1 + ADRM_SIZES[t->instr.addressmode];
    char *data = malloc(size);
    *dataptr = data;
    if (t->instr.addressmode==ADRM_REL) {
        //ERROR("Not implemented!\n");
        //return 0;
        int n = t->instr.number - t->instr.address - 2;
        
        if (CHAR_MIN>n || CHAR_MAX<n) {
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


int write_data(State *s) {
    int len = 0;
    for (TokensListElement *ptr = s->tokens->head; ptr!=NULL; ptr = ptr->next) len += ptr->token->binSize;

    char *data = malloc(len);
    int j = 0;
    for (TokensListElement *ptr = s->tokens->head; ptr!=NULL; ptr = ptr->next) {
        char *tdata;
        int n = token_compile(ptr->token, &tdata);
        if (n<0) {
            FAIL("Could not compile data!\n");
            free(tdata);
            return -1;
        }
        for (int i = 0; i<ptr->token->binSize; i++) data[j+i] = tdata[i];
        j += ptr->token->binSize;
        free(tdata);
    }


    FILE *f = fopen(s->outfile, "wb");
    if (!f) {
        
        ERROR("An error occured opening the file %s!\n", s->outfile);
        ERROR("Error opening file: %s\n", strerror(errno));
        return -1;
    }
    fwrite(data, 1, len, f);
    free(data);
    fclose(f);
    ERROR("Done!\n");
    return 0;
}

int main(int argc, char**argv) {
    logging_level(0);
    State *state = state_new();
    if (!state) goto ERR_INIT;
    if (state_load_instr(state, "opcodes.csv")<0) goto ERR_INIT;
    if (state_parse_commandline(state, argc, argv)<0) goto ERR_INIT;
    
    LOG(2, "Init done!\n");

    state->tokens = load_file(state->infile);
    if (!state->tokens) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }

    if (pass_one(state)<0) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }
    
    if (pass_two(state)<0) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;
    }
    if (write_data(state)<0) {
        FAIL("Compilation failed!\n");
        state_free(state);
        state = NULL;
        return -1;  
    }

    LOG(2, "Now dunping tha file: \n");
    LOGDO(2, tokenslist_debug_print(state->tokens));
    LOG(2, "Now tha defines:\n");
    LOGDO(2, map_debug_print(state->defines));
    LOG(2, "And the labels:\n");
    LOGDO(2, map_debug_print(state->labels));
    LOG(2, "Cleaning up...\n");
    state_free(state);
    state = NULL;

    return 0;
    
    ERR_INIT:
        FAIL("Initialization failed!\n");
        state_free(state);
        return -1;
}