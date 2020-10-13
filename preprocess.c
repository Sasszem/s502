#include <string.h>

#include "state.h"
#include "token.h"
#include "logging.h"
#include "preprocess.h"
#include "util.h"
#include "number.h"

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
    int as_num = number_get_number(s, num, nend-num);
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