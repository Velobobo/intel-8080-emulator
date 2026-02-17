#include "cpu/cpu.h"
#include "memory/memory.h"
#include "cpu/load.h"
#include <iostream>
#include <filesystem>

int main(void)
{
    std::cout << "CWD = " << std::filesystem::current_path() << "\n";
    Memory mem;
    mem.reset();

    CPU cpu;
    cpu.mem = &mem;
    cpu.reset();

    loadROM(&cpu, "roms/testing/CPUTEST.COM", 0x100);
    cpu.pc = 0x100;

    while (true)
    {
        int cycles = cpu.step();
        if ((cpu.flags.f & 0x02) == 0)
        {
            printf("ERROR: flag bit1 cleared at PC=%04X\n", cpu.pc);
            exit(1);
        }

        // BDOS trap
        if (cpu.pc == 0x0005)
        {
            if (cpu.c == 9)
            {
                // print string
                u16 addr = cpu.DE();
                char ch;
                while ((ch = cpu.mem->read(addr++)) != '$')
                    putchar(ch);
            }
            else if (cpu.c == 2)
            {
                // print char
                putchar(cpu.e);
            }
            else if (cpu.c == 0)
            {
                // PROGRAM TERMINATION
                printf("\n[BDOS] Program terminated\n");
                return 0; // or set cpu.halted = true;
            }

            // simulate RET
            cpu.pc = cpu.mem->read(cpu.sp) | (cpu.mem->read(cpu.sp + 1) << 8);
            cpu.sp += 2;
        }

        if (cycles == 0)
        {
            printf("ERROR: Unimplemented opcode at PC=%04X\n", cpu.pc);
            printf("Opcode = %02X\n", cpu.mem->read(cpu.pc));
            break;
        }
    }

    std::cout << "Finished\n";
    return 0;
}
