#pragma once
#include "util/types.h"

struct Flags{
    union {
        u8 f;
        struct {
            u8 c :1; //bit 0 carry
            u8 one:1; // bit 1 ,always 1
            u8 p:1; // bit 2 parity
            u8 :1; // bit 3 unused
            u8 ac:1; // bit 4  aux carry
            u8 :1; // bit 5 unused
            u8 z:1; // bit 6 zero
            u8 s:1; // bit 7 sign
        };

    };
};


u8 parity(u8 v);
void setZSP(Flags& f, u8 v);