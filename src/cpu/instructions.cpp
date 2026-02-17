#include "cpu/instructions.h"
#include "cpu/flags.h"
#include "cpu/opcodes.h"
#include "cpu/cpu.h"
#include <cstdio>
static int count = 0;
static inline u16 read_u16(CPU &cpu)
{
    return cpu.mem->read(cpu.pc + 1) | (cpu.mem->read(cpu.pc + 2) << 8);
}

static inline void push(CPU &cpu, u16 val)
{
    cpu.mem->write(--cpu.sp, (val >> 8) & 0xFF);
    cpu.mem->write(--cpu.sp, val & 0xFF);
}

static inline u16 pop(CPU &cpu)
{
    u8 lo = cpu.mem->read(cpu.sp++);
    u8 hi = cpu.mem->read(cpu.sp++);
    return (hi << 8) | lo;
}

static inline u8 read_reg(CPU &cpu, u8 code)
{
    switch (code)
    {
    case 0:
        return cpu.b;
    case 1:
        return cpu.c;
    case 2:
        return cpu.d;
    case 3:
        return cpu.e;
    case 4:
        return cpu.h;
    case 5:
        return cpu.l;
    case 6:
        return cpu.mem->read(cpu.HL()); // M
    case 7:
        return cpu.a;
    }
    return 0;
}

static inline void write_reg(CPU &cpu, u8 code, u8 val)
{
    switch (code)
    {
    case 0:
        cpu.b = val;
        break;
    case 1:
        cpu.c = val;
        break;
    case 2:
        cpu.d = val;
        break;
    case 3:
        cpu.e = val;
        break;
    case 4:
        cpu.h = val;
        break;
    case 5:
        cpu.l = val;
        break;
    case 6:
        cpu.mem->write(cpu.HL(), val);
        break; // M
    case 7:
        cpu.a = val;
        break;
    }
}

static inline void add_to_a(CPU &cpu, u8 value, bool with_carry)
{
    u8 carry = with_carry ? cpu.flags.c : 0;
    u16 result = cpu.a + value + carry;

    cpu.flags.ac = (((cpu.a & 0x0F) + (value & 0x0F) + carry) > 0x0F);
    cpu.flags.c = (result > 0xFF);

    cpu.a = result & 0xFF;
    setZSP(cpu.flags, cpu.a);
}

static inline void sub_from_a(CPU &cpu, u8 value, bool with_borrow)
{
    u8 borrow = with_borrow ? cpu.flags.c : 0;
    u16 result = cpu.a - value - borrow;

    cpu.flags.ac = ((cpu.a & 0x0F) < ((value & 0x0F) + borrow));
    cpu.flags.c = (result > 0xFF);

    cpu.a = result & 0xFF;
    setZSP(cpu.flags, cpu.a);
}

static inline void ana_a(CPU &cpu, u8 value)
{
    cpu.flags.ac = ((cpu.a | value) & 0x08) != 0;
    cpu.a = cpu.a & value;
    cpu.flags.c = 0;
    setZSP(cpu.flags, cpu.a);
}

static inline void xra_a(CPU &cpu, u8 value)
{
    cpu.a = cpu.a ^ value;
    cpu.flags.c = 0;
    cpu.flags.ac = 0;
    setZSP(cpu.flags, cpu.a);
}

static inline void ora_a(CPU &cpu, u8 value)
{
    cpu.a = cpu.a | value;
    cpu.flags.c = 0;
    cpu.flags.ac = 0;
    setZSP(cpu.flags, cpu.a);
}

static inline void cmp_a(CPU &cpu, u8 value)
{
    u16 result = (u16)cpu.a - (u16)value;
    u8 res8 = result & 0xFF;

    cpu.flags.z = (res8 == 0);
    cpu.flags.s = (res8 & 0x80) != 0;
    cpu.flags.p = parity(res8);
    cpu.flags.c = (result > 0xFF);
    cpu.flags.ac = ((cpu.a & 0x0F) < (value & 0x0F));
}

int execute_instruction(CPU &cpu)
{
    u8 opcode = cpu.mem->read(cpu.pc);

    // MOV r1, r2 : 0x40 – 0x7F (except 0x76)
    if ((opcode & 0xC0) == 0x40)
    {

        if (opcode == 0x76)
        { // HLT
            cpu.halted = true;
            cpu.pc += 1;
            return opcode_table[opcode].cycles;
        }

        u8 dst = (opcode >> 3) & 0x07;
        u8 src = opcode & 0x07;

        u8 val = read_reg(cpu, src);
        write_reg(cpu, dst, val);
        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    // ADD / ADC group: 0x80–0x8F
    if ((opcode & 0xF0) == 0x80)
    {

        u8 src = opcode & 0x07;
        bool is_adc = (opcode & 0x08) != 0;

        u8 value;
        if (src == 6)
            value = cpu.mem->read(cpu.HL());
        else
            value = read_reg(cpu, src);

        add_to_a(cpu, value, is_adc);

        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    // SUB / SBB group: 0x90–0x9F
    if ((opcode & 0xF0) == 0x90)
    {

        u8 src = opcode & 0x07;
        bool is_sbb = (opcode & 0x08) != 0;

        u8 value;
        if (src == 6)
            value = cpu.mem->read(cpu.HL());
        else
            value = read_reg(cpu, src);

        sub_from_a(cpu, value, is_sbb);

        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    // ANA / XRA group: 0xA0–0xAF
    if ((opcode & 0xF0) == 0xA0)
    {

        u8 src = opcode & 0x07;
        bool is_xra = (opcode & 0x08) != 0;

        u8 value;
        if (src == 6)
            value = cpu.mem->read(cpu.HL());
        else
            value = read_reg(cpu, src);

        if (is_xra)
            xra_a(cpu, value);
        else
            ana_a(cpu, value);

        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    // ORA / CMP group: 0xB0–0xBF
    if ((opcode & 0xF0) == 0xB0)
    {

        u8 src = opcode & 0x07;
        bool is_cmp = (opcode & 0x08) != 0;

        u8 value;
        if (src == 6)
            value = cpu.mem->read(cpu.HL());
        else
            value = read_reg(cpu, src);

        if (is_cmp)
            cmp_a(cpu, value);
        else
            ora_a(cpu, value);

        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    // Rest opcodes
    switch (opcode)
    {

    case 0x00: // NOP
        break;

    case 0x01: // LXI B,d16 | loads high byte into B , low byte into C
        cpu.b = cpu.mem->read(cpu.pc + 2);
        cpu.c = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x02: // STAX B | [BC]=A stores content of register A at location BC
        cpu.mem->write(cpu.BC(), cpu.a);
        break;

    case 0x03: // INX B | BC=BC+1
        cpu.setBC(cpu.BC() + 1);
        break;

    case 0x04: // INR B | B=B+1
        cpu.flags.ac = ((cpu.b & 0x0F) == 0x0F);
        cpu.b = cpu.b + 1;
        setZSP(cpu.flags, cpu.b);
        break;

    case 0x05: // DCR B
        /*cpu.flags.ac = ((cpu.b & 0x0F) == 0x00);
        cpu.b = cpu.b - 1;
        setZSP(cpu.flags, cpu.b);
        break;*/
        cpu.flags.ac = ((cpu.b ^ (cpu.b-1) ^ 0x01) & 0x10) != 0;
        cpu.b--;
        setZSP(cpu.flags, cpu.b);
        break;

    case 0x06: // MVI B,D8
        cpu.b = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x07:
    { // RLC
        u8 msb = (cpu.a >> 7) & 1;
        cpu.a = (cpu.a << 1) | msb;
        cpu.flags.c = msb;
        break;
    }
    case 0x08: // NOP
        break;

    case 0x09:
    { // DAD B
        u32 res = cpu.HL() + cpu.BC();
        cpu.flags.c = res > 0xFFFF;
        cpu.setHL(res & 0xFFFF);
        break;
    }
    case 0x0A: // LDAX B
        cpu.a = cpu.mem->read(cpu.BC());
        break;

    case 0x0B: // DCX B
        cpu.setBC(cpu.BC() - 1);
        break;

    case 0x0C: // INR C
        cpu.flags.ac = ((cpu.c & 0x0F) == 0x0F);
        cpu.c = cpu.c + 1;
        setZSP(cpu.flags, cpu.c);
        break;

    case 0x0D: // DCR C
        cpu.flags.ac = ((cpu.c & 0x0F) == 0x00);
        cpu.c = cpu.c - 1;
        setZSP(cpu.flags, cpu.c);
        break;

    case 0x0E: // MVI C,d8
        cpu.c = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x0F:
    { // RRC
        u8 lsb = (cpu.a) & 1;
        cpu.a = (cpu.a >> 1) | (lsb << 7);
        cpu.flags.c = lsb;
        break;
    }
    case 0x10: // NOP
        break;

    case 0x11: // LXI D,d16
        cpu.e = cpu.mem->read(cpu.pc + 1);
        cpu.d = cpu.mem->read(cpu.pc + 2);
        break;

    case 0x12: // STAX D
        cpu.mem->write(cpu.DE(), cpu.a);
        break;

    case 0x13: // INX D
        cpu.setDE(cpu.DE() + 1);
        break;

    case 0x14: // INR D
        cpu.flags.ac = ((cpu.d & 0x0F) == 0x0F);
        cpu.d++;
        setZSP(cpu.flags, cpu.d);
        break;

    case 0x15: // DCR D
        cpu.flags.ac = ((cpu.d & 0x0F) == 0x00);
        cpu.d--;
        setZSP(cpu.flags, cpu.d);
        break;

    case 0x16: // MVI D,d8
        cpu.d = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x17:
    { // RAL
        u8 old_cy = cpu.flags.c;
        u8 msb = (cpu.a >> 7) & 1;
        cpu.a = (cpu.a << 1) | old_cy;
        cpu.flags.c = msb;
        break;
    }

    case 0x18: // NOP
        break;

    case 0x19:
    { // DAD D
        u32 res = cpu.HL() + cpu.DE();
        cpu.flags.c = res > 0xFFFF;
        cpu.setHL(res & 0xFFFF);
        break;
    }

    case 0x1A: // LDAX D
        cpu.a = cpu.mem->read(cpu.DE());
        break;

    case 0x1B: // DCX D
        cpu.setDE(cpu.DE() - 1);
        break;

    case 0x1C: // INR E
        cpu.flags.ac = ((cpu.e & 0x0F) == 0x0F);
        cpu.e++;
        setZSP(cpu.flags, cpu.e);
        break;

    case 0x1D: // DCR E
        cpu.flags.ac = ((cpu.e & 0x0F) == 0x00);
        cpu.e--;
        setZSP(cpu.flags, cpu.e);
        break;

    case 0x1E: // MVI E,d8
        cpu.e = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x1F:
    { // RAR
        u8 old_cy = cpu.flags.c;
        u8 lsb = cpu.a & 1;
        cpu.a = (cpu.a >> 1) | (old_cy << 7);
        cpu.flags.c = lsb;
        break;
    }

    case 0x20: // NOP
        break;

    case 0x21: // LXI H,d16
        cpu.l = cpu.mem->read(cpu.pc + 1);
        cpu.h = cpu.mem->read(cpu.pc + 2);
        break;

    case 0x22:
    { // SHLD adr
        u16 addr = cpu.mem->read(cpu.pc + 1) | (cpu.mem->read(cpu.pc + 2) << 8);
        cpu.mem->write(addr, cpu.l);
        cpu.mem->write(addr + 1, cpu.h);
        break;
    }

    case 0x23: // INX H
        cpu.setHL(cpu.HL() + 1);
        break;

    case 0x24: // INR H
        cpu.flags.ac = ((cpu.h & 0x0F) == 0x0F);
        cpu.h++;
        setZSP(cpu.flags, cpu.h);
        break;

    case 0x25: // DCR H
        cpu.flags.ac = ((cpu.h & 0x0F) == 0x00);
        cpu.h--;
        setZSP(cpu.flags, cpu.h);
        break;

    case 0x26: // MVI H,d8
        cpu.h = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x27:
    { // DAA
        u8 correction = 0;
        // Save original value and flags
        u8 orig = cpu.a;
        bool carry_in = cpu.flags.c;

        // Lower nibble correction
        if ((orig & 0x0F) > 9 || cpu.flags.ac)
            correction |= 0x06;

        // Upper nibble correction (and decide carry)
        if ((orig >> 4) > 9 || carry_in || (((orig >> 4) >= 9) && ((orig & 0x0F) > 9)))
            correction |= 0x60;

        u16 res = orig + correction;

        // Auxiliary carry: carry from bit 3 when adding correction to low nibble
        cpu.flags.ac = ((orig & 0x0F) + (correction & 0x0F)) > 0x0F;

        // Carry: carry out of the byte addition
        cpu.flags.c = (res > 0xFF);

        cpu.a = res & 0xFF;

        setZSP(cpu.flags, cpu.a);
        break;
    }

    case 0x28: // NOP
        break;

    case 0x29:
    { // DAD H
        u32 res = cpu.HL() + cpu.HL();
        cpu.flags.c = (res > 0xFFFF);
        cpu.setHL(res & 0xFFFF);
        break;
    }

    case 0x2A:
    { // LHLD adr
        u16 addr = cpu.mem->read(cpu.pc + 1) | (cpu.mem->read(cpu.pc + 2) << 8);
        cpu.l = cpu.mem->read(addr);
        cpu.h = cpu.mem->read(addr + 1);
        break;
    }

    case 0x2B: // DCX H
        cpu.setHL(cpu.HL() - 1);
        break;

    case 0x2C: // INR L
        cpu.flags.ac = ((cpu.l & 0x0F) == 0x0F);
        cpu.l++;
        setZSP(cpu.flags, cpu.l);
        break;

    case 0x2D: // DCR L
        cpu.flags.ac = ((cpu.l & 0x0F) == 0x00);
        cpu.l--;
        setZSP(cpu.flags, cpu.l);
        break;

    case 0x2E: // MVI L,d8
        cpu.l = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x2F: // CMA
        cpu.a = ~cpu.a;
        break;

    case 0x30: // NOP
        break;

    case 0x31: // LXI SP,d16
        cpu.sp = cpu.mem->read(cpu.pc + 1) | (cpu.mem->read(cpu.pc + 2) << 8);
        break;

    case 0x32:
    { // STA adr
        u16 addr = cpu.mem->read(cpu.pc + 1) | (cpu.mem->read(cpu.pc + 2) << 8);
        cpu.mem->write(addr, cpu.a);
        break;
    }

    case 0x33: // INX SP
        cpu.sp++;
        break;

    case 0x34:
    { // INR M
        u16 addr = cpu.HL();
        u8 val = cpu.mem->read(addr);
        cpu.flags.ac = ((val & 0x0F) == 0x0F);
        val++;
        cpu.mem->write(addr, val);
        setZSP(cpu.flags, val);
        break;
    }

    case 0x35:
    { // DCR M
        u16 addr = cpu.HL();
        u8 val = cpu.mem->read(addr);
        cpu.flags.ac = ((val & 0x0F) == 0x00);
        val--;
        cpu.mem->write(addr, val);
        setZSP(cpu.flags, val);
        break;
    }

    case 0x36: // MVI M,d8
        cpu.mem->write(cpu.HL(), cpu.mem->read(cpu.pc + 1));
        break;

    case 0x37: // STC
        cpu.flags.c = 1;
        break;

    case 0x38: // NOP
        break;

    case 0x39:
    { // DAD SP
        u32 res = cpu.HL() + cpu.sp;
        cpu.flags.c = (res > 0xFFFF);
        cpu.setHL(res & 0xFFFF);
        break;
    }

    case 0x3A:
    { // LDA adr
        u16 addr = cpu.mem->read(cpu.pc + 1) | (cpu.mem->read(cpu.pc + 2) << 8);
        cpu.a = cpu.mem->read(addr);
        break;
    }

    case 0x3B: // DCX SP
        cpu.sp--;
        break;

    case 0x3C: // INR A
        cpu.flags.ac = ((cpu.a & 0x0F) == 0x0F);
        cpu.a++;
        setZSP(cpu.flags, cpu.a);
        break;
        /*
        if (count < 50)
        {
            printf("INR A before: A=%02X F=%02X\n", cpu.a, cpu.flags.f);
        }

        cpu.flags.ac = ((cpu.a & 0x0F) == 0x0F);
        cpu.a++;
        setZSP(cpu.flags, cpu.a);

        if (count < 50)
        {
            printf("INR A after : A=%02X F=%02X\n\n", cpu.a, cpu.flags.f);
        }
        count++;
        break;
        */

    case 0x3D: // DCR A
        cpu.flags.ac = ((cpu.a & 0x0F) == 0x00);
        cpu.a--;
        setZSP(cpu.flags, cpu.a);
        break;

    case 0x3E: // MVI A,d8
        cpu.a = cpu.mem->read(cpu.pc + 1);
        break;

    case 0x3F: // CMC
        cpu.flags.c = !cpu.flags.c;
        break;

    case 0xC0: // RNZ
        if (!cpu.flags.z)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xC1: // POP B
        cpu.setBC(pop(cpu));
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xC2: // JNZ adr
        if (!cpu.flags.z)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xC3: // JMP adr
        cpu.pc = read_u16(cpu);
        return opcode_table[opcode].cycles;

    case 0xC4: // CNZ adr
        if (!cpu.flags.z)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xC5: // PUSH B
        push(cpu, cpu.BC());
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xC6:
    { // ADI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        add_to_a(cpu, val, false);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xC7: // RST 0
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x00;
        return opcode_table[opcode].cycles;

    case 0xC8: // RZ
        if (cpu.flags.z)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xC9: // RET
        cpu.pc = pop(cpu);
        return opcode_table[opcode].cycles;

    case 0xCA: // JZ adr
        if (cpu.flags.z)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xCC: // CZ adr
        if (cpu.flags.z)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xCD: // CALL adr
        push(cpu, cpu.pc + 3);
        cpu.pc = read_u16(cpu);
        return opcode_table[opcode].cycles;

    case 0xCE:
    { // ACI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        add_to_a(cpu, val, true);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
        break;
    }

    case 0xCF: // RST 1
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x08;
        return opcode_table[opcode].cycles;

    case 0xD0: // RNC
        if (!cpu.flags.c)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xD1: // POP D
        cpu.setDE(pop(cpu));
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xD2: // JNC adr
        if (!cpu.flags.c)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xD3: // OUT d8
        cpu.out(cpu.mem->read(cpu.pc + 1), cpu.a);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;

    case 0xD4: // CNC adr
        if (!cpu.flags.c)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xD5: // PUSH D
        push(cpu, cpu.DE());
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xD6:
    { // SUI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        sub_from_a(cpu, val, false);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xD7: // RST 2
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x10;
        return opcode_table[opcode].cycles;

    case 0xD8: // RC
        if (cpu.flags.c)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xDA: // JC adr
        if (cpu.flags.c)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xDB:
    { // IN d8
        u8 port = cpu.mem->read(cpu.pc + 1);
        cpu.a = cpu.in(port);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xDC: // CC adr
        if (cpu.flags.c)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xDE:
    { // SBI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        sub_from_a(cpu, val, true);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xDF: // RST 3
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x18;
        return opcode_table[opcode].cycles;

    case 0xE0: // RPO
        if (!cpu.flags.p)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xE1: // POP H
        cpu.setHL(pop(cpu));
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xE2: // JPO adr
        if (!cpu.flags.p)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xE3:
    { // XTHL
        u8 lo = cpu.mem->read(cpu.sp);
        u8 hi = cpu.mem->read(cpu.sp + 1);

        cpu.mem->write(cpu.sp, cpu.l);
        cpu.mem->write(cpu.sp + 1, cpu.h);

        cpu.l = lo;
        cpu.h = hi;

        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    case 0xE4: // CPO adr
        if (!cpu.flags.p)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xE5: // PUSH H
        push(cpu, cpu.HL());
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xE6:
    { // ANI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        cpu.flags.ac = ((cpu.a | val) & 0x08) != 0;
        cpu.a &= val;
        cpu.flags.c = 0;
        setZSP(cpu.flags, cpu.a);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xE7: // RST 4
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x20;
        return opcode_table[opcode].cycles;

    case 0xE8: // RPE
        if (cpu.flags.p)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xE9: // PCHL
        cpu.pc = cpu.HL();
        return opcode_table[opcode].cycles;

    case 0xEA: // JPE adr
        if (cpu.flags.p)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xEB:
    { // XCHG
        u8 td = cpu.d, te = cpu.e;
        cpu.d = cpu.h;
        cpu.e = cpu.l;
        cpu.h = td;
        cpu.l = te;
        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    case 0xEC: // CPE adr
        if (cpu.flags.p)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xEE:
    { // XRI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        cpu.a ^= val;
        cpu.flags.c = 0;
        cpu.flags.ac = 0;
        setZSP(cpu.flags, cpu.a);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xEF: // RST 5
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x28;
        return opcode_table[opcode].cycles;

    case 0xF0: // RP
        if (!cpu.flags.s)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xF1:
    { // POP PSW
        u16 psw = pop(cpu);
        cpu.flags.f = psw & 0xFF;
        cpu.flags.f |= 0x02; // bit 1 always set
        cpu.a = (psw >> 8) & 0xFF;
        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    case 0xF2: // JP adr
        if (!cpu.flags.s)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xF3: // DI
        cpu.inte = false;
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xF4: // CP adr
        if (!cpu.flags.s)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xF5:
    { // PUSH PSW
        u16 psw = (cpu.a << 8) | (cpu.flags.f | 0x02);
        push(cpu, psw);
        cpu.pc += 1;
        return opcode_table[opcode].cycles;
    }

    case 0xF6:
    { // ORI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        cpu.a |= val;
        cpu.flags.c = 0;
        cpu.flags.ac = 0;
        setZSP(cpu.flags, cpu.a);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xF7: // RST 6
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x30;
        return opcode_table[opcode].cycles;

    case 0xF8: // RM
        if (cpu.flags.s)
        {
            cpu.pc = pop(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xF9: // SPHL
        cpu.sp = cpu.HL();
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xFA: // JM adr
        if (cpu.flags.s)
        {
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xFB: // EI
        cpu.inte = true;
        cpu.pc += 1;
        return opcode_table[opcode].cycles;

    case 0xFC: // CM adr
        if (cpu.flags.s)
        {
            push(cpu, cpu.pc + 3);
            cpu.pc = read_u16(cpu);
            return opcode_table[opcode].cycles + 6;
        }
        cpu.pc += 3;
        return opcode_table[opcode].cycles;

    case 0xFE:
    { // CPI d8
        u8 val = cpu.mem->read(cpu.pc + 1);
        cmp_a(cpu, val);
        cpu.pc += 2;
        return opcode_table[opcode].cycles;
    }

    case 0xFF: // RST 7
        push(cpu, cpu.pc + 1);
        cpu.pc = 0x38;
        return opcode_table[opcode].cycles;

    default:
        printf("Unimplemented opcode %02X at %04X\n", opcode, cpu.pc);
        return 0;
    }

    cpu.pc += opcode_table[opcode].bytes;
    return opcode_table[opcode].cycles;
}
