#pragma once
#include "util/types.h"

struct Opcode{
    const char* mnemonic;
    u8 bytes;
    u8 cycles; // base cycles (not taken)
};

extern Opcode opcode_table[256];