#ifndef GUARD_INSTRUCTIONS
#define GUARD_INSTRUCTIONS


enum AddressMode {
    ADRM_ACC = 0,
    ADRM_ABS,
    ADRM_ABS_X,
    ADRM_ABS_Y,
    ADRM_IMM,
    ADRM_IMP,
    ADRM_IND,
    ADRM_IND_X,
    ADRM_IND_Y,
    ADRM_REL,
    ADRM_ZPG,
    ADRM_ZPG_X,
    ADRM_ZPG_Y,
    ADRM_COUNT
};

enum {
    OPC_INVALID = 0xff
};

typedef struct Instruction {
    char mnem[4];
    unsigned char opcs[ADRM_COUNT];
    struct Instruction* next;
} Instruction;

//enum AddrMode instruction_get_addr_mode(Token t);
//int instruction_is_valid(Token t, Instruction* codes);
Instruction* instruction_load(char* fname);
Instruction* instruction_find(Instruction* list, char* mnem);
void instruction_free(Instruction* list);
void instruction_print(Instruction* list);
void instruction_print_all(Instruction* list);
#endif