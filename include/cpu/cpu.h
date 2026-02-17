#pragma once
#include "util/types.h"
#include "memory/memory.h"
#include "cpu/flags.h"

struct CPU {
    u8 a,b,c,d,e,h,l; //general purpose registers
    u16 sp,pc; 
    Flags flags;
    bool inte;
    bool halted;
    Memory* mem;

    int step();
    void reset();

    u16 BC();
    u16 DE();
    u16 HL();

    void setBC(u16 v);
    void setDE(u16 v);
    void setHL(u16 v);
    u8 in(u8 port);
    void out(u8 port, u8 value);

};