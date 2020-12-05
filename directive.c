#include "debugmalloc.h"

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


///////////////////////////////////////////////////
// START OF INDIVIDUAL TOKEN PROCESSOR FUNCTIONS //
///////////////////////////////////////////////////


/**
 * @brief Token processor function
 * Should process one token and return a DIRCommand
 *
 * The reason why it takes a TokensListElemnt* and not just a Token* is that we need to do an insert to it's position (in require)
 */
typedef enum DIRCommand(*tokenprocessor)(State*, TokensListElement* ptr);


/**
 * @brief Process an ifbeq directive
 * @return DIR_NOP or DIR_STOP
 * @param s current state
 * @param ptr pointer to element to process
 */
enum DIRCommand process_define(State* s, TokensListElement* ptr) {

    int n;
    char **line = util_split_string(ptr->token->stripped, &n);
    
    if (n!=3) {
        ERROR("Mismatched number of arguments for '%s'\n", line[0]);
        goto ERR;
    }

    int as_num = number_get_number(s, line[2]);
    if (as_num < 0) {
        if (as_num==NUMBER_LABEL_NODEF)
            ERROR("Can not use undefined labels with define!\n");
        FAIL("Define argument is not valid!\n");
        goto ERR;
    }

    if (strlen(line[1])>MAP_MAX_KEY_LEN-1) {
        ERROR("Too long constant name! (max is %d chars)\n", MAP_MAX_KEY_LEN-1);
        goto ERR;    
    }

    char def[MAP_MAX_KEY_LEN];
    strncpy(def, line[1], MAP_MAX_KEY_LEN);

    if (map_set(s->defines, def, as_num) < 0) goto ERR;
    free(line);
    return DIR_NOP;

ERR:
    FAIL("process_define() failed!\n");
    token_print(ptr->token);
    free(line);
    return DIR_STOP;
}

/**
 * @brief Process an ifbeq directive
 * @return DIR_IF_TRUE or DIR_IF_FALSE or DIR_STOP
 */
enum DIRCommand process_ifbeq(State* s, TokensListElement* ptr) {
    int n;
    char **line = util_split_string(ptr->token->stripped, &n);

    if (n!=3) {
        ERROR("Mismatched number of arguments for '%s'\n", line[0]);
        goto ERR;
    }

    int first_as_num = number_get_number(s, line[1]);
    int second_as_num = number_get_number(s, line[2]);

    if (first_as_num < 0 || second_as_num < 0) {
        if (first_as_num==NUMBER_LABEL_NODEF || second_as_num==NUMBER_LABEL_NODEF) {
            ERROR("Can not forward-ref labels with ifbeq!\n");
        }
        else {
            FAIL("ifbeq argument not defined!\n");
        }
        goto ERR;
    }

    free(line);
    return first_as_num > second_as_num ? DIR_IF_TRUE : DIR_IF_FALSE;

ERR:
    token_print(ptr->token);
    FAIL("process_ifbeq() failed!\n");
    free(line);
    return DIR_STOP;
}

/**
 * @brief "Process" an endif directive
 * @returns DIR_ENDIF
 */
enum DIRCommand process_endif(State* s, TokensListElement* ptr) {
    return DIR_ENDIF;
}

/**
 * @brief Process a print directive
 * @returns DIR_NOP or DIR_STOP
 */
enum DIRCommand process_print(State* s, TokensListElement* ptr) {
    char* str = &(ptr->token->stripped[1]);
    str = util_find_string_segment(str) + 1;
    if (*(str - 1) == 0) {
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
enum DIRCommand process_printc(State* s, TokensListElement* ptr) {
    int n;
    char **line = util_split_string(ptr->token->stripped, &n);

    if (n!=2) {
        ERROR("Mismatched number of arguments for '%s'\n", line[0]);
        token_print(ptr->token);
        free(line);
        return DIR_STOP;
    }

    int v = map_get(s->defines, line[1]);
    printf("\e[44mDEFINE\e[49m:\t%s = ", line[1]);

    if (v < 0)
        printf("\e[31mNOT DEFINED\e[39m\n");
    else
        printf("%d\n", v);

    free(line);
    return DIR_NOP;
}

/**
 * @brief Process an include directive
 * @returns DIR_NOP or DIR_STOP
 *
 * Reads another file and inserts the tokens into the list.
 * This modifies the tokenslist directly!
 */
enum DIRCommand process_include(State* s, TokensListElement* ptr) {
    int n;
    char **line = util_split_string(ptr->token->stripped, &n);
    if (n!=2) {
        ERROR("Mismatched number of arguments for '%s'\n", line[0]);
        free(line);
        return DIR_STOP;
    }
    TokensList* f = load_file(line[1]);
    if (f == NULL) {
        FAIL("Could not include file!\n");
        token_print(ptr->token);
        free(line);
        return DIR_STOP;
    }
    tokenslist_insert(s->tokens, ptr, f);
    tokenslist_free(f);
    free(line);
    return DIR_NOP;
}

/**
 * @brief Process an ifdef directive
 * @returns DIR_STOP, DIR_IF_TRUE or DIR_IF_FALSE
 */
enum DIRCommand process_ifdef(State* s, TokensListElement* ptr) {
    int n;
    char **line = util_split_string(ptr->token->stripped, &n);

    if (n!=2) {
        ERROR("Mismatched number of arguments for '%s'\n", line[0]);
        token_print(ptr->token);
        free(line);
        return DIR_STOP;
    }
    int num = map_get(s->defines, line[1]);
    free(line);
    
    return num<0 ? DIR_IF_FALSE : DIR_IF_TRUE;
}

/**
 * @brief Process an ifndef directive
 * @returns DIR_STOP, DIR_IF_TRUE or DIR_IF_FALSE
 *
 * Uses process_ifdef internally
 */
enum DIRCommand process_ifndef(State* s, TokensListElement* ptr) {
    enum DIRCommand ret = process_ifdef(s, ptr);
    if (ret == DIR_IF_FALSE)
        return DIR_IF_TRUE;
    if (ret == DIR_IF_TRUE)
        return DIR_IF_FALSE;
    return DIR_STOP;
}

/**
 * @brief Process an org directive
 * @returns DIR_STOP or DIR_NOP
 *
 * Modifies state PC
 */
enum DIRCommand process_org(State* s, TokensListElement* ptr) {
    int n;
    char **line = util_split_string(ptr->token->stripped, &n);
    if (n!=2) {
        ERROR("Mismatched number of arguments for '%s'\n", line[0]);
        goto ERR;
    }
    int num = number_get_number(s, line[1]);
    if (num < 0) {
        if (num == NUMBER_LABEL_NODEF) {
            ERROR("Can not use undefined labels with org!\n");
        }
        FAIL("Invalid number with .org!\n");
        goto ERR;
    }
    s->PC = num;
    free(line);
    LOG(3, "PC = %d\n", num);
    return DIR_NOP;

ERR:
    free(line);
    FAIL("process_org() failed!\n");
    token_print(ptr->token);
    return DIR_STOP;
}

/**
 * @brief Process a data directive
 * @returns DIR_NOP
 *
 * Only counts size, generating binary will be done if all labels are defined (pass 3)
 */
enum DIRCommand process_data(State* s, TokensListElement* ptr) {
    // collect size
    int size = 0;
    int n;
    char** arr = util_split_string(ptr->token->stripped, &n);
    for (int i = 1; i < n; i++) {
        if (arr[i][0] == 'w')
            size += 2;
        else if (arr[i][0] == '"')
            size += strlen(arr[i]) - 2;
        else
            size += 1;
    }
    ptr->token->binSize = size;
    free(arr);
    return DIR_NOP;
}

/**
 * @brief Process a pad directive
 * @returns DIR_NOP
 *
 */
enum DIRCommand process_pad(State* s, TokensListElement* ptr) {
    int n;
    char** line = util_split_string(ptr->token->stripped, &n);

    if (2 > n || 3 < n) {
        ERROR("Mismatched number of arguments for '%s'\n", line[0]);
        goto ERR;
    }

    int target = number_get_number(s, line[1]);
    if (target < 0) {
        ERROR("Invalid argument in .pad!\n");
        goto ERR;
    }
    int size = target - s->PC;
    if (size < 0) {
        ERROR("Negative padding! PC=%x, target=%x\n", s->PC, target);
        goto ERR;
    }
    ptr->token->binSize = size;
    free(line);
    return DIR_NOP;

ERR:
    token_print(ptr->token);
    free(line);
    return DIR_STOP;
}

///////////////////////////////////////////
// END OF TOKEN PROCESSOR FUNCTIONS LIST //
///////////////////////////////////////////

/**
 * @brief The list of all processor functions and their tokens
 *
 * Their order DOES matter as comparision can not check end of strings as tokens do not end after directive names.
 * (namely printc must come before print or will get falsely reconized as a print)
 */
struct { tokenprocessor p; char* name; } processors[] = {
    { process_define,   "define"    },
    { process_ifdef,    "ifdef"     },
    { process_printc,   "printc"    },
    { process_print,    "print"     },
    { process_include,  "include"   },
    { process_endif,    "endif"     },
    { process_ifndef,   "ifndef"    },
    { process_ifbeq,    "ifbeq"     },
    { process_org,      "org"       },
    { process_data,     "data"      },
    { process_pad,      "pad"       },
};

/**
 * @brief List of tokens to "process" when skipping tokens due to a falsy if
 */
struct { enum DIRCommand ret; char* name; } skipProcessors[] = {
    { DIR_ENDIF,    "endif" },
    { DIR_IF_TRUE,  "ifdef" },
    { DIR_IF_TRUE,  "ifndef"},
    { DIR_IF_TRUE,  "ifbeq" },
};


enum DIRCommand do_directive_token(State* s, TokensListElement* ptr, int skip) {
    char* directive = ptr->token->stripped + 1;

    if (skip) {
        for (int i = 0; i < sizeof(skipProcessors) / sizeof(skipProcessors[0]); i++) {
            if (util_match_string(directive, skipProcessors[i].name, strlen(skipProcessors[i].name)) == 0) {
                return skipProcessors[i].ret;
            }
        }
        return DIR_NOP;
    }

    for (int i = 0; i < sizeof(processors) / sizeof(processors[0]); i++) {
        if (util_match_string(directive, processors[i].name, strlen(processors[i].name)) == 0) {
            return processors[i].p(s, ptr);
        }
    }

    ERROR("Unknown directive: %s\n", directive);
    token_print(ptr->token);
    return DIR_STOP;
}

//////////////////////////////////
// PASS 3 DIRECTIVE COMPILATION //
//////////////////////////////////

int compile_data(State* s, Token* t, char** dataptr) {
    char* buff = malloc(t->binSize);
    int p = 0;
    int n;
    char** arr = util_split_string(t->stripped, &n);
    for (int i = 1; i < n; i++) {
        LOG(4, ".data entry: '%s'\n", arr[i]);
        if (arr[i][0] == 'w') {
            int num = number_get_number(s, &arr[i][2]);
            if (num < 0) {
                if (num==NUMBER_LABEL_NODEF)
                    ERROR("Label '%s' is not defined!\n", &arr[i][2]);
                ERROR("Invalid word in .data!\n");
                token_print(t);
                free(arr);
                free(buff);
                *dataptr = NULL;
                return -1;
            }
            buff[p++] = num & 0xff;
            buff[p++] = (num >> 8) & 0xff;
        } else if (arr[i][0] == '"') {
            if (arr[i][strlen(arr[i]) - 1] != '"') {
                ERROR("Malformed string in .data! (no whitespaces allowed even in quotes)\n");
                token_print(t);
                free(arr);
                free(buff);
                *dataptr = NULL;
                return -1;
            }
            for (int j = 1; j < strlen(arr[i]) - 1; j++)
                buff[p++] = arr[i][j];
        } else {
            int num = number_get_number(s, arr[i]);
            if (num < 0 || num >> 8) {
                if (num==NUMBER_LABEL_NODEF)
                    ERROR("Label '%s' is not defined!\n", arr[i]);
                ERROR("Invalid byte in .data!\n");
                token_print(t);
                free(arr);
                free(buff);
                *dataptr = NULL;
                return -1;
            }
            buff[p++] = num & 0xff;
        }
    }
    *dataptr = buff;
    free(arr);
    return t->binSize;
}
int compile_pad(State* s, Token* t, char** dataptr) {
    int to = 0;
    int n;
    char** arr = util_split_string(t->stripped, &n);
    if (n == 3) {
        to = number_get_number(s, arr[2]);
        if (to < 0 || to>>8) {
            ERROR("Can not pad with invalid value!\n");
            token_print(t);
            *dataptr = NULL;
            free(arr);
            return -1;
        }
    }

    char* buff = malloc(t->binSize);
    memset(buff, to, t->binSize);
    *dataptr = buff;
    free(arr);
    return t->binSize;
}
int compile_incbin(State* s, Token* t, char** dataptr) {
    ERROR("incbin not implemented yet!\n");
    return -1;
}
int directive_compile(State* s, Token* t, char** dataptr) {
    // data, pad or incbin
    if (util_match_string(t->stripped, ".data", strlen(".data")) == 0)
        return compile_data(s, t, dataptr);
    if (util_match_string(t->stripped, ".pad", strlen(".pad")) == 0)
        return compile_pad(s, t, dataptr);
    if (util_match_string(t->stripped, ".incbin", strlen(".incbin")) == 0)
        return compile_incbin(s, t, dataptr);
    return -1;
}