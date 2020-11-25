#include "state.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "pass_twothree.h"
#include "tokenFunc.h"
#include "logging.h"
#include "debugmalloc.h"


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