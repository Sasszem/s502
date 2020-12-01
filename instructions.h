#ifndef GUARD_INSTRUCTIONS
#define GUARD_INSTRUCTIONS

/// Possible address modes of an instruction
enum AddressMode {  
    /// Accumlator
    ADRM_ACC = 0,   
    
    /// Absolute addressing
    ADRM_ABS,       
    
    /// Absolute, X indexed
    ADRM_ABS_X,     
    
    /// Absolute, Y indexed
    ADRM_ABS_Y,     
    
    /// Immidiate
    ADRM_IMM,       
    
    /// Implied (no operand)
    ADRM_IMP,       
    
    /// Indirect (16 bit)
    ADRM_IND,       
    
    /// Indirect (8 bit), X indexed
    ADRM_IND_X,     
    
    /// Indirect (8 bit), Y indexed
    ADRM_IND_Y,     
    
    /// Relative (8bit, signed 2's complement)
    ADRM_REL,       
    
    /// Zeropage
    ADRM_ZPG,       
    
    /// Zeropage, X indexed
    ADRM_ZPG_X,     
    
    /// Zeropage, Y indexed
    ADRM_ZPG_Y,     
    
    /// Total number of addressing modes
    ADRM_COUNT      
};

/// Map address modes to binary sizes
extern int ADRM_SIZES[ADRM_COUNT + 1];

/// Map address modes to human-readable names
extern const char *ADRM_NAMES[ADRM_COUNT+1];

enum {
    /// An invalid opcode to signal invalid / non-existent variations
    OPC_INVALID = 0xff
};

/**
 * @class Instruction
 * @brief linked list member holding instruction data
 * 
 * Member of a list which holds data about instructions and opcodes.   
 * This list should be loaded from file using instruction_load()
 */
typedef struct Instruction {
    /// 3-letter mnemonic of an instruction + trailing 0
    char mnem[4];
    /// Different combinarions of this instruction. Invalid ones are set to ::OPC_INVALID
    unsigned char opcs[ADRM_COUNT];
    /// Next instruction pointer on ::NULL
    struct Instruction* next;
} Instruction;


/**
 * @memberof Instruction 
 * @brief Load instruction data from CSV file
 * @param fname CSV file name/path to load from
 * @returns loaded instructions or NULL on error
 * 
 * Opens the file, parses CSV and returns instruction data.   
 * Checks for simple problems. Prints errors.   
 * Returns ::NULL on error, and linked list (withut sentinel) otherwise.   
 * Works both on Windows and Linux platforms with the same files.   
 */
Instruction* instruction_load(char* fname);

/**
 * @memberof Instruction
 * @brief find the Instruction entry for a given mnemonic
 * @param list linked list head containing loaded instructions
 * @param mnem pointer to string to seatch for
 * @returns instruction list entry or ::NULL if not found
 * 
 * Searches the linked list for an instruction entry.   
 * Only checks the first 3 characters, so there is no need to copy the data on the caller side. 
 */
Instruction* instruction_find(Instruction* list, char* mnem);

/**
 * @memberof Instruction
 * @brief free all memory associated with an Instruction* list
 * @param list list to free
 * 
 * Frees all memory associated with the instruction data linked list, including the list head.   
 * Invalidates all pointers to any data within the list, so they should be all ::NULL-ed!
 */
void instruction_free(Instruction* list);

/**
 * @memberof Instruction
 * @brief fancy-print one instruction data
 * @param instr instruction list entry to print
 * 
 * Fancy-print all data associated with one instruction (mnemonic, address modes and opcodes for them).   
 * Useful for error reporting and debugging.
 */
void instruction_print(Instruction* instr);

/**
 * @memberof Instruction
 * @brief debug-print all instructions in a list
 * @param list list of instructions to print
 * 
 * Fancy-print all instructions from the list using instruction_print()   
 * Useful for debugging 
 */
void instruction_print_all(Instruction* list);

#endif