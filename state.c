enum tokenType {
    TT_INSTR,
    TT_PREPROC,
    TT_LABEL
};

enum AddressMode {
    AM_IMPLIED,
    AM_INDIRECT,
    AM_INVALID
}; 

enum TokenPreprocType {
    TPT_INCLUDE,
    TPT_DEFINE,
    TPT_ORG,
    TPT_DATA,
    TPT_START
};

struct TokenLabel {
    int addr;
};

struct TokenPreproc {
    enum TokenPreprocType type;
};

struct TokenInstr {
    enum AddressMode addressmode;
};

// TODO: define-t enumra cserélni
#define TOKEN_BUFFER_SIZE 32
#define TOKEN_SOURCE_FILE_SIZE 32

typedef struct {
    int binSize;
    enum tokenType type;
    union {
        struct TokenLabel label;
        struct TokenPreproc preproc;
        struct TokenInstr instr;
    } fields;
    char stripped[TOKEN_BUFFER_SIZE];
    int len;
    struct {
        char fname[TOKEN_SOURCE_FILE_SIZE];
        int lineno;
    } source;
} Token;

typedef struct {int n;} Defines;
typedef struct {int n;} Labels;


typedef struct TokensListElement {
    Token token;
    struct TokensListElement *next;
} TokensListElement;

typedef struct {
    TokensListElement *head;
    TokensListElement *tail;
} TokensList;

typedef struct {
    Defines defines;
    Labels labels;
    TokensList tokens;

} State;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "debugmalloc.h"
/**
 * Create a new (empty) linekd list for tokens 
 */
TokensList* tokenslist_make() {
    TokensList *ret = (TokensList*)malloc(sizeof(TokensList));
    ret->head = NULL;
    ret->tail = NULL;
}

/**
 * Add a token to a linekd list of tokens
 */
void tokenslist_add(TokensList *list, Token t) {
    TokensListElement *elem = (TokensListElement*)malloc(sizeof(TokensListElement));
    elem->next = NULL;
    elem->token = t;
    if (list->head==NULL) {
        list->head = elem;
        list->tail = elem;
        return;
    }
    list->tail->next = elem;
    list->tail = elem;
}

void tokenslist_delete(TokensList *list) {
    while (list->head!=NULL) {
        TokensListElement *n = list->head->next;
        free(list->head);
        list->head = n;
    }
    free(list);
}


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
    for (ptr = 0; ptr<TOKEN_BUFFER_SIZE; ) {
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
    if (ptr==TOKEN_BUFFER_SIZE) {
        t->len = ptr;
        return -1;
    }
    if (t->stripped[ptr-1]==' ')
        ptr--;
    
    t->len = ptr;
    if (c==EOF)
        return 0;
    return 1;
}


/**
 * Pretty-print one token, with its source and length
 */
void token_print(Token *token) {
    printf("%s:%d:%d\t\t%.*s\n", token->source.fname, token->source.lineno, token->len ,token->len, token->stripped);
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
    if (t->stripped[3]==' ') {
        t->type == TT_INSTR;
        found++;
    }
    if (found!=1) {
        return -1;
    }
    printf("Recognized token as %d:\n", t->type);
    token_print(t);
    return 0;
} 

/**
 * Pretty-print all tokens in a list
 */
void tokenslist_debug_print(TokensList *list) {
    TokensListElement *ptr = list->head;
    while (ptr!=NULL) {
        token_print(&(ptr->token));
        ptr = ptr->next;
    }
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
    while ((res=read_token(f, &tok))>0) {
        if (tok.len>0) {
            strncpy(tok.source.fname, name, TOKEN_SOURCE_FILE_SIZE);
            tok.source.lineno = lineno;
            tokenslist_add(tokenslist ,tok);
        }
        lineno++;
    }
    fclose(f);
    if (res<0) {
        printf("ERROR: line is too long!\n");
        token_print(&tok);
        tokenslist_delete(tokenslist);
        return NULL;
    }
    return tokenslist;
}

int recognize_tokens(TokensList *t) {
    TokensListElement *ptr = t->head;
    while (ptr!=NULL) {
        int res = recognize_token(&(ptr->token));
        if (res<0){
            printf("ERROR: Can not recognize token:");
            token_print(&(ptr->token));
            tokenslist_delete(t);
            return -1;
        }
        ptr = ptr->next;
    }
    return 0;
}

int main() {
    printf("Reading file...\n");
    TokensList *list = read_file("test.asm");
    if (list==NULL)
        return -1;
    printf("Running first analysis...\n");
    if (recognize_tokens(list)<0)
        return -1;
    printf("Now dunping tha file: \n");
    tokenslist_debug_print(list);
    printf("Cleaning up...\n");
    tokenslist_delete(list);
    return 0;
}