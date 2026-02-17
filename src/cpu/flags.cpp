#include "cpu/flags.h"

u8 parity(u8 v) {
    u8 count = 0;
    for (int i = 0; i < 8; i++)
        if (v & (1 << i))
            count++;
    return (count % 2) == 0; 
}

void setZSP(Flags& f, u8 v) {
    f.z = (v == 0);
    f.s = (v & 0x80) != 0;
    f.p = parity(v);
}
