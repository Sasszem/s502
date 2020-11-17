#include <string.h>
#include <stdlib.h>

#include "state.h"
#include "token_t.h"
#include "tokenFunc.h"
#include "logging.h"
#include "preprocess.h"
#include "util.h"
#include "number.h"
#include "loadfile.h"

/**
 * Internal command type for preprocessor
 * Token processor functions return these to signal different commands to the preprocessor
 */
enum PPCommand {
    PPC_STOP = -1,
    PPC_NOP = 0,
    PPC_IF_TRUE,
    PPC_IF_FALSE,
    PPC_ENDIF,
};

/**
 * Token processor function
 * Should process one token and return a PPCommand
 */
typedef enum PPCommand(*tokenprocessor)(State *, TokensList *, TokensListElement*);


/**
 * Process a .define directive
 * Updates definese
 */
enum PPCommand process_define(State *s, TokensList *list, TokensListElement *ptr) {
    // find string
    char *define = ptr->token->stripped + 1;
    
    char *name = util_find_string_segment(define) + 1;
    if (*(name-1)=='\0') {
        ERROR("Not enough args to define!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }

    char *num = util_find_string_segment(name) + 1;
    if (*(num-1)=='\0') {
        ERROR("Not enough args to define!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }

    char *nend = util_find_string_segment(num);
    if (*nend!='\0') {
        ERROR("Too many args to define!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }

    //LOG("First part:\t\t%.*s\n", name-define, define);
    //LOG("Second part:\t\t%.*s\n", num-name, name);
    //LOG("Third part:\t\t%.*s\n", nend-num, num);
    int as_num = number_get_number(s, num, nend-num);
    if (as_num<0) {
        FAIL("Define argument is not valid!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }
    char def[DEFINE_MAX_LEN];
    strncpy(def, name, num-name);
    def[num-name-1] = 0;
    if (map_set(s->defines, def, as_num)<0) {
        FAIL("process_define() failed!\n");
        return PPC_STOP;
    }
    return PPC_NOP;
}

/**
 * Process an ifbeq directive
 * Can return PPC_IF_TRUE or PPC_IF_FALSE
 */
enum PPCommand process_ifbeq(State *s, TokensList *list, TokensListElement *ptr) {
    // find string
    char *ifbeq = ptr->token->stripped + 1;
    
    char *first = util_find_string_segment(ifbeq) + 1;
    if (*(first-1)=='\0') {
        ERROR("Not enough args to ifbeq!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }

    char *second = util_find_string_segment(first) + 1;
    if (*(second-1)=='\0') {
        ERROR("Not enough args to ifbeq!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }

    char *send = util_find_string_segment(second);
    if (*send!='\0') {
        ERROR("Too many args to ifbeq!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }

    //LOG("First part:\t\t%.*s\n", name-define, define);
    //LOG("Second part:\t\t%.*s\n", num-name, name);
    //LOG("Third part:\t\t%.*s\n", nend-num, num);
    int first_as_num = number_get_number(s, first, second-first-1);
    if (first_as_num<0) {
        FAIL("ifbeq first argument not defined!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }
    int second_as_num = number_get_number(s, second, send-second);
    if (second_as_num<0) {
        FAIL("ifbeq second argument not defined!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }
    return first_as_num>second_as_num ? PPC_IF_TRUE : PPC_IF_FALSE;
}

/**
 * "Process" an endif directive
 */
enum PPCommand process_endif(State *s, TokensList *list, TokensListElement *ptr){
    return PPC_ENDIF;
}

/**
 * Process a print directive
 */
enum PPCommand process_print(State *s, TokensList *list, TokensListElement *ptr) {
    char *str = &(ptr->token->stripped[1]);
    str = util_find_string_segment(str) + 1;
    printf("\e[44mMESSAGE\e[49m\t%s\n", str);
    return PPC_NOP;
}


/**
 * Process a printc directive
 */
enum PPCommand process_printc(State *s, TokensList *list, TokensListElement *ptr) {
    char *str = &(ptr->token->stripped[1]);
    str = util_find_string_segment(str) + 1;
    char *send = util_find_string_segment(str);

    if (*send!='\0') {
        ERROR("Too many arguemnts to printc!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }

    char buffer[DEFINE_MAX_LEN];
    strncpy(buffer, str, send-str);
    buffer[send-str] = 0;
    int v = map_get(s->defines, buffer);
    printf("\e[44mDEFINE\e[49m:\t%s = ", buffer);

    if (v==-1)
        printf("\e[31mNOT DEFINED\e[39m\n");
    else
        printf("%d\n", v);
    return PPC_NOP;
}

/**
 * Process an include directive
 * This modifies the tokenslist directly!
 */
enum PPCommand process_include(State *s, TokensList *list, TokensListElement *ptr) {
    char *str = &(ptr->token->stripped[1]);
    str = util_find_string_segment(str) + 1;
    if (*(str-1)=='\0') {
        ERROR("Too few argumenst to include!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }
    char *send = util_find_string_segment(str);
    if (*send!='\0') {
        ERROR("Too many arguments to include!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }
    LOG("Including '%.*s'\n", (int)(send-str), str);
    char name[FILENAME_MAX];
    strncpy(name, str, send-str);
    TokensList *f = load_file(name);
    if (f==NULL) {
        FAIL("Could not include file!\n");
        token_print(ptr->token);
        return PPC_STOP;
    }
    tokenslist_insert(list, ptr, f);
    tokenslist_free(f);
    f = NULL;
    return PPC_NOP;
}

/**
 * Process an ifdef directive 
 */
enum PPCommand process_ifdef(State *s, TokensList *list, TokensListElement *ptr) {
    char *cmd = &(ptr->token->stripped[1]);
    char *val = util_find_string_segment(cmd) + 1;
    if (*(val-1)!=' ') {
        ERROR("Too few arguments for ifdef!");
        token_print(ptr->token);
        return PPC_STOP;
    }
    char *vend = util_find_string_segment(val);
    if (*vend!='\0'){
        ERROR("Too many arguments for ifdef!");
        token_print(ptr->token);
        return PPC_STOP;
    }
    char name[DEFINE_MAX_LEN];
    strncpy(name, val, vend-val);
    name[vend-val] = 0;
    if (map_get(s->defines, name)!=-1) {
        return PPC_IF_TRUE;
    }
    return PPC_IF_FALSE;
}

/**
 * Process an ifndef directive
 * Uses process_ifdef internally
 */
enum PPCommand process_ifndef(State *s, TokensList *list, TokensListElement *ptr) {
    enum PPCommand ret = process_ifdef(s, list, ptr);
    if (ret==PPC_IF_FALSE)
        return PPC_IF_TRUE;
    if (ret==PPC_IF_TRUE)
        return PPC_IF_FALSE;
    return PPC_STOP;
}


/**
 * The list of all processor functions and their tokens
 * Currently compare is kinda broken, so their order DOES matter
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
};

/**
 * List of tokens to "process" when skipping tokens due to a falsy if
 */
struct {enum PPCommand ret; char *name;} skipProcessors[] = {
    { PPC_ENDIF,    "endif" },
    { PPC_IF_TRUE,  "ifdef" },
    { PPC_IF_TRUE,  "ifndef"},
    { PPC_IF_TRUE,  "ifbeq" },
};


/**
 * @brief very simple int stack
 */
typedef struct _stack {
    int val;
    struct _stack *next; 
} stack_el, *stack_ptr;

/**
 * @brief create an empty stack
 * @returns stack pointer or NULL on error
 */
stack_ptr stack_new() {
    stack_ptr head = malloc(sizeof(stack_el));
    if (!head) {
        ERROR("Memory allocation error in stack_new()!\n");
        return NULL;
    }
    head->next = NULL;
    return head;
}

/**
 * @brief check if the stack is empty or not
 * @param stack the stack to check
 * @returns 1 if empty, 0 if not
 */
int stack_empty(stack_ptr stack) {
    return (!stack->next);
};

/**
 * @brief push one element to the stack
 * @param stack stack to push to
 * @param val value to push
 * @returns 0 on success, -1 on error
 */
int stack_push(stack_ptr stack, int val) {
    stack_ptr elem = malloc(sizeof(stack_el));
    if (!elem) {
        ERROR("Memory allocation error in stack_push()!\n");
        return -1;
    }
    elem->val = val;
    elem->next = stack->next;
    stack->next = elem;
    return 0;
}

/**
 * @brief pop the top of the stack
 * @param stack the stack to pop from
 * @returns 0 on success, -1 on error (popping from an empty stack)
 */
int stack_pop(stack_ptr stack) {
    if (stack_empty(stack)) return -1;
    stack_ptr elem = stack->next;
    stack->next = elem->next;
    free(elem);
    return 0;
}

/**
 * @brief get the top element of the stack
 * @param stack the stack to get the top of
 * @param def default value if the stack is empty
 * @returns top of the stack or def
 */
int stack_top(stack_ptr stack, int def) {
    if (stack_empty(stack)) return def;
    return stack->next->val;
};



/**
 * @brief free all memory associated with a stack
 * @param stack stack pointer. Will be invalid after stack_free!
 */
void stack_free(stack_ptr stack) {
    while (!stack_empty(stack)) stack_pop(stack);
    free(stack);
}


/**
 * Pre-process a single token
 */
enum PPCommand do_preprocessor_token(State *s, TokensList *list, TokensListElement *ptr, int skip) {
    LOG("Processing preproessor token:\n");
    LOGDO(token_print(ptr->token));
    
    char *f = ptr->token->stripped + 1;

    if (skip) {
        for (int i = 0; i<sizeof(skipProcessors)/sizeof(skipProcessors[0]); i++) {
            if (strncmp(f, skipProcessors[i].name, strlen(skipProcessors[i].name))==0) {
                return skipProcessors[i].ret;
            }
        }
        return PPC_NOP;
    }

    for (int i = 0; i<sizeof(processors)/sizeof(processors[0]); i++) {
        if (strncmp(f, processors[i].name, strlen(processors[i].name))==0) {
            return processors[i].p(s, list, ptr);
        }
    }
    
    ERROR("Unknown preprocessor directive: %s\n", f);
    token_print(ptr->token);
    return PPC_STOP;
}

/**
 * Preprocess the tokens
 * - Handles conditionals
 * - (replace constanst inside opcodes)
 */
int preprocess(State *s, TokensList *tokens) {
    TokensListElement *ptr = tokens->head;
    stack_ptr ifstack = stack_new();
    if (!ifstack) {
        FAIL("Pass 1 init failed!\n");
        return -1;
    }
    int skiponce = 0;
    enum PPCommand p;
    while (ptr!=NULL) {
        if (ptr->token->type == TT_PREPROC) {
            p = do_preprocessor_token(s, tokens, ptr, stack_top(ifstack, 0));
            skiponce = 1;
            if (p==PPC_IF_TRUE) {
                if (stack_push(ifstack, stack_top(ifstack, 0))<0) {
                    FAIL("Pass one failed!\n");
                    stack_free(ifstack);
                    return -1;
                }
            }
            if (p==PPC_IF_FALSE) {
                if (stack_push(ifstack, 1)<0) {
                    FAIL("Pass one failed!\n");
                    stack_free(ifstack);
                    return -1;
                }
            }

            if (p==PPC_ENDIF) {
                if (stack_pop(ifstack)<0) {
                    ERROR("More endif's than if's!\n");
                    token_print(ptr->token);
                    stack_free(ifstack);
                    FAIL("Pass 1 failed!\n");
                    return -1;
                }
            }
            if (p==PPC_STOP) {
                FAIL("Pass 1 failed!\n");
                return -1;
            }
        }
        if (ptr->token->type == TT_INSTR) {
            // set number
            if (ptr->token->len>4) {
                int idx = 3;
                while (ptr->token->stripped[idx]==' ' || ptr->token->stripped[idx]=='(' || ptr->token->stripped[idx]=='#') ptr++;
                ptr->token->fields.instr.number = number_get_number(s, &ptr->token->stripped[idx], 5);
            }
        }
        if (stack_top(ifstack, 0)||skiponce){
            ptr = tokenslist_remove(tokens, ptr);
            skiponce = 0;
        } else {
            ptr = ptr->next;
        }
    }
    return 0;
}