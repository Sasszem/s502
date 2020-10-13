
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
    LOG("Getting %s\n", name);
    struct Define *p = defines_find(d, name);
    if (p==NULL)
        return -1;
    return p->value;
   
}


void defines_debug_print(Defines *d) {
    struct Define *ptr = d->head;
    while(ptr!=NULL) {
        printf("\t%s:\t\t%d\n", ptr->name, ptr->value);
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
    LOGDO(defines_debug_print(&d));
    LOG("DEF1: %d\n", defines_get(&d, "DEF1"));
    defines_delete(&d);
    LOG("DELETE\n");
    LOG("DEF1: %d\n", defines_get(&d, "DEF1"));
    LOGDO(defines_debug_print(&d));
}

typedef struct {int n;} Labels;


typedef struct TokensListElement {
    Token token;
    struct TokensListElement *next, *prev;
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
    elem->prev = NULL;
    elem->token = t;
    if (list->head==NULL) {
        list->head = elem;
        list->tail = elem;
        return;
    }
    list->tail->next = elem;
    elem->prev = list->tail;
    list->tail = elem;
}

void tokenslist_remove(TokensList *list, TokensListElement *el) {
    if (list->head == el)
        list->head = el->next;
    if (list->tail == el)
        list->tail = el->prev;
    
    if (el->next!=NULL)
        el->next->prev = el->prev;
    if (el->prev!=NULL) {
        el->prev->next = el->next;
    }
    free(el);
}

void tokenslist_delete(TokensList *list) {
    while (list->head!=NULL) {
        TokensListElement *n = list->head->next;
        free(list->head);
        list->head = n;
    }
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
    LOGDO(token_print(t));
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
    printf("\t%s:%d:%d\t\t%.*s\n", token->source.fname, token->source.lineno, token->len ,token->len, token->stripped);
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
        t->type == TT_INSTR;
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
 * Pretty-print all tokens in a list
 */
void tokenslist_debug_print(TokensList *list) {
    TokensListElement *ptr = list->head;
    LOG("Dumping code:\n");

    // pretty suprised this is valid...
    LOGDO(
        while (ptr!=NULL) {
            token_print(&(ptr->token));
            ptr = ptr->next;
        }
    );
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
            ERROR("Can not recognize token:\n");
            token_print(&(ptr->token));
            tokenslist_delete(t);
            free(t);
            return -1;
        }
        ptr = ptr->next;
    }
    return 0;
}

char* util_find_string_segment(char *ptr) {
    char *end = ptr;
    while(*end!=' ' && *end!='\0') end++;
    return end;
}

int char_to_digit(char c) {
    if ('0'<=c && c<='9')
        return c-'0';
    if ('a'<=c && c<='f')
        c += 'A'-'a';
    if ('A'<=c && c<='F')
        return c-'A'+10;
    return -1;
}


int parse_number(char *str, int count) {
    int base = 10;
    int ptr = 0;
    int num = 0;
    if (str[0]=='$') {
        base = 16;
        ptr++;
    }
    while (ptr<count) {
        num *= base;
        int digit = char_to_digit(str[ptr]);
        if (digit==-1 || digit >=base) {
            ERROR("Can not interpret number: %.*s\n", count, str);
            return -1;
        }
        
        num += digit;
        ptr++;
    }
    return num;    
}

int get_number(State *s, char *str, int count) {
    int ptr = 0;
    if (str[ptr]=='#')
        ptr++;
    if (str[ptr]=='@') {
        char number[DEFINE_MAX_LEN];
        strncpy(number, str+ptr+1, count-ptr-1);
        number[count-ptr-1] = 0;
        int n = defines_get(&(s->defines), number);
        if (n==-1) {
            ERROR("Undefined constant: %.*s\n", count-ptr-1, str+ptr+1);
            return -1;
        }
        return n;
    }
    // pass error (-1 value)
    return parse_number(str+ptr, count-ptr);
}

int process_define(State *s, Token t) {
    
    // find string
    char *define = t.stripped + 1;
    
    char *name = util_find_string_segment(define) + 1;
    if (*(name-1)=='\0') {
        ERROR("Not enough args to define!\n");
        token_print(&t);
        return -1;
    }

    char *num = util_find_string_segment(name) + 1;
    if (*(num-1)=='\0') {
        ERROR("Not enough args to define!\n");
        token_print(&t);
        return -1;
    }

    char *nend = util_find_string_segment(num);
    if (*nend!='\0') {
        ERROR("Too many args to define!\n");
        token_print(&t);
        return -1;
    }

    //LOG("First part:\t\t%.*s\n", name-define, define);
    //LOG("Second part:\t\t%.*s\n", num-name, name);
    //LOG("Third part:\t\t%.*s\n", nend-num, num);
    int as_num = get_number(s, num, nend-num);
    if (as_num<0)
        return -1;
    char def[DEFINE_MAX_LEN];
    strncpy(def, name, num-name);
    def[num-name-1] = 0;
    defines_set(&(s->defines), def, as_num);
}

enum PPCommand {
    PPC_STOP = -1,
    PPC_NOP = 0,
    PPC_INC_IF_LVL,
    PPC_DEC_IF_LVL,
};

enum PPCommand process_include(State *s, Token t) {
    ERROR("INCLUDE NOT IMPLEMENTED YET!\n");

    return PPC_STOP;
}

enum PPCommand do_preprocessor_token(State *s, Token t) {
    LOG("Processing preproessor token:\n");
    LOGDO(token_print(&t));
    
    char *f = t.stripped + 1;
    char *end = util_find_string_segment(f);
    //LOG("1st segment: %.*s\n", end-f, f);

    if (strncmp(t.stripped+1, "define", strlen("define"))==0) {
        //LOG("Found a define...\n");
        return process_define(s, t)<0 ? PPC_STOP : PPC_NOP;
    }
    if (strncmp(t.stripped+1, "include", strlen("include"))==0) {
        return process_include(s, t)<0 ? PPC_STOP : PPC_NOP;
    }
    ERROR("Unknown preprocessor directive: %s\n", f);
    return PPC_STOP;
}
/**
 * Goes trough the tokens in the list
 * - handles defines
 * (- does includes)
 * (- reads trough ifdef'd blocks)
 */
int preprocess(State *s, TokensList *tokens) {
    TokensListElement *ptr = tokens->head;
    int iflevel = 0;
    int skip = 0;
    while (ptr!=NULL) {
        Token t = ptr->token;
        if (t.type == TT_PREPROC) {
            skip = 1;

            enum PPCommand p = do_preprocessor_token(s, t);
            
            if (p==PPC_INC_IF_LVL)
                iflevel++;
            if (p==PPC_DEC_IF_LVL)
                iflevel = iflevel==0 ? 0 : iflevel-1;
            if (p==PPC_STOP)
                return -1;
        }

        ptr = ptr->next;
        if (skip==1 || iflevel>0){
            tokenslist_remove(tokens, ptr->prev);
        } 
        skip = 0;
    }
    return 0;
}

State* state_make() {
    State *ret = (State*)malloc(sizeof(State));
    
    ret->defines.head = NULL;
    ret->defines.tail = NULL;
    
    ret->labels.n = 0;

    ret->tokens.head = NULL;
    ret->tokens.tail = NULL;

    return ret;
}


TokensList* load_file(State *s, char* name) {
    LOG("Reading file...\n");
    TokensList *list = read_file("test.asm");
    if (list==NULL)
        return NULL;
    LOG("Running first analysis...\n");
    if (recognize_tokens(list)<0)
        return NULL;
    if (preprocess(s, list)<0) {
        tokenslist_delete(list);
        free(list);
        return NULL;
    }
    return list;
}

void labels_delete(Labels *l) {

}

void state_delete(State *s) {
    defines_delete(&(s->defines));
    tokenslist_delete(&(s->tokens));
    labels_delete(&(s->labels));
}

int main() {
    //defines_test();
    //return 0;
    State *state = state_make();
    TokensList *list = load_file(state, "test.asm");

    if (list==NULL) {
        state_delete(state);
        free(state);
        return -1;
    }
    LOG("Now dunping tha file: \n");
    LOGDO(tokenslist_debug_print(list));
    LOG("Now tha defines:\n");
    LOGDO(defines_debug_print(&(state->defines)));
    LOG("Cleaning up...\n");
    tokenslist_delete(list);
    state_delete(state);
    free(state);
    free(list);
    return 0;
}