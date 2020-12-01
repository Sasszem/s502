#include "debugmalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "state.h"
#include "pass_twothree.h"
#include "tokenFunc.h"
#include "logging.h"


int pass_two(State* s) {
    for (TokensListElement* ptr = s->tokens->head; ptr != NULL; ptr = ptr->next) {
        int ret = token_get_operand(s, ptr->token);
        if (ret < 0 || ptr->token->instr.number < 0) {
            if (ret == 0) {
                ERROR("Undefined label reference!\n");
            }
            token_print(ptr->token);
            FAIL("Pass two failed!\n");
            return -1;
        }
    }
    return 0;
}

/**
 * @brief compile tokens and concat binary data
 * @param list list of tokens to compile
 * @param n pointer to return length of binary data to
 * @returns puffer to data or NULL on error
 *
 * The resulting buffer should be freed by the callee!
 */
char* concat_bin(State* s, int* n) {
    int len = 0;
    for (TokensListElement* ptr = s->tokens->head; ptr != NULL; ptr = ptr->next) len += ptr->token->binSize;
    *n = len;
    char* data = malloc(len);
    int j = 0;
    for (TokensListElement* ptr = s->tokens->head; ptr != NULL; ptr = ptr->next) {
        char* tdata;
        int n = token_compile(s, ptr->token, &tdata);
        if (n < 0) {
            free(data);
            free(tdata);
            return NULL;
        }
        for (int i = 0; i < ptr->token->binSize; i++) data[j + i] = tdata[i];
        j += ptr->token->binSize;
        free(tdata);
    }

    return data;
}

int write_data(State* s) {
    int l;
    char* data = concat_bin(s, &l);
    if (!data) {
        FAIL("Could not compile data!\n");
        return -1;
    }

    FILE* f = fopen(s->outfile, "wb");
    if (!f) {
        ERROR("An error occured opening the file %s!\n", s->outfile);
        ERROR("Error opening file: %s\n", strerror(errno));
        free(data);
        return -1;
    }
    fwrite(data, 1, l, f);
    free(data);
    fclose(f);
    return 0;
}