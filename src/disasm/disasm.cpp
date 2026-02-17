#include <cstdio>
#include "disasm/disasm.h"
#include "cpu/opcodes.h"

int disasm(const u8* code, u16 pc) {
    u8 opcode = code[pc];
    Opcode op = opcode_table[opcode];

    printf("%04x  %-12s", pc, op.mnemonic);

    if (op.bytes == 2)
        printf(" #%02x", code[pc+1]);
    else if (op.bytes == 3)
        printf(" #%02x%02x", code[pc+2], code[pc+1]);

    printf("\n");
    return op.bytes;
}


void disasm_all(const u8* code, u16 start, u16 size) {
    u16 pc = start;

    while (pc < start + size) {
        int bytes = disasm(code, pc);

        // Safety check (for illegal / unimplemented opcodes)
        if (bytes <= 0) {
            printf("Invalid opcode at %04x\n", pc);
            break;
        }

        pc += bytes;
    }
}
