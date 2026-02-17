#include <cstdio>
#include <cstring>
#include "cpu/cpu.h"
#include "cpu/load.h"

bool loadROM(CPU* cpu, const char* path, u16 offset) {
    if (!cpu || !cpu->mem) {
        printf("CPU or memory not initialized\n");
        return false;
    }

    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Failed to open ROM: %s\n", path);
        return false;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (offset + size > 0x10000) {
        printf("ROM too large to fit in memory\n");
        fclose(f);
        return false;
    }

    size_t read = fread(cpu->mem->data + offset, 1, size, f);
    fclose(f);

    if (read != (size_t)size) {
        printf("Failed to read full ROM\n");
        return false;
    }

    printf("Loaded ROM: %s (%ld bytes) at 0x%04X\n",
           path, size, offset);

    return true;
}
