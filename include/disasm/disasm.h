#pragma once
#include "util/types.h"

int disasm(const u8* code,u16 pc);
void disasm_all(const u8* code, u16 start, u16 size);