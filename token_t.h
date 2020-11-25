#ifndef GUARD_TOKENTYPE
#define GUARD_TOKENTYPE

// TODO: define-t enumra cserélni
#define TOKEN_BUFFER_SIZE 32
#define TOKEN_SOURCE_FILE_SIZE 32

#include "instructions.h"

enum tokenType {
    TT_INSTR,
    TT_PREPROC,
    TT_LABEL
};

struct TokenLabel {
    int addr;
};

typedef struct {
    int binSize;
    enum tokenType type;
    struct {
        enum AddressMode addressmode;
        int number;
        Instruction *inst;
    } instr;

    char stripped[TOKEN_BUFFER_SIZE];
    int len;
    struct {
        char fname[TOKEN_SOURCE_FILE_SIZE];
        int lineno;
    } source;
} Token;


#endif