#include "addressmode.h"

/**
 * @file
 * @brief constant data related to AddressMode
 *
 * Textual names and binary sizes of operands
 */


 /**
  * @brief Human-readable names of address modes
  *
  * Can be used to pretty print if indexed by enum value
  */
const char* ADRM_NAMES[] = {
    "Accumulator",
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


/**
 * @brief operand sizes of addressmodes
 */
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
