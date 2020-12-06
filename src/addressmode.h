#ifndef GUARD_ADDRESSMODE
#define GUARD_ADDRESSMODE

/**
 * @file
 * @brief AddressMode enum and related data
 */

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

extern int ADRM_SIZES[ADRM_COUNT + 1];

extern const char* ADRM_NAMES[ADRM_COUNT + 1];

#endif