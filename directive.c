#include <string.h>
#include <stdlib.h>

#include "state.h"
#include "token_t.h"
#include "tokenFunc.h"
#include "logging.h"
#include "directive.h"
#include "util.h"
#include "number.h"
#include "loadfile.h"
#include "istack.h"



/**
 * @brief Token processor function
 * Should process one token and return a DIRCommand
 * 
 * The reason why it takes a TokensListElemnt* and not just a Token* is that we need to do an insert to it's position (in require)
 */
typedef enum DIRCommand(*tokenprocessor)(State *,TokensListElement *ptr);


/**
 * @brief Process an ifbeq directive
 * @return DIR_NOP or DIR_STOP
 * @param s current state
 * @param ptr pointer to element to process
 */
enum DIRCommand process_define(State *s, TokensListElement *ptr) {
    // find string
    char *define = ptr->token->stripped + 1;
    
    char *name = util_find_string_segment(define) + 1;
    if (*(name-1)=='\0') {
        ERROR("Not enough args to define!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }

    char *num = util_find_string_segment(name) + 1;
    if (*(num-1)=='\0') {
        ERROR("Not enough args to define!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }

    char *nend = util_find_string_segment(num);
    if (*nend!='\0') {
        ERROR("Too many args to define!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }

    //LOG("First part:\t\t%.*s\n", name-define, define);
    //LOG("Second part:\t\t%.*s\n", num-name, name);
    //LOG("Third part:\t\t%.*s\n", nend-num, num);
    int as_num = number_get_number(s, num, nend-num);
    if (as_num<0) {
        FAIL("Define argument is not valid!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }
    char def[MAP_MAX_KEY_LEN];
    strncpy(def, name, num-name);
    def[num-name-1] = 0;
    if (map_set(s->defines, def, as_num)<0) {
        FAIL("process_define() failed!\n");
        return DIR_STOP;
    }
    return DIR_NOP;
}

/**
 * @brief Process an ifbeq directive
 * @return DIR_IF_TRUE or DIR_IF_FALSE
 */
enum DIRCommand process_ifbeq(State *s, TokensListElement *ptr) {
    // find string
    char *ifbeq = ptr->token->stripped + 1;
    
    char *first = util_find_string_segment(ifbeq) + 1;
    if (*(first-1)=='\0') {
        ERROR("Not enough args to ifbeq!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }

    char *second = util_find_string_segment(first) + 1;
    if (*(second-1)=='\0') {
        ERROR("Not enough args to ifbeq!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }

    char *send = util_find_string_segment(second);
    if (*send!='\0') {
        ERROR("Too many args to ifbeq!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }

    //LOG("First part:\t\t%.*s\n", name-define, define);
    //LOG("Second part:\t\t%.*s\n", num-name, name);
    //LOG("Third part:\t\t%.*s\n", nend-num, num);
    int first_as_num = number_get_number(s, first, second-first-1);
    if (first_as_num<0) {
        FAIL("ifbeq first argument not defined!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }
    int second_as_num = number_get_number(s, second, send-second);
    if (second_as_num<0) {
        FAIL("ifbeq second argument not defined!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }
    return first_as_num>second_as_num ? DIR_IF_TRUE : DIR_IF_FALSE;
}

/**
 * @brief "Process" an endif directive
 * @returns DIR_ENDIF
 */
enum DIRCommand process_endif(State *s, TokensListElement *ptr){
    return DIR_ENDIF;
}

/**
 * @brief Process a print directive
 * @returns DIR_NOP or DIR_STOP
 */
enum DIRCommand process_print(State *s, TokensListElement *ptr) {
    char *str = &(ptr->token->stripped[1]);
    str = util_find_string_segment(str) + 1;
    if (*(str-1) == 0) {
        ERROR("Print with empty message!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }
    printf("\e[44mMESSAGE\e[49m\t%s\n", str);
    return DIR_NOP;
}


/**
 * @brief Process a printc directive
 * @returns DIR_NOP or DIR_STOP
 */
enum DIRCommand process_printc(State *s, TokensListElement *ptr) {
    char *str = &(ptr->token->stripped[1]);
    str = util_find_string_segment(str) + 1;
    char *send = util_find_string_segment(str);

    if (*send!='\0') {
        ERROR("Too many arguemnts to printc!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }

    char buffer[MAP_MAX_KEY_LEN];
    strncpy(buffer, str, send-str);
    buffer[send-str] = 0;
    int v = map_get(s->defines, buffer);
    printf("\e[44mDEFINE\e[49m:\t%s = ", buffer);

    if (v==-1)
        printf("\e[31mNOT DEFINED\e[39m\n");
    else
        printf("%d\n", v);
    return DIR_NOP;
}

/**
 * @brief Process an include directive
 * @returns DIR_NOP or DIR_STOP
 * 
 * Reads another file and inserts the tokens into the list.
 * This modifies the tokenslist directly!
 */
enum DIRCommand process_include(State *s, TokensListElement *ptr) {
    char *str = &(ptr->token->stripped[1]);
    str = util_find_string_segment(str) + 1;
    if (*(str-1)=='\0') {
        ERROR("Too few argumenst to include!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }
    char *send = util_find_string_segment(str);
    if (*send!='\0') {
        ERROR("Too many arguments to include!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }
    LOG(3, "Including '%.*s'\n", (int)(send-str), str);
    char name[FILENAME_MAX];
    strncpy(name, str, send-str);
    TokensList *f = load_file(name);
    if (f==NULL) {
        FAIL("Could not include file!\n");
        token_print(ptr->token);
        return DIR_STOP;
    }
    tokenslist_insert(s->tokens, ptr, f);
    tokenslist_free(f);
    f = NULL;
    return DIR_NOP;
}

/**
 * @brief Process an ifdef directive
 * @returns DIR_STOP, DIR_IF_TRUE or DIR_IF_FALSE
 */
enum DIRCommand process_ifdef(State *s, TokensListElement *ptr) {
    char *cmd = &(ptr->token->stripped[1]);
    char *val = util_find_string_segment(cmd) + 1;
    if (*(val-1)!=' ') {
        ERROR("Too few arguments for ifdef!");
        token_print(ptr->token);
        return DIR_STOP;
    }
    char *vend = util_find_string_segment(val);
    if (*vend!='\0'){
        ERROR("Too many arguments for ifdef!");
        token_print(ptr->token);
        return DIR_STOP;
    }
    char name[MAP_MAX_KEY_LEN];
    strncpy(name, val, vend-val);
    name[vend-val] = 0;
    if (map_get(s->defines, name)!=-1) {
        return DIR_IF_TRUE;
    }
    return DIR_IF_FALSE;
}

/**
 * @brief Process an ifndef directive
 * @returns DIR_STOP, DIR_IF_TRUE or DIR_IF_FALSE
 * 
 * Uses process_ifdef internally
 */
enum DIRCommand process_ifndef(State *s, TokensListElement *ptr) {
    enum DIRCommand ret = process_ifdef(s, ptr);
    if (ret==DIR_IF_FALSE)
        return DIR_IF_TRUE;
    if (ret==DIR_IF_TRUE)
        return DIR_IF_FALSE;
    return DIR_STOP;
}

/**
 * @brief Process an org directive
 * @returns DIR_STOP or DIR_NOP
 * 
 * Modifies state PC
 */
enum DIRCommand process_org(State *s, TokensListElement *ptr) {
    char *cmd = &(ptr->token->stripped[1]);
    char *val = util_find_string_segment(cmd) + 1;
    if (*(val-1)!=' ') {
        ERROR("Too few arguments for org!");
        token_print(ptr->token);
        return DIR_STOP;
    }
    char *vend = util_find_string_segment(val);
    if (*vend!='\0'){
        ERROR("Too many arguments for org!");
        token_print(ptr->token);
        return DIR_STOP;
    }
    int num = number_get_number(s, val, vend-val);
    if (num<0) {
        if (num<0) {
            ERROR("Can not use undefined labels with org!\n");
        }
        FAIL("process_org() failed!");
        return DIR_STOP;
    }
    s->PC = num;
    LOG(3, "PC = %d\n", num);
    return DIR_NOP;
}

/**
 * @brief The list of all processor functions and their tokens
 * 
 * Their order DOES matter as comparision can not check end of strings as tokens do not end after directive names.
 * (namely printc must come before print or will get falsely reconized as a print) 
 */
struct {tokenprocessor p; char *name;} processors[] = {
    { process_define,   "define"    },
    { process_ifdef,    "ifdef"     },
    { process_printc,   "printc"    },
    { process_print,    "print"     },
    { process_include,  "include"   },
    { process_endif,    "endif"     },
    { process_ifndef,   "ifndef"    },
    { process_ifbeq,    "ifbeq"     },
    { process_org,      "org"       },
};

/**
 * @brief List of tokens to "process" when skipping tokens due to a falsy if
 */
struct {enum DIRCommand ret; char *name;} skipProcessors[] = {
    { DIR_ENDIF,    "endif" },
    { DIR_IF_TRUE,  "ifdef" },
    { DIR_IF_TRUE,  "ifndef"},
    { DIR_IF_TRUE,  "ifbeq" },
};


enum DIRCommand do_directive_token(State *s, TokensListElement *ptr, int skip) {
    LOG(4, "Processing directive token:\n");
    LOGDO(4, token_print(ptr->token));
    
    char *f = ptr->token->stripped + 1;

    if (skip) {
        for (int i = 0; i<sizeof(skipProcessors)/sizeof(skipProcessors[0]); i++) {
            if (strncmp(f, skipProcessors[i].name, strlen(skipProcessors[i].name))==0) {
                return skipProcessors[i].ret;
            }
        }
        return DIR_NOP;
    }

    for (int i = 0; i<sizeof(processors)/sizeof(processors[0]); i++) {
        if (strncmp(f, processors[i].name, strlen(processors[i].name))==0) {
            return processors[i].p(s, ptr);
        }
    }
    
    ERROR("Unknown directive: %s\n", f);
    token_print(ptr->token);
    return DIR_STOP;
}
