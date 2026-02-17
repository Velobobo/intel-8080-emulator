#include "cpu/cpu.h"
#include "cpu/opcodes.h"
#include "cpu/instructions.h"
#include <cstdio>





void CPU::reset(){
    a=b=c=d=e=h=l=0;
    pc=sp=0;
    flags.f =0x2; // bit 1 always set
    inte=false;
}

u16 CPU::BC() {
    return (u16(b) << 8) | u16(c);
}

u16 CPU::DE() {
    return (u16(d) << 8) | u16(e);
}

u16 CPU::HL() {
    return (u16(h) << 8) | u16(l);
}

void CPU::setBC(u16 v) {
    b = (v >> 8) & 0xFF;
    c = v & 0xFF;
}

void CPU::setDE(u16 v) {
    d = (v >> 8) & 0xFF;
    e = v & 0xFF;
}

void CPU::setHL(u16 v) {
    h = (v >> 8) & 0xFF;
    l = v & 0xFF;
}

int CPU::step() {
    return execute_instruction(*this);
}

u8 CPU::in(u8 port) {
    return 0x00; // change this
}

void CPU::out(u8 port, u8 value) {
    // chnange
}
