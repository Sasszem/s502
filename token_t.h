#ifndef GUARD_TOKENTYPE
#define GUARD_TOKENTYPE

enum {
    TOKEN_BUFFER_SIZE = 32,
    TOKEN_SOURCE_FILE_SIZE = 32,
};

#include "instructions.h"

/**
 * @brief possible token types
 */
enum tokenType {
    // instruction token
    TT_INSTR,
    // directive token
    TT_DIRECTIVE,
    // label token
    TT_LABEL
};

/**
 * @class Token
 * @brief Token type to store token information
 */
typedef struct {
    /// number of bytes this token will generate
    int binSize;
    /// type of this token
    enum tokenType type;
    /// instruction data. Not used if token type is not TT_INSTR
    struct {
        /// Address mode of the instruction
        enum AddressMode addressmode;
        /// operand of the instruction
        int number;
        /// pointer to instruction data
        Instruction* inst;
        /// address of instruction in resulting bytecode 
        int address;
    } instr;

    /// stripped text from source file
    char stripped[TOKEN_BUFFER_SIZE];
    /// length of stripped text
    int len;
    /// source of this token
    struct {
        /// file name of source file
        char fname[TOKEN_SOURCE_FILE_SIZE];
        /// line in source file
        int lineno;
    } source;
} Token;


#endif