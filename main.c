#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "debugmalloc.h"

#define LOGGING 1
#include "logging.h"

#include "token.h"
#include "map.h"
#include "tokenslist.h"

typedef struct {int n;} Labels;



typedef struct {
    Map defines;
    Labels labels;
    TokensList tokens;

} State;



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
        int n = map_get(&(s->defines), number);
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
    map_set(&(s->defines), def, as_num);
    return 0;
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
    map_empty(&(s->defines));
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
    LOGDO(map_debug_print(&(state->defines)));
    LOG("Cleaning up...\n");
    tokenslist_delete(list);
    state_delete(state);
    free(state);
    free(list);
    return 0;
}