#ifndef GUARD_OPCODES
#define GUARD_OPCODES

#include "token.h"


enum {
    OPC_INVALID = 0xff
};

typedef struct Opcode {
    char mnem[4];
    unsigned char opcs[ADRM_COUNT];
    struct Opcode *next;
} Opcode;

enum AddrMode opcode_get_addr_mode(Token t);
int opcode_is_valid(Token t, Opcode *codes);
Opcode* opcode_read_opcodes(char *fname);
void opcode_free(Opcode *list);
void opcode_print(Opcode *list);

#endif