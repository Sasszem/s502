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
    AM_IMPLIED,
    AM_INDIRECT,
    AM_INVALID
}; 

enum TokenPreprocType {
    TPT_INCLUDE,
    TPT_DEFINE,
    TPT_ORG,
    TPT_DATA,
    TPT_START
};

struct TokenLabel {
    int addr;
};

struct TokenPreproc {
    enum TokenPreprocType type;
};

struct TokenInstr {
    enum AddressMode addressmode;
};

typedef struct {
    int binSize;
    enum tokenType type;
    union {
        struct TokenLabel label;
        struct TokenPreproc preproc;
        struct TokenInstr instr;
    } fields;
    char stripped[TOKEN_BUFFER_SIZE];
    int len;
    struct {
        char fname[TOKEN_SOURCE_FILE_SIZE];
        int lineno;
    } source;
} Token;

#endif