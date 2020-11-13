#ifndef GUARD_TOKEN
#define GUARD_TOKEN

// TODO: define-t enumra cserélni
#define TOKEN_BUFFER_SIZE 32
#define TOKEN_SOURCE_FILE_SIZE 32


enum tokenType {
    TT_INSTR,
    TT_PREPROC,
    TT_LABEL
};

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


struct TokenLabel {
    int addr;
};

typedef struct {
    int binSize;
    enum tokenType type;
    union {
        struct TokenLabel label;
        struct {
            enum AddressMode addressmode;
            int number;
        } instr;
    } fields;
    char stripped[TOKEN_BUFFER_SIZE];
    int len;
    struct {
        char fname[TOKEN_SOURCE_FILE_SIZE];
        int lineno;
    } source;
} Token;

void token_print(Token *token);

#endif