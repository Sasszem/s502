#ifndef GUARD_INSTRUCTIONS
#define GUARD_INSTRUCTIONS

#include "token.h"

enum {
    OPC_INVALID = 0xff
};

typedef struct Instruction {
    char mnem[4];
    unsigned char opcs[ADRM_COUNT];
    struct Instruction* next;
} Instruction;

enum AddrMode instruction_get_addr_mode(Token t);
int instruction_is_valid(Token t, Instruction* codes);
Instruction* instruction_load(char* fname);
void instruction_free(Instruction* list);
void instruction_print(Instruction* list);

#endif