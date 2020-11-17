#include "tokenslist.h"
#include "istack.h"
#include "state.h"
#include "pass_one.h"
#include "preprocess.h"
#include <stdlib.h>
#include "logging.h"
#include "tokenFunc.h"


/**
 * @brief Compilation pass 1
 * @param s compiler state
 * @param tokens all tokens loaded so far
 * @returns 0 on success, -1 on error
 */
int pass_one(State *s, TokensList *tokens) {
    TokensListElement *ptr = tokens->head;
    istack_ptr ifstack = istack_new();
    if (!ifstack) {
        FAIL("Pass 1 init failed!\n");
        return -1;
    }
    int skiponce = 0;
    enum PPCommand p;
    while (ptr!=NULL) {
        if (ptr->token->type == TT_PREPROC) {
            p = do_preprocessor_token(s, tokens, ptr, istack_top(ifstack, 0));
            skiponce = 1;
            if (p==PPC_IF_TRUE) {
                if (istack_push(ifstack, istack_top(ifstack, 0))<0) {
                    FAIL("Pass 1 failed!\n");
                    istack_free(ifstack);
                    return -1;
                }
            }
            if (p==PPC_IF_FALSE) {
                if (istack_push(ifstack, 1)<0) {
                    FAIL("Pass 1 failed!\n");
                    istack_free(ifstack);
                    return -1;
                }
            }

            if (p==PPC_ENDIF) {
                if (istack_pop(ifstack)<0) {
                    ERROR("More endif's than if's!\n");
                    token_print(ptr->token);
                    istack_free(ifstack);
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
            /*if (ptr->token->len>4) {
                int idx = 3;
                while (ptr->token->stripped[idx]==' ' || ptr->token->stripped[idx]=='(' || ptr->token->stripped[idx]=='#') ptr++;
                ptr->token->fields.instr.number = number_get_number(s, &ptr->token->stripped[idx], 5);
            }*/
            if (token_link_instruction(s, ptr->token)<0) {
                ERROR("Unknown instruction!\n");
                token_print(ptr->token);
                FAIL("Pass 1 failed!\n");
                return -1;
            }
            if (token_get_addressmode(ptr->token)<0) {
                ERROR("Invalid instruction-addressmode combination!\n");
                ERROR("A-mode: %s\n", ADRM_NAMES[ptr->token->fields.instr.addressmode]);
                token_print(ptr->token);
                FAIL("Pass 1 failed!\n");
                return -1;
            }
            token_print(ptr->token);
            LOG("Instruction: \n");
            instruction_print(ptr->token->fields.instr.inst);
            LOG("A-mode: %s\n", ADRM_NAMES[ptr->token->fields.instr.addressmode]);
        }
        if (istack_top(ifstack, 0)||skiponce){
            ptr = tokenslist_remove(tokens, ptr);
            skiponce = 0;
        } else {
            ptr = ptr->next;
        }
    }
    return 0;
}