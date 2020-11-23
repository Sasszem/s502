
#include <stdio.h>
#include <errno.h>

#include "logging.h"
#include "number.h"
#include "debugmalloc.h"
#include "instructions.h"


/**
 * @brief Load instruction and opcode info from file
 * @param fname file name to load from
 * @return Linked list with instructions or NULL on error
 */
Instruction* instruction_load(char* fname) {
    FILE* f = fopen(fname, "r");
    if (f == NULL) {
        ERROR("An error occured opening the file %s!\n", fname);
        ERROR("Error opening file: %s\n", strerror(errno));
        return NULL;
    }

    // ignore first line
    // (Also possibly has BOM for unicode)

    int x;
    while (x = fgetc(f), x != '\n' && x != EOF);
    if (x == EOF) {
        goto ERR_MALFORMED;
    }
    Instruction* list = malloc(sizeof(Instruction));
    if (list==NULL) goto ERR_MEM;
    list->next = NULL;
    Instruction* curr = list;

    char buff[16];

    int rowindex = 0;
    int ptr = 0;
    int row = 1;
    while (x = fgetc(f), x != EOF) {
        if (x == ';' || x == '\n') {
            // end of column
            buff[ptr] = 0;

            // process data
            if (rowindex == 0) {
                if (ptr != 3) goto ERR_MALFORMED;
                memcpy(curr->mnem, buff, 3);
                curr->mnem[3] = 0;
            }             else {
                if (ptr != 2 && ptr != 0) goto ERR_MALFORMED;
                if (ptr)
                    curr->opcs[rowindex - 1] = 16 * number_char_to_digit(buff[0]) + number_char_to_digit(buff[1]);
                else
                    curr->opcs[rowindex - 1] = OPC_INVALID;
            }
            ptr = 0;
            rowindex++;
            if (x == '\n') {
                if (rowindex != 14)
                    goto ERR_MALFORMED;
                rowindex = 0;
                row++;

                curr->next = malloc(sizeof(Instruction));
                if (curr->next==NULL) goto ERR_MEM;
                curr->next->next = NULL;
                curr = curr->next;
            }


            continue;
        }
        buff[ptr++] = x;
    }

    // file might had a trailing newline
    // in that case the last entry is not valid

    if (rowindex == 0) {
        for (curr = list; curr->next != NULL && curr->next->next != NULL; curr = curr->next);
        free(curr->next);
        curr->next = NULL;
    }

    fclose(f);
    return list;
ERR_MALFORMED:
    ERROR("Malformed instruction line:column: %d : %d\n", row + 1, rowindex + 1);
    ERROR("Malformed instructions/opcodes file: %s\n", fname);
    goto CLEANUP;

ERR_MEM:
    ERROR("Memory allocation error in instruction_load()!\n");
    goto CLEANUP;

CLEANUP:
    instruction_free(list);
    fclose(f);

    return NULL;
}

/**
 * @brief find an instruction based on it's mnemonic. Only checks 3 chars (no need to copy anything)
 * @return Instr* pointer or NULL
 */
Instruction* instruction_find(Instruction *list, char *mnem) {
    Instruction *ptr;
    for (ptr = list; ptr != NULL; ptr = ptr->next) {
        if (memcmp(mnem, ptr->mnem, 3)==0)
            return ptr;
    }
    return NULL;
}

/**
 * @brief free all memory associated with an Instruction* linked list
 * @param list list to free. All references to it will be invalid! Also accepts NULL
 */
void instruction_free(Instruction* list) {
    if (!list) return;
    while (list->next != NULL) {
        Instruction* ptr = list->next;
        list->next = ptr->next;
        free(ptr);
    }
    free(list);
}

const char* ADRM_NAMES[] = {
    "Accumlator",
    "Absolute",
    "Absolute, X",
    "Absolute, Y",
    "Immidiate",
    "Implied",
    "Indirect",
    "Indirect, X",
    "Indirect, Y",
    "Relative",
    "Zeropage",
    "Zeropage, X",
    "Zeropage, Y",
    "ERROR! OVERINDEX!",
};

int ADRM_SIZES[ADRM_COUNT + 1] = {
    0,      // ACC
    2,      // ABS
    2,      // ABS_X
    2,      // ABS_Y
    1,      // IMM
    0,      // IMP
    2,      // IND
    1,      // IND_X
    1,      // IND_Y
    1,      // REL
    1,      // ZPG
    1,      // ZPG_X
    1,      // ZPG_Y
    -1,
};

/**
 * @brief debug-print one instruction
 * @param instr the instruction to print
 */
void instruction_print(Instruction *instr) {
    printf("%s", instr->mnem);
    for (int i = 0; i < ADRM_COUNT; i++)
        if (instr->opcs[i] != OPC_INVALID) {
            printf("\t%s:\t%x\n", ADRM_NAMES[i], instr->opcs[i]);
        }
    printf("\n");
}

/**
 * @brief debug-print all loaded instructions
 * @param list a linked list containing the instructions
 */
void instruction_print_all(Instruction* list) {
    for (; list != NULL; list = list->next) {
        instruction_print(list);
    }
}