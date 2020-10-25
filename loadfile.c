
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "debugmalloc.h"

#include "loadfile.h"
#include "logging.h"

/**
 * Read a single token into a Token struct from file f
 * - read relevant part
 * - return 1 on success, 0 on EOF and -1 on error
 ***/

// TODO: refactor this
// TODO: basic docs
int read_token(FILE *f, Token *t) {
    int seen_whitespace = 1, eol = 0;
    int ptr, c;
    // -1 for trailing nullbyte
    for (ptr = 0; ptr<TOKEN_BUFFER_SIZE-1; ) {
        c = fgetc(f);
        // HANDLE EOF
        if (c==EOF || c=='\n') {
            break;
        }
        if (eol)
            continue;
        if (c=='\t' || c==' ') {
            if (!seen_whitespace) {
                c = ' ';
                seen_whitespace = 1;
            } else {
                continue;
            }
        } else {
            seen_whitespace = 0;
            if (c==';') {
                eol = 1;
                continue;
            }
        }

        t->stripped[ptr++] = c;
    }
    if (ptr==TOKEN_BUFFER_SIZE-1) {
        t->len = ptr;
        return -1;
    }
    if (t->stripped[ptr-1]==' ')
        ptr--;
    LOG("READ TOKEN:\n");
    LOGDO(token_print(t));
    t->stripped[ptr] = 0;
    t->len = ptr;
    if (c==EOF)
        return 0;
    return 1;
}



/**
 * Parse token - test if it's an opcode, a label or a preprocessor statement
 */
int recognize_token(Token *t) {
    int found = 0;
    if (t->stripped[0]=='.') {
        t->type = TT_PREPROC;
        found++;
    }
    if (t->stripped[t->len-1]==':') {
        t->type = TT_LABEL;
        found++;
    }
    if (t->stripped[3]==' ' || t->stripped[3]=='\0') {
        t->type = TT_INSTR;
        found++;
    }
    if (found!=1) {
        ERROR("Can not recognize token:\n");
        token_print(t);
        return -1;
    }
    LOG("Recognized token as %d:\n", t->type);
    LOGDO(token_print(t));
    return 0;
} 


/**
 * High-level func to read the contents from a file
 * - reads file contents
 * - parses every line as a token
 * - generates token list
 * - returns said token list
 */
TokensList* read_file(char *name) {
    FILE *f = fopen(name, "r");
    if (f==NULL) {
        printf("An error occured opening the file %s!\n", name);
        printf("Error opening file: %s\n", strerror( errno ));
        return NULL;
    }
    TokensList *tokenslist = tokenslist_make();
    Token tok;
    int lineno = 1;
    int res;
    do {
        res=read_token(f, &tok);
        if (tok.len>0) {
            strncpy(tok.source.fname, name, TOKEN_SOURCE_FILE_SIZE);
            tok.source.lineno = lineno;
            tokenslist_add(tokenslist ,tok);
        }
        lineno++;
    } while (res>0);

    fclose(f);
    if (res<0) {
        ERROR("line is too long!\n");
        token_print(&tok);
        tokenslist_delete(tokenslist);
        free(tokenslist);
        return NULL;
    }
    return tokenslist;
}

int recognize_tokens(TokensList *t) {
    TokensListElement *ptr = t->head;
    while (ptr!=NULL) {
        int res = recognize_token(&(ptr->token));
        if (res<0){
            return -1;
        }
        ptr = ptr->next;
    }
    return 0;
}

// this should not be here, or rather all the other function shoudl be elsewhere...
// TODO: refactor this
// maybe we can use a monadic pattern here?
TokensList* load_file(char* name) {
    LOG("Reading file...\n");
    TokensList *list = read_file(name);
    if (list==NULL)
        return NULL;
    LOG("Running first analysis...\n");
    if (recognize_tokens(list)<0) {
        tokenslist_delete(list);
        free(list);
        return NULL;
    }
    return list;
}