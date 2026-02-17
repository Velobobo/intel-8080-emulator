#pragma once
#include "util/types.h"

struct Memory {
    u8 data[0x10000];

    u8 read(u16 addr);
    void write(u16 addr,u8 value);
    void reset();
};