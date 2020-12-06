#include <stdlib.h>
#include <string.h>

#include "tokenslist.h"
#include "istack.h"
#include "state.h"
#include "pass_one.h"
#include "directive.h"
#include "logging.h"
#include "tokenFunc.h"
#include "addressmode.h"


int pass_one(State* s) {
    TokensListElement* ptr = s->tokens->head;

    // stack to store state of conditionals
    // top of stack is the current value
    // 1 means we disabled compilation
    // in that case we do not generate anything
    istack_ptr ifstack = istack_new();
    if (!ifstack) {
        FAIL("Pass 1 init failed!\n");
        return -1;
    }

    // return value from directive processing
    enum DIRCommand p;

    // on all tokens
    while (ptr != NULL) {
        LOG(4, "Processing token:\n");
        LOGDO(4, token_print(ptr->token));

        ///////////////
        // DIRECTIVE //
        ///////////////
        if (ptr->token->type == TT_DIRECTIVE) {
            p = do_directive_token(s, ptr, istack_top(ifstack, 0));

            if (p == DIR_IF_TRUE)
                if (istack_push(ifstack, istack_top(ifstack, 0)) < 0) goto ERR_FREE;
            if (p == DIR_IF_FALSE)
                if (istack_push(ifstack, 1) < 0) goto ERR_FREE;

            if (p == DIR_ENDIF)
                if (istack_pop(ifstack) < 0) {
                    ERROR("More endif's than if's!\n");
                    token_print(ptr->token);
                    goto ERR_FREE;
                }
            if (p == DIR_STOP) goto ERR_FREE;
        }

        /////////////////
        // INSTRUCTION //
        /////////////////
        if (ptr->token->type == TT_INSTR) {
            ptr->token->instr.address = s->PC;

            if (token_analyze_instruction(s, ptr->token) < 0) goto ERR_FREE;
            if (token_get_operand(s, ptr->token) < 0) goto ERR_FREE;

            LOGDO(3, token_print(ptr->token));
            LOG(3, "Instruction: \n");
            LOGDO(3, instruction_print(ptr->token->instr.inst));
            LOG(3, "A-mode: %s\n", ADRM_NAMES[ptr->token->instr.addressmode]);
        }

        ///////////
        // LABEL //
        ///////////
        if (ptr->token->type == TT_LABEL) {
            if (ptr->token->len>=MAP_MAX_KEY_LEN) {
                ERROR("Label is too long!");
                token_print(ptr->token);
                goto ERR_FREE;
            }

            char labelname[MAP_MAX_KEY_LEN];
            // pragmas are here because GCC would generate a warning
            // (thus an error)
            // that I truncate the string
            // it's right of course, but also that's my intetntion
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wstringop-truncation"
            strncpy(labelname, ptr->token->stripped, MAP_MAX_KEY_LEN-1);
            labelname[ptr->token->len - 1] = 0;
            #pragma GCC diagnostic pop
            LOG(5, "Label: %s\n", labelname);
            if (map_get(s->labels, labelname) >= 0) {
                ERROR("Can not re-define label '%s'\n", labelname);
                ERROR("(prev. value: $%x = %d\n)", map_get(s->labels, labelname), map_get(s->labels, labelname));
                token_print(ptr->token);
                goto ERR_FREE;
            }

            map_set(s->labels, labelname, s->PC);
            LOG(3, "Set label: '%s' to %d\n", labelname, s->PC);
        }

        ////////////
        // COMMON //
        ////////////
        if (istack_top(ifstack, 0) || ptr->token->binSize <= 0) {
            ptr = tokenslist_remove(s->tokens, ptr);
        } else {
            s->PC += ptr->token->binSize;
            ptr = ptr->next;
        }
    }
    istack_free(ifstack);
    return 0;


ERR_FREE:
    FAIL("Pass 1 failed!\n");
    istack_free(ifstack);
    return -1;
}