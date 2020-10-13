
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "debugmalloc.h"

#define LOGGING 1
#include "logging.h"

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

#define DEFINE_MAX_LEN 16

struct Define {
    int value;
    char name[DEFINE_MAX_LEN];
    struct Define *next;
};

typedef struct {
    struct Define *head, *tail;
} Defines;


struct Define* defines_find(Defines *d, char *key) {
    struct Define *ptr = d->head;
    while(ptr!=NULL) {
        if (strncmp(ptr->name, key, DEFINE_MAX_LEN)==0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}


void defines_set(Defines *d, char *name, int value) {
    struct Define *ptr = defines_find(d, name);
    if (ptr==0) {
        ptr = (struct Define*)malloc(sizeof(struct Define));    
        ptr->next = NULL;
        if (d->head==NULL) {
            d->head = ptr;
            d->tail = ptr;
        } else {
            d->tail->next = ptr;
            d->tail = ptr; 
        }
        strncpy(ptr->name, name, DEFINE_MAX_LEN);
    }

    ptr->value = value;

}

void defines_delete(Defines *d) {
    struct Define *ptr;
    while (d->head!=NULL) {
        ptr = d->head->next;
        free(d->head);
        d->head = ptr;
    }
    d->head = NULL;
    d->tail = NULL;
}



/**
 * Get the value of a define by name
 * Returns -1 on not found
 */
int defines_get(Defines *d, char *name) {
    struct Define *p = defines_find(d, name);
    if (p==NULL)
        return -1;
    return p->value;
   
}


void defines_debug_print(Defines *d) {
    struct Define *ptr = d->head;
    while(ptr!=NULL) {
        LOG("%s:\t\t%d\n", ptr->name, ptr->value);
        ptr = ptr->next;
    }
}

void defines_test() {
    Defines d = {NULL, NULL};
    defines_set(&d, "DEF1", 1234);
    defines_set(&d, "DEF2", 4567);
    defines_debug_print(&d);
    LOG("DEF1: %d\n", defines_get(&d, "DEF1"));
    defines_set(&d, "DEF1", 4321);
    defines_debug_print(&d);
    LOG("DEF1: %d\n", defines_get(&d, "DEF1"));
    defines_delete(&d);
    LOG("DELETE\n");
    LOG("DEF1: %d\n", defines_get(&d, "DEF1"));
    defines_debug_print(&d);
}

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
    token_print(t);
    t->stripped[ptr] = 0;
    t->len = ptr;
    if (c==EOF)
        return 0;
    return 1;
}


/**
 * Pretty-print one token, with its source and length
 */
void token_print(Token *token) {
    LOGC("\t%s:%d:%d\t\t%.*s\n", token->source.fname, token->source.lineno, token->len ,token->len, token->stripped);
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
        ERROR("Can not recognize token:\n");
        token_print(t);
        return -1;
    }
    LOG("Recognized token as %d:\n", t->type);
    token_print(t);
    return 0;
} 

/**
 * Pretty-print all tokens in a list
 */
void tokenslist_debug_print(TokensList *list) {
    TokensListElement *ptr = list->head;
    LOG("Dumping code:\n");
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
        return NULL;
    }
    return tokenslist;
}

int recognize_tokens(TokensList *t) {
    TokensListElement *ptr = t->head;
    while (ptr!=NULL) {
        int res = recognize_token(&(ptr->token));
        if (res<0){
            ERROR("Can not recognize token:\n");
            token_print(&(ptr->token));
            tokenslist_delete(t);
            return -1;
        }
        ptr = ptr->next;
    }
    return 0;
}

TokensList* load_file(char* name) {
    TokensList *tl = read_file(name);
    LOG("Reading file...\n");
    TokensList *list = read_file("test.asm");
    if (list==NULL)
        return NULL;
    LOG("Running first analysis...\n");
    if (recognize_tokens(list)<0)
        return NULL;
    
    return tl;
}

void preprocess(TokensList *tokens) {

}

int main() {
    //defines_test();
    //return 0;
    TokensList *list = load_file("test.asm");
    LOG("Now dunping tha file: \n");
    tokenslist_debug_print(list);
    LOG("Cleaning up...\n");
    tokenslist_delete(list);
    return 0;
}