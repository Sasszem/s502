#ifndef GUARD_INSTRUCTIONS
#define GUARD_INSTRUCTIONS

#include "addressmode.h"

/**
 * @file
 * @brief Instruction data and related operations
 * 
 * Public interface for instructions.c
 */

enum {
    /// An invalid opcode to signal invalid / non-existent variations
    OPC_INVALID = 0xff
};

/**
 * @class Instruction
 * @brief linked list member holding instruction data
 *
 * Member of a list which holds data about instructions and opcodes.<br>
 * This list should be loaded from file using instruction_load()
 */
typedef struct Instruction {
    /// 3-letter mnemonic of an instruction + trailing 0
    char mnem[4];
    /// Different combinarions of this instruction. Invalid ones are set to ::OPC_INVALID
    unsigned char opcs[ADRM_COUNT];
    /// Next instruction pointer on NULL
    struct Instruction* next;
} Instruction;


/**
 * @memberof Instruction
 * @brief Load instruction data from CSV file
 * @param fname CSV file name/path to load from
 * @returns loaded instructions or NULL on error
 *
 * Opens the file, parses CSV and returns instruction data.<br>
 * Checks for simple problems. Prints errors.<br>
 * Returns NULL on error, and linked list (withut sentinel) otherwise.<br>
 * Works both on Windows and Linux platforms with the same files.
 */
Instruction* instruction_load(char* fname);

/**
 * @memberof Instruction
 * @brief find the Instruction entry for a given mnemonic
 * @param list linked list head containing loaded instructions
 * @param mnem pointer to string to seatch for
 * @returns instruction list entry or NULL if not found
 *
 * Searches the linked list for an instruction entry.<br>
 * Only checks the first 3 characters, so there is no need to copy the data on the caller side.
 */
Instruction* instruction_find(Instruction* list, char* mnem);

/**
 * @memberof Instruction
 * @brief free all memory associated with an Instruction* list
 * @param list list to free
 *
 * Frees all memory associated with the instruction data linked list, including the list head.<br>
 * Invalidates all pointers to any data within the list, so they should be all NULL-ed! 
 */
void instruction_free(Instruction* list);

/**
 * @memberof Instruction
 * @brief fancy-print one instruction data
 * @param instr instruction list entry to print
 * 
 * Fancy-print all data associated with one instruction (mnemonic, address modes and opcodes for them).<br>
 * Useful for error reporting and debugging.
 */
void instruction_print(Instruction* instr);

/**
 * @memberof Instruction
 * @brief debug-print all instructions in a list
 * @param list list of instructions to print
 *
 * Fancy-print all instructions from the list using instruction_print()<br>
 * Useful for debugging
 */
void instruction_print_all(Instruction* list);

#endif