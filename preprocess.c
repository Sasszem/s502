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
    PPC_IF_TRUE,
    PPC_IF_FALSE,
    PPC_ENDIF,
};

enum PPCommand process_include(State *s, Token t) {
    ERROR("INCLUDE NOT IMPLEMENTED YET!\n");

    return PPC_STOP;
}

enum PPCommand process_ifdef(State *s, Token t) {
    char *cmd = t.stripped+1;
    char *val = util_find_string_segment(cmd) + 1;
    if (*(val-1)!=' ') {
        ERROR("Too few arguments for ifdef!");
        token_print(&t);
        return PPC_STOP;
    }
    char *vend = util_find_string_segment(val);
    if (*vend!='\0'){
        ERROR("Too many arguments for ifdef!");
        token_print(&t);
        return PPC_STOP;
    }
    char name[DEFINE_MAX_LEN];
    strncpy(name, val, vend-val);
    name[vend-val] = 0;
    if (map_get(&(s->defines), name)!=-1) {
        return PPC_IF_TRUE;
    }
    return PPC_IF_FALSE;
}

enum PPCommand do_preprocessor_token(State *s, Token t, int skip) {
    LOG("Processing preproessor token:\n");
    LOGDO(token_print(&t));
    
    char *f = t.stripped + 1;

    if (strncmp(t.stripped+1, "ifdef", strlen("ifdef"))==0) {
        if (skip)
            return PPC_IF_TRUE;
        return process_ifdef(s, t);
    }

    if (strncmp(t.stripped+1, "endif", strlen("endif"))==0) {
        return PPC_ENDIF;
    }

    if (skip)
        return PPC_NOP;

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
    int skip = 0, skiponce = 0;
    enum PPCommand p;
    while (ptr!=NULL) {
        Token t = ptr->token;
        if (t.type == TT_PREPROC) {
            p = do_preprocessor_token(s, t, skip);

            if (p==PPC_IF_TRUE) {
                iflevel++;
                skiponce = 1;
            }
            if (p==PPC_IF_FALSE) {
                iflevel++;
                skiponce = 1;
                skip = 1;
            }
            if (p==PPC_ENDIF) {
                iflevel--;
                skiponce = 1;
                if (iflevel<0) {
                    ERROR("More endif's than if's!\n");
                    token_print(&t);
                    return -1;
                }
                if (iflevel==0) {
                    skip =0;
                }
            }
            if (p==PPC_STOP)
                return -1;
        }
        if (skip||skiponce){
            ptr = tokenslist_remove(tokens, ptr);
            skiponce = 0;
        } else {
            ptr = ptr->next;
        }
    }
    return 0;
}