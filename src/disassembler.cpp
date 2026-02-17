#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "util/types.h"
#include "disasm/disasm.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <8080 binary>\n", argv[0]);
        return 1;
    }

    // 64KB memory
    static u8 memory[0x10000];
    memset(memory, 0, sizeof(memory));

    // Open file
    FILE* f = fopen(argv[1], "rb");
    if (!f) {
        perror("Failed to open file");
        return 1;
    }

    // Load at 0x0000 (8080 COM programs expect this)
    size_t bytes = fread(memory, 1, sizeof(memory), f);
    fclose(f);

    printf("Loaded %zu bytes\n\n", bytes);

    // Disassemble entire file
    disasm_all(memory, 0x0000, (u16)bytes);

    return 0;
}
